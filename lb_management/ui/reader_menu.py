"""
Reader Manager Console Menu.

Provides an interactive text-based UI for managing library patrons.
"""

from __future__ import annotations

from typing import TYPE_CHECKING

from .input_helper import (
    read_non_empty,
    read_choice,
    confirm,
    press_enter_to_continue,
)
from .menu import HEADER

if TYPE_CHECKING:
    from ..managers.library_manager import LibraryManager


VALID_READER_TYPES = ["student", "teacher"]


def _show_reader_menu() -> None:
    """Display the Reader Manager sub-menu."""
    print(HEADER)
    print("  READER MANAGER")
    print(HEADER)
    print()
    print("  1. Add Reader")
    print("  2. Remove Reader")
    print("  3. Find Reader")
    print("  4. Block Reader")
    print("  5. Unblock Reader")
    print("  6. Show All Readers")
    print()
    print("  0. Back")
    print()


def run(lm: LibraryManager) -> None:
    """Run the Reader Manager menu loop.

    Args:
        lm: The ``LibraryManager`` facade instance.
    """
    while True:
        _show_reader_menu()
        choice = read_choice("  Choose: ", ["0", "1", "2", "3", "4", "5", "6"])

        try:
            if choice == "0":
                return
            elif choice == "1":
                _add_reader(lm)
            elif choice == "2":
                _remove_reader(lm)
            elif choice == "3":
                _find_reader(lm)
            elif choice == "4":
                _block_reader(lm)
            elif choice == "5":
                _unblock_reader(lm)
            elif choice == "6":
                _show_all_readers(lm)

        except (ValueError, KeyError) as e:
            print(f"\n  Error: {e}")
            press_enter_to_continue()


# ------------------------------------------------------------------
# Handlers
# ------------------------------------------------------------------

def _add_reader(lm: LibraryManager) -> None:
    """Prompt for reader details and register them."""
    print("\n  --- Add New Reader ---")
    student_id = read_non_empty("  Student ID: ")
    full_name = read_non_empty("  Full Name: ")

    print(f"\n  Reader types: {', '.join(VALID_READER_TYPES)}")
    reader_type = read_choice("  Reader Type: ", VALID_READER_TYPES).lower()

    from .input_helper import read_optional
    phone = read_optional("  Phone (optional): ")
    email = read_optional("  Email (optional): ")

    from ..models.reader import Reader
    reader = Reader(
        student_id=student_id,
        full_name=full_name,
        reader_type=reader_type,
        phone=phone,
        email=email,
    )
    lm.add_reader(reader)
    print(f"\n  Reader '{student_id}' added successfully.")
    press_enter_to_continue()


def _remove_reader(lm: LibraryManager) -> None:
    """Remove a reader after confirmation."""
    print("\n  --- Remove Reader ---")
    reader_id = read_non_empty("  Student ID: ")
    reader = lm.find_reader(reader_id)
    print(f"\n  Reader found: {reader.display_info()}")
    if confirm("\n  Confirm removal? (y/n): "):
        lm.remove_reader(reader_id)
        print(f"\n  Reader '{reader_id}' removed successfully.")
    else:
        print("\n  Cancelled.")
    press_enter_to_continue()


def _find_reader(lm: LibraryManager) -> None:
    """Look up and display a reader by ID."""
    print("\n  --- Find Reader ---")
    reader_id = read_non_empty("  Student ID: ")
    reader = lm.find_reader(reader_id)
    print(f"\n  {reader.display_info()}")
    press_enter_to_continue()


def _block_reader(lm: LibraryManager) -> None:
    """Block a reader from borrowing."""
    print("\n  --- Block Reader ---")
    reader_id = read_non_empty("  Student ID: ")
    reader = lm.find_reader(reader_id)
    if reader.is_blocked:
        print(f"\n  Reader '{reader_id}' is already blocked.")
    else:
        reader.block_reader()
        print(f"\n  Reader '{reader_id}' has been blocked.")
    press_enter_to_continue()


def _unblock_reader(lm: LibraryManager) -> None:
    """Restore borrowing privileges for a reader."""
    print("\n  --- Unblock Reader ---")
    reader_id = read_non_empty("  Student ID: ")
    reader = lm.find_reader(reader_id)
    if not reader.is_blocked:
        print(f"\n  Reader '{reader_id}' is not blocked.")
    else:
        reader.unblock_reader()
        print(f"\n  Reader '{reader_id}' has been unblocked.")
    press_enter_to_continue()


def _show_all_readers(lm: LibraryManager) -> None:
    """List all registered readers."""
    print("\n  --- All Readers ---")
    if not lm.readers:
        print("\n  (No readers registered.)")
    else:
        print(f"\n  Total: {len(lm.readers)} reader(s)\n")
        for i, reader in enumerate(lm.readers, 1):
            print(f"  {i:3d}. {reader.display_info()}")
    press_enter_to_continue()