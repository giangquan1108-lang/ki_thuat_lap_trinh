"""
BorrowRecord domain model.

Represents a single borrowing transaction / loan record.
"""

from __future__ import annotations

from dataclasses import dataclass, field
from datetime import datetime, timezone
from typing import Any, Optional


@dataclass
class BorrowRecord:
    """A record of one book being borrowed by one reader.

    Attributes:
        record_id: Unique identifier for this loan record.
        book_id: ID of the book that was borrowed.
        reader_id: ID of the reader who borrowed the book.
        borrow_date: Date/time when the book was checked out.
        due_date: Date/time when the book must be returned.
        return_date: Date/time when the book was actually returned
            (``None`` while still borrowed).
        status: Current status: ``'borrowing'`` or ``'returned'``.
        fine: Monetary penalty in VND for late return.
        late_days: Number of days the return is overdue.
    """

    record_id: str
    book_id: str
    reader_id: str
    borrow_date: datetime
    due_date: datetime
    return_date: Optional[datetime] = None
    status: str = "borrowing"
    fine: float = 0.0
    late_days: int = 0

    # ------------------------------------------------------------------
    # Display
    # ------------------------------------------------------------------

    def display_info(self) -> str:
        """Return a human-readable summary of the loan record.

        Returns:
            Formatted string with key fields.
        """
        return (
            f"BorrowRecord[ID={self.record_id}, "
            f"Book={self.book_id}, Reader={self.reader_id}, "
            f"Borrowed={self.borrow_date.isoformat()}, "
            f"Due={self.due_date.isoformat()}, "
            f"Returned={self.return_date.isoformat() if self.return_date else 'N/A'}, "
            f"Status='{self.status}', "
            f"Fine={self.fine}, LateDays={self.late_days}]"
        )

    # ------------------------------------------------------------------
    # Business logic
    # ------------------------------------------------------------------

    def mark_returned(self, return_date: datetime) -> None:
        """Record the return of the book.

        Args:
            return_date: The date/time the book was returned.
        """
        self.return_date = return_date
        self.status = "returned"

    def calculate_late_days(self, current_date: Optional[datetime] = None) -> int:
        """Compute how many days the book is overdue.

        Args:
            current_date: Reference date (defaults to ``datetime.now()``).

        Returns:
            Number of whole days past the due date, or ``0`` if not overdue.
        """
        ref = current_date if current_date is not None else datetime.now()
        if ref <= self.due_date:
            self.late_days = 0
            return 0
        delta = ref - self.due_date
        self.late_days = delta.days
        return self.late_days

    def calculate_fine(self, fine_per_day: float) -> float:
        """Calculate the late-return penalty.

        Args:
            fine_per_day: Penalty rate in VND per late day.

        Returns:
            Total fine amount (``late_days * fine_per_day``).
        """
        self.fine = self.late_days * fine_per_day
        return self.fine

    def is_overdue(self, current_date: Optional[datetime] = None) -> bool:
        """Check whether this loan is currently overdue.

        Args:
            current_date: Reference date (defaults to ``datetime.now()``).

        Returns:
            ``True`` if the current date exceeds the due date
            **and** the book has not been returned yet.
        """
        if self.status == "returned":
            return False
        ref = current_date if current_date is not None else datetime.now()
        return ref > self.due_date

    # ------------------------------------------------------------------
    # Serialization
    # ------------------------------------------------------------------

    def to_dict(self) -> dict[str, Any]:
        """Serialize the record to a dictionary suitable for JSON.

        All datetime values are converted to ISO-8601 strings.

        Returns:
            Dictionary representation of the borrow record.
        """
        return {
            "record_id": self.record_id,
            "book_id": self.book_id,
            "reader_id": self.reader_id,
            "borrow_date": self.borrow_date.isoformat(),
            "due_date": self.due_date.isoformat(),
            "return_date": self.return_date.isoformat()
            if self.return_date
            else None,
            "status": self.status,
            "fine": self.fine,
            "late_days": self.late_days,
        }

    @classmethod
    def from_dict(cls, data: dict[str, Any]) -> BorrowRecord:
        """Construct a ``BorrowRecord`` instance from a dictionary.

        ISO-8601 strings are parsed back into ``datetime`` objects.

        Args:
            data: Dictionary obtained from a previous ``to_dict()`` call
                  or from a JSON file.

        Returns:
            A fully-populated ``BorrowRecord`` instance.
        """
        return cls(
            record_id=data["record_id"],
            book_id=data["book_id"],
            reader_id=data["reader_id"],
            borrow_date=datetime.fromisoformat(data["borrow_date"]),
            due_date=datetime.fromisoformat(data["due_date"]),
            return_date=datetime.fromisoformat(data["return_date"])
            if data.get("return_date")
            else None,
            status=data.get("status", "borrowing"),
            fine=data.get("fine", 0.0),
            late_days=data.get("late_days", 0),
        )