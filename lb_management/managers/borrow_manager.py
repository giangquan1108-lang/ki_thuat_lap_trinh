"""
BorrowManager – Manages borrow records and the borrow/return workflow.

Coordinates with BookManager and ReaderManager to keep counts in sync.
"""

from __future__ import annotations

import uuid
from datetime import datetime, timedelta
from typing import TYPE_CHECKING

from ..models.borrow_record import BorrowRecord

if TYPE_CHECKING:
    from .book_manager import BookManager
    from .reader_manager import ReaderManager

# Default loan period in days
DEFAULT_BORROW_DAYS = 14


class BorrowManager:
    """Manages the collection of ``BorrowRecord`` objects.

    Orchestrates the full borrow/return workflow, ensuring that book
    quantities and reader borrow counts are updated atomically via
    the supplied *book_manager* and *reader_manager*.

    Responsibilities:
        - Create borrow records on successful validation.
        - Process returns, calculate late fines.
        - Query overdue records and count active borrows.
    """

    def __init__(
        self,
        book_manager: BookManager,
        reader_manager: ReaderManager,
        borrow_limit: int = 5,
        fine_per_day: float = 5000.0,
    ) -> None:
        """Initialise the borrow manager.

        Args:
            book_manager: Reference to the ``BookManager`` instance.
            reader_manager: Reference to the ``ReaderManager`` instance.
            borrow_limit: Maximum concurrent borrows per reader.
            fine_per_day: Late-return penalty rate (VND/day).
        """
        self._book_mgr = book_manager
        self._reader_mgr = reader_manager
        self.borrow_limit = borrow_limit
        self.fine_per_day = fine_per_day

        self.borrow_records: list[BorrowRecord] = []

    # ------------------------------------------------------------------
    # Borrow
    # ------------------------------------------------------------------

    def borrow_book(self, reader_id: str, book_id: str) -> BorrowRecord:
        """Process a borrow request end-to-end.

        Validates:
            - Reader exists and is not blocked.
            - Reader has not reached the borrow limit.
            - Book exists and has copies available.

        On success:
            - Creates and stores a ``BorrowRecord``.
            - Decrements the book's quantity.
            - Increments the book's total borrow count.
            - Increments the reader's active borrow count.

        Args:
            reader_id: ID of the borrowing reader.
            book_id: ID of the book to borrow.

        Returns:
            The newly created ``BorrowRecord``.

        Raises:
            ValueError: If any validation check fails.
            KeyError: If book or reader is not found.
        """
        reader = self._reader_mgr.find_reader(reader_id)
        book = self._book_mgr.find_book(book_id)

        # --- Validation ---
        if not book.is_available():
            raise ValueError(
                f"Book '{book_id}' is not available "
                f"(qty={book.quantity}, status={book.status})."
            )
        if reader.is_blocked:
            raise ValueError(f"Reader '{reader_id}' is blocked.")
        if not reader.can_borrow(self.borrow_limit):
            raise ValueError(
                f"Reader '{reader_id}' has reached the borrow limit "
                f"({reader.borrowed_count}/{self.borrow_limit})."
            )

        # --- Process borrow ---
        now = datetime.now()
        due = now + timedelta(days=DEFAULT_BORROW_DAYS)

        record = BorrowRecord(
            record_id=str(uuid.uuid4()),
            book_id=book_id,
            reader_id=reader_id,
            borrow_date=now,
            due_date=due,
        )

        book.update_quantity(-1)
        book.increase_borrow_count()
        reader.increase_borrow_count()

        self.borrow_records.append(record)
        return record

    # ------------------------------------------------------------------
    # Return
    # ------------------------------------------------------------------

    def return_book(self, record_id: str) -> BorrowRecord:
        """Process a book return.

        Steps:
            - Finds the borrow record.
            - Marks it as returned.
            - Updates the book quantity (+1).
            - Decreases the reader's active borrow count.
            - Calculates late days and fine (if overdue).

        Args:
            record_id: UUID of the borrow record.

        Returns:
            The updated ``BorrowRecord``.

        Raises:
            KeyError: If the borrow record is not found.
            ValueError: If the record is already marked as returned.
        """
        record = self._find_record(record_id)

        if record.status == "returned":
            raise ValueError(f"Record '{record_id}' is already returned.")

        now = datetime.now()
        record.mark_returned(now)

        # Update book
        book = self._book_mgr.find_book(record.book_id)
        book.update_quantity(1)

        # Update reader
        reader = self._reader_mgr.find_reader(record.reader_id)
        reader.decrease_borrow_count()

        # Late penalty
        record.calculate_late_days(now)
        record.calculate_fine(self.fine_per_day)

        return record

    # ------------------------------------------------------------------
    # Query helpers
    # ------------------------------------------------------------------

    def _find_record(self, record_id: str) -> BorrowRecord:
        """Linear-search helper for borrow records.

        Raises:
            KeyError: If not found.
        """
        for rec in self.borrow_records:
            if rec.record_id == record_id:
                return rec
        raise KeyError(f"BorrowRecord '{record_id}' not found.")

    def count_active_borrows(self) -> int:
        """Number of records currently with status 'borrowing'."""
        return sum(1 for r in self.borrow_records if r.status == "borrowing")

    def get_overdue_records(self) -> list[BorrowRecord]:
        """Return all currently overdue (not yet returned) records."""
        now = datetime.now()
        return [r for r in self.borrow_records if r.is_overdue(now)]