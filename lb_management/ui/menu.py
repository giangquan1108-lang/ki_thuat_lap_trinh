"""
Main menu and application runner for the Library Management System.

Dispatches to sub-menus for Book, Reader, Borrow, and Library management.
"""

from __future__ import annotations

from typing import TYPE_CHECKING

from .input_helper import read_choice

if TYPE_CHECKING:
    from ..managers.library_manager import LibraryManager


HEADER = "=" * 45
TITLE = "LIBRARY MANAGEMENT SYSTEM"


def show_main_menu() -> None:
    """Display the top-level menu options."""
    print(HEADER)
    print(f"  {TITLE}")
    print(HEADER)
    print()
    print("  1. Book Manager")
    print("  2. Reader Manager")
    print("  3. Borrow Record Manager")
    print("  4. Library Manager")
    print("  0. Exit")
    print()


def run(lm: LibraryManager) -> None:
    """Start the main menu loop.

    Delegates to the appropriate sub-menu based on user choice.

    Args:
        lm: The ``LibraryManager`` facade instance.
    """
    from .book_menu import run as run_book_menu
    from .reader_menu import run as run_reader_menu
    from .borrow_menu import run as run_borrow_menu
    from .library_menu import run as run_library_menu

    while True:
        show_main_menu()
        choice = read_choice("  Choose: ", ["0", "1", "2", "3", "4"])

        if choice == "0":
            print("\n  Goodbye!")
            break
        elif choice == "1":
            run_book_menu(lm)
        elif choice == "2":
            run_reader_menu(lm)
        elif choice == "3":
            run_borrow_menu(lm)
        elif choice == "4":
            run_library_menu(lm)