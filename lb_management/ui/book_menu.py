"""
Book Manager Console Menu.

Provides an interactive text-based UI for managing the book catalogue.
"""

from __future__ import annotations

from typing import TYPE_CHECKING

from .input_helper import (
    read_non_empty,
    read_int,
    read_choice,
    press_enter_to_continue,
)
from .menu import HEADER

if TYPE_CHECKING:
    from ..managers.library_manager import LibraryManager


def _show_book_menu() -> None:
    """Display the Book Manager sub-menu."""
    print(HEADER)
    print("  BOOK MANAGER")
    print(HEADER)
    print()
    print("  1. Add Book")
    print("  2. Remove Book")
    print("  3. Update Book")
    print("  4. Find Book By ID")
    print("  5. Search Book By Title")
    print("  6. Show All Books")
    print("  7. Show Top Borrowed Books")
    print()
    print("  0. Back")
    print()


def run(lm: LibraryManager) -> None:
    """Run the Book Manager menu loop.

    Args:
        lm: The ``LibraryManager`` facade instance.
    """
    from ..models.book import Book

    while True:
        _show_book_menu()
        choice = read_choice("  Choose: ", ["0", "1", "2", "3", "4", "5", "6", "7"])

        try:
            if choice == "0":
                return

            elif choice == "1":  # Add Book ---------------
                _add_book(lm)

            elif choice == "2":  # Remove Book ------------
                _remove_book(lm)

            elif choice == "3":  # Update Book ------------
                _update_book(lm)

            elif choice == "4":  # Find Book By ID --------
                _find_book_by_id(lm)

            elif choice == "5":  # Search Book By Title ---
                _search_book_by_title(lm)

            elif choice == "6":  # Show All Books ---------
                _show_all_books(lm)

            elif choice == "7":  # Show Top Borrowed -------
                _show_top_borrowed_books(lm)

        except (ValueError, KeyError) as e:
            print(f"\n  Error: {e}")
            press_enter_to_continue()


# ------------------------------------------------------------------
# Handlers
# ------------------------------------------------------------------

def _add_book(lm: LibraryManager) -> None:
    """Prompt for book details and add to the catalogue."""
    print("\n  --- Add New Book ---")
    book_id = read_non_empty("  Book ID: ")
    title = read_non_empty("  Title: ")
    author = read_non_empty("  Author: ")
    category = read_non_empty("  Category: ")
    quantity = read_int("  Quantity: ", min_val=1)

    from ..models.book import Book

    book = Book(id=book_id, title=title, author=author, category=category,
                quantity=quantity)
    lm.add_book(book)
    print(f"\n  Book '{book_id}' added successfully.")
    press_enter_to_continue()


def _remove_book(lm: LibraryManager) -> None:
    """Remove a book by ID after confirmation."""
    print("\n  --- Remove Book ---")
    book_id = read_non_empty("  Book ID: ")
    book = lm.find_book(book_id)
    print(f"\n  Book found: {book.display_info()}")
    from .input_helper import confirm
    if confirm("\n  Confirm removal? (y/n): "):
        lm.remove_book(book_id)
        print(f"\n  Book '{book_id}' removed successfully.")
    else:
        print("\n  Cancelled.")
    press_enter_to_continue()


def _update_book(lm: LibraryManager) -> None:
    """Update a book's attributes."""
    print("\n  --- Update Book ---")
    book_id = read_non_empty("  Book ID: ")
    book = lm.find_book(book_id)
    print(f"\n  Current: {book.display_info()}")
    print()

    from .input_helper import read_optional

    title = read_optional("  New Title (leave blank to keep): ")
    author = read_optional("  New Author (leave blank to keep): ")
    category = read_optional("  New Category (leave blank to keep): ")
    qty_str = read_optional("  New Quantity (leave blank to keep): ")
    status = read_optional("  New Status [available/unavailable/lost] (leave blank to keep): ")

    kwargs = {}
    if title:
        kwargs["title"] = title
    if author:
        kwargs["author"] = author
    if category:
        kwargs["category"] = category
    if qty_str:
        try:
            qty = int(qty_str)
            if qty < 0:
                raise ValueError
            kwargs["quantity"] = qty
        except ValueError:
            print("\n  Invalid quantity — skipped.")
    if status:
        if status not in ("available", "unavailable", "lost"):
            print(f"\n  Invalid status '{status}' — skipped.")
        else:
            kwargs["status"] = status

    if kwargs:
        lm.update_book(book_id, **kwargs)
        print("\n  Book updated successfully.")
    else:
        print("\n  No changes made.")
    press_enter_to_continue()


def _find_book_by_id(lm: LibraryManager) -> None:
    """Look up and display a single book by its ID."""
    print("\n  --- Find Book By ID ---")
    book_id = read_non_empty("  Book ID: ")
    book = lm.find_book(book_id)
    print(f"\n  {book.display_info()}")
    press_enter_to_continue()


def _search_book_by_title(lm: LibraryManager) -> None:
    """Search and display books whose titles contain a substring."""
    print("\n  --- Search Book By Title ---")
    title = read_non_empty("  Search keyword: ")
    results = lm.search_book_by_title(title)
    if not results:
        print("\n  No books found matching that keyword.")
    else:
        print(f"\n  Found {len(results)} book(s):")
        for i, book in enumerate(results, 1):
            print(f"  {i}. {book.display_info()}")
    press_enter_to_continue()


def _show_all_books(lm: LibraryManager) -> None:
    """List every book in the catalogue."""
    print("\n  --- All Books ---")
    if not lm.books:
        print("\n  (No books in the system.)")
    else:
        print(f"\n  Total: {len(lm.books)} book(s)\n")
        for i, book in enumerate(lm.books, 1):
            print(f"  {i:3d}. {book.display_info()}")
    press_enter_to_continue()


def _show_top_borrowed_books(lm: LibraryManager) -> None:
    """Show the books that have been borrowed the most."""
    print("\n  --- Top Borrowed Books ---")
    top = lm.show_top_books(10)
    if not top:
        print("\n  (No books in the system.)")
    else:
        print()
        for i, book in enumerate(top, 1):
            print(f"  {i:2d}. {book.id} | {book.title} | Borrowed {book.borrow_count} times")
    press_enter_to_continue()