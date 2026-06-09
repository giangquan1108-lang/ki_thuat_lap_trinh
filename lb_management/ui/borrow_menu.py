"""
Borrow Record Manager Console Menu.

Provides an interactive text-based UI for borrow/return operations.
"""

from __future__ import annotations

from typing import TYPE_CHECKING

from .input_helper import (
    read_non_empty,
    read_choice,
    press_enter_to_continue,
)
from .menu import HEADER

if TYPE_CHECKING:
    from ..managers.library_manager import LibraryManager


def _show_borrow_menu() -> None:
    """Display the Borrow Record Manager sub-menu."""
    print(HEADER)
    print("  BORROW RECORD MANAGER")
    print(HEADER)
    print()
    print("  1. Borrow Book")
    print("  2. Return Book")
    print("  3. Find Record")
    print("  4. Show All Records")
    print("  5. Show Overdue Records")
    print()
    print("  0. Back")
    print()


def run(lm: LibraryManager) -> None:
    """Run the Borrow Record Manager menu loop.

    Args:
        lm: The ``LibraryManager`` facade instance.
    """
    while True:
        _show_borrow_menu()
        choice = read_choice("  Choose: ", ["0", "1", "2", "3", "4", "5"])

        try:
            if choice == "0":
                return
            elif choice == "1":
                _borrow_book(lm)
            elif choice == "2":
                _return_book(lm)
            elif choice == "3":
                _find_record(lm)
            elif choice == "4":
                _show_all_records(lm)
            elif choice == "5":
                _show_overdue_records(lm)

        except (ValueError, KeyError) as e:
            print(f"\n  Error: {e}")
            press_enter_to_continue()


# ------------------------------------------------------------------
# Handlers
# ------------------------------------------------------------------

def _borrow_book(lm: LibraryManager) -> None:
    """Process a book borrow request."""
    print("\n  --- Borrow Book ---")
    reader_id = read_non_empty("  Reader ID: ")
    book_id = read_non_empty("  Book ID: ")

    record = lm.borrow_book(reader_id, book_id)
    print(f"\n  Borrow successful!")
    print(f"  Record ID: {record.record_id}")
    print(f"  Due Date:  {record.due_date.strftime('%Y-%m-%d %H:%M')}")
    press_enter_to_continue()


def _return_book(lm: LibraryManager) -> None:
    """Process a book return."""
    print("\n  --- Return Book ---")
    record_id = read_non_empty("  Record ID: ")

    record = lm.return_book(record_id)
    print(f"\n  Book returned successfully!")
    print(f"  Return Date: {record.return_date.strftime('%Y-%m-%d %H:%M') if record.return_date else 'N/A'}")
    if record.late_days > 0:
        print(f"  Late Days: {record.late_days} day(s)")
        print(f"  Fine: {record.fine:,.0f} VND")
    else:
        print("  On time — no fine.")
    press_enter_to_continue()


def _find_record(lm: LibraryManager) -> None:
    """Find and display a borrow record by ID."""
    print("\n  --- Find Record ---")
    record_id = read_non_empty("  Record ID: ")

    # Linear search through all records
    for rec in lm.borrow_records:
        if rec.record_id == record_id:
            print(f"\n  {rec.display_info()}")
            press_enter_to_continue()
            return

    print(f"\n  Record '{record_id}' not found.")
    press_enter_to_continue()


def _show_all_records(lm: LibraryManager) -> None:
    """List all borrow records."""
    print("\n  --- All Borrow Records ---")
    records = lm.borrow_records
    if not records:
        print("\n  (No borrow records.)")
    else:
        print(f"\n  Total: {len(records)} record(s)\n")
        for i, rec in enumerate(records, 1):
            status_mark = " [OVERDUE]" if rec.is_overdue() else ""
            print(f"  {i:3d}. {rec.display_info()}{status_mark}")
    press_enter_to_continue()


def _show_overdue_records(lm: LibraryManager) -> None:
    """List all overdue borrow records."""
    print("\n  --- Overdue Records ---")
    overdue = lm.show_overdue_books()
    if not overdue:
        print("\n  (No overdue records — all books returned on time.)")
    else:
        print(f"\n  Total overdue: {len(overdue)} record(s)\n")
        for i, rec in enumerate(overdue, 1):
            print(f"  {i:3d}. Book: {rec.book_id} | Reader: {rec.reader_id}")
            print(f"       Due: {rec.due_date.strftime('%Y-%m-%d %H:%M')} | "
                  f"Record ID: {rec.record_id}")
            print()
    press_enter_to_continue()