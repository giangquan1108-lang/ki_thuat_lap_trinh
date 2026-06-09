"""
LibraryManager – Facade that wires together all sub-managers.

Provides a unified API for the library system, delegating work to
``BookManager``, ``ReaderManager``, and ``BorrowManager``, and
handling JSON persistence.
"""

from __future__ import annotations

import json
import os
from typing import Any

from ..models.book import Book
from ..models.reader import Reader
from ..models.borrow_record import BorrowRecord
from .book_manager import BookManager
from .reader_manager import ReaderManager
from .borrow_manager import BorrowManager


class LibraryManager:
    """Central facade for the library management system.

    Composes three specialised managers:
        - ``BookManager`` – book catalogue CRUD & search.
        - ``ReaderManager`` – patron CRUD & lookups.
        - ``BorrowManager`` – borrow/return workflow & fine calculation.

    Also owns JSON persistence via ``load_data()`` / ``save_data()``.
    """

    def __init__(
        self,
        data_dir: str = "data",
        borrow_limit: int = 5,
        fine_per_day: float = 5000.0,
    ) -> None:
        """Initialise the facade and its child managers.

        Args:
            data_dir: Path to the directory containing ``books.json``,
                ``readers.json``, and ``borrowRecords.json``.
            borrow_limit: Maximum concurrent borrows per reader.
            fine_per_day: Late-return penalty rate (VND/day).
        """
        self.data_dir = data_dir
        self.borrow_limit = borrow_limit
        self.fine_per_day = fine_per_day

        # Child managers
        self._book_mgr = BookManager()
        self._reader_mgr = ReaderManager()
        self._borrow_mgr = BorrowManager(
            book_manager=self._book_mgr,
            reader_manager=self._reader_mgr,
            borrow_limit=borrow_limit,
            fine_per_day=fine_per_day,
        )

    # ------------------------------------------------------------------
    # Convenience aliases for external access
    # ------------------------------------------------------------------
    @property
    def books(self) -> list[Book]:
        """Current list of all books."""
        return self._book_mgr.books

    @property
    def readers(self) -> list[Reader]:
        """Current list of all readers."""
        return self._reader_mgr.readers

    @property
    def borrow_records(self) -> list[BorrowRecord]:
        """Current list of all borrow records."""
        return self._borrow_mgr.borrow_records

    @property
    def book_index(self) -> dict[str, Book]:
        """Hash index for O(1) book lookups."""
        return self._book_mgr.book_index

    @property
    def reader_index(self) -> dict[str, Reader]:
        """Hash index for O(1) reader lookups."""
        return self._reader_mgr.reader_index

    # ==================================================================
    # Persistence
    # ==================================================================

    def _file_path(self, filename: str) -> str:
        return os.path.join(self.data_dir, filename)

    def save_data(self) -> None:
        """Persist all in-memory collections to JSON files."""
        os.makedirs(self.data_dir, exist_ok=True)

        with open(self._file_path("books.json"), "w", encoding="utf-8") as f:
            json.dump([b.to_dict() for b in self.books], f, ensure_ascii=False, indent=2)

        with open(self._file_path("readers.json"), "w", encoding="utf-8") as f:
            json.dump([r.to_dict() for r in self.readers], f, ensure_ascii=False, indent=2)

        with open(self._file_path("borrowRecords.json"), "w", encoding="utf-8") as f:
            json.dump(
                [rec.to_dict() for rec in self.borrow_records],
                f,
                ensure_ascii=False,
                indent=2,
            )

    def load_data(self) -> None:
        """Load collections from JSON files and rebuild indexes."""
        # Books
        path = self._file_path("books.json")
        if os.path.exists(path):
            with open(path, "r", encoding="utf-8") as f:
                raw = json.load(f)
            self._book_mgr.books = [Book.from_dict(d) for d in raw]
            self._book_mgr._rebuild_index()

        # Readers
        path = self._file_path("readers.json")
        if os.path.exists(path):
            with open(path, "r", encoding="utf-8") as f:
                raw = json.load(f)
            self._reader_mgr.readers = [Reader.from_dict(d) for d in raw]
            self._reader_mgr._rebuild_index()

        # Borrow records
        path = self._file_path("borrowRecords.json")
        if os.path.exists(path):
            with open(path, "r", encoding="utf-8") as f:
                raw = json.load(f)
            self._borrow_mgr.borrow_records = [
                BorrowRecord.from_dict(d) for d in raw
            ]

    # ==================================================================
    # Book delegation
    # ==================================================================

    def add_book(self, book: Book) -> None:
        self._book_mgr.add_book(book)

    def remove_book(self, book_id: str) -> None:
        self._book_mgr.remove_book(book_id)

    def update_book(self, book_id: str, **kwargs: Any) -> None:
        self._book_mgr.update_book(book_id, **kwargs)

    def find_book(self, book_id: str) -> Book:
        return self._book_mgr.find_book(book_id)

    def search_book_by_title(self, title: str) -> list[Book]:
        return self._book_mgr.search_book_by_title(title)

    # ==================================================================
    # Reader delegation
    # ==================================================================

    def add_reader(self, reader: Reader) -> None:
        self._reader_mgr.add_reader(reader)

    def remove_reader(self, reader_id: str) -> None:
        self._reader_mgr.remove_reader(reader_id)

    def find_reader(self, reader_id: str) -> Reader:
        return self._reader_mgr.find_reader(reader_id)

    # ==================================================================
    # Borrow delegation
    # ==================================================================

    def borrow_book(self, reader_id: str, book_id: str) -> BorrowRecord:
        return self._borrow_mgr.borrow_book(reader_id, book_id)

    def return_book(self, record_id: str) -> BorrowRecord:
        return self._borrow_mgr.return_book(record_id)

    # ==================================================================
    # Reporting
    # ==================================================================

    def generate_report(self) -> dict[str, Any]:
        """Produce a summary snapshot of the library."""
        return {
            "total_books": len(self.books),
            "total_readers": len(self.readers),
            "total_records": len(self.borrow_records),
            "books_available": self._book_mgr.count_available(),
            "books_borrowed": self._borrow_mgr.count_active_borrows(),
        }

    def show_overdue_books(self) -> list[BorrowRecord]:
        """List all currently overdue borrow records."""
        return self._borrow_mgr.get_overdue_records()

    def show_top_books(self, limit: int = 10) -> list[Book]:
        """Return the books with the highest total borrow counts."""
        return self._book_mgr.get_top_books(limit)