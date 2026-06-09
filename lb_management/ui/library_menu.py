"""
Library Manager Console Menu.

Provides system-level operations: reports, statistics, and data persistence.
"""

from __future__ import annotations

from typing import TYPE_CHECKING

from .input_helper import (
    read_choice,
    press_enter_to_continue,
)
from .menu import HEADER

if TYPE_CHECKING:
    from ..managers.library_manager import LibraryManager


def _show_library_menu() -> None:
    """Display the Library Manager sub-menu."""
    print(HEADER)
    print("  LIBRARY MANAGER")
    print(HEADER)
    print()
    print("  1. Generate Report")
    print("  2. Show Statistics")
    print("  3. Load Data")
    print("  4. Save Data")
    print()
    print("  0. Back")
    print()


def run(lm: LibraryManager) -> None:
    """Run the Library Manager menu loop.

    Args:
        lm: The ``LibraryManager`` facade instance.
    """
    while True:
        _show_library_menu()
        choice = read_choice("  Choose: ", ["0", "1", "2", "3", "4"])

        try:
            if choice == "0":
                return
            elif choice == "1":
                _generate_report(lm)
            elif choice == "2":
                _show_statistics(lm)
            elif choice == "3":
                _load_data(lm)
            elif choice == "4":
                _save_data(lm)

        except (ValueError, KeyError) as e:
            print(f"\n  Error: {e}")
            press_enter_to_continue()
        except OSError as e:
            print(f"\n  File error: {e}")
            press_enter_to_continue()


# ------------------------------------------------------------------
# Handlers
# ------------------------------------------------------------------

def _generate_report(lm: LibraryManager) -> None:
    """Display a summary report of the library."""
    print("\n  --- Report ---")
    report = lm.generate_report()
    print()
    print(f"  Total Books:      {report['total_books']}")
    print(f"  Total Readers:    {report['total_readers']}")
    print(f"  Total Records:    {report['total_records']}")
    print(f"  Books Available:  {report['books_available']}")
    print(f"  Books Borrowed:   {report['books_borrowed']}")
    press_enter_to_continue()


def _show_statistics(lm: LibraryManager) -> None:
    """Show additional library statistics."""
    print("\n  --- Statistics ---")
    print()

    # Basic report
    report = lm.generate_report()
    print("  [Summary]")
    print(f"    Books:      {report['total_books']}")
    print(f"    Readers:    {report['total_readers']}")
    print(f"    Records:    {report['total_records']}")
    print()

    # Overdue
    overdue = lm.show_overdue_books()
    print(f"  [Overdue]      {len(overdue)} record(s)")

    # Top books
    top = lm.show_top_books(5)
    print(f"  [Top 5 Books]")
    if not top:
        print("    (No data)")
    else:
        for i, book in enumerate(top, 1):
            print(f"    {i}. {book.id} | {book.title} "
                  f"(borrowed {book.borrow_count}x)")

    # Active readers
    active = sum(1 for r in lm.readers if r.borrowed_count > 0)
    blocked = sum(1 for r in lm.readers if r.is_blocked)
    print()
    print(f"  [Readers]")
    print(f"    Active:     {active}")
    print(f"    Blocked:    {blocked}")

    press_enter_to_continue()


def _load_data(lm: LibraryManager) -> None:
    """Load data from JSON files."""
    print("\n  --- Load Data ---")
    print(f"\n  Loading from: {lm.data_dir}")

    before_books = len(lm.books)
    before_readers = len(lm.readers)
    before_records = len(lm.borrow_records)

    lm.load_data()

    after_books = len(lm.books)
    after_readers = len(lm.readers)
    after_records = len(lm.borrow_records)

    print(f"\n  Books loaded:    {after_books} (was {before_books})")
    print(f"  Readers loaded:  {after_readers} (was {before_readers})")
    print(f"  Records loaded:  {after_records} (was {before_records})")
    print("\n  Data loaded successfully.")
    press_enter_to_continue()


def _save_data(lm: LibraryManager) -> None:
    """Save current state to JSON files."""
    print("\n  --- Save Data ---")
    lm.save_data()
    print(f"\n  Data saved to '{lm.data_dir}' successfully.")
    print(f"  Books:    {len(lm.books)}")
    print(f"  Readers:  {len(lm.readers)}")
    print(f"  Records:  {len(lm.borrow_records)}")
    press_enter_to_continue()