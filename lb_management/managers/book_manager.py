"""
BookManager – Manages the book catalogue.

Provides CRUD operations, hash-index lookups, and title search.
"""

from __future__ import annotations

from typing import Any

from ..models.book import Book


class BookManager:
    """Manages the collection of ``Book`` objects with O(1) ID lookups.

    Responsibilities:
        - Add / remove / update books.
        - Find books by ID (hash index).
        - Search books by title (case-insensitive partial match).
    """

    def __init__(self) -> None:
        self.books: list[Book] = []
        self.book_index: dict[str, Book] = {}

    # ------------------------------------------------------------------
    # Index helpers
    # ------------------------------------------------------------------

    def _rebuild_index(self) -> None:
        """Rebuild the book hash index from the current list."""
        self.book_index.clear()
        for book in self.books:
            self.book_index[book.id] = book

    # ------------------------------------------------------------------
    # CRUD
    # ------------------------------------------------------------------

    def add_book(self, book: Book) -> None:
        """Register a new book.

        Args:
            book: The ``Book`` instance to add.

        Raises:
            ValueError: If a book with the same ID already exists.
        """
        if book.id in self.book_index:
            raise ValueError(f"Book '{book.id}' already exists.")
        self.books.append(book)
        self.book_index[book.id] = book

    def remove_book(self, book_id: str) -> None:
        """Remove a book from the catalogue.

        Args:
            book_id: ID of the book to remove.

        Raises:
            KeyError: If the book is not found.
        """
        book = self.find_book(book_id)
        self.books.remove(book)
        del self.book_index[book_id]

    def update_book(self, book_id: str, **kwargs: Any) -> None:
        """Update arbitrary attributes of an existing book.

        Args:
            book_id: ID of the book to update.
            **kwargs: Attribute name/value pairs to set.

        Raises:
            KeyError: If the book is not found.
        """
        book = self.find_book(book_id)
        for attr, value in kwargs.items():
            if hasattr(book, attr):
                setattr(book, attr, value)

    # ------------------------------------------------------------------
    # Query
    # ------------------------------------------------------------------

    def find_book(self, book_id: str) -> Book:
        """Look up a book by ID (O(1) via hash index).

        Args:
            book_id: Unique book identifier.

        Returns:
            The matching ``Book`` instance.

        Raises:
            KeyError: If no book with *book_id* exists.
        """
        if book_id not in self.book_index:
            raise KeyError(f"Book '{book_id}' not found.")
        return self.book_index[book_id]

    def search_book_by_title(self, title: str) -> list[Book]:
        """Case-insensitive partial title search.

        Args:
            title: Substring to match against book titles.

        Returns:
            List of matching ``Book`` instances (may be empty).
        """
        lower = title.lower()
        return [b for b in self.books if lower in b.title.lower()]

    # ------------------------------------------------------------------
    # Reporting helpers
    # ------------------------------------------------------------------

    def count_available(self) -> int:
        """Count books that are currently available."""
        return sum(1 for b in self.books if b.is_available())

    def get_top_books(self, limit: int = 10) -> list[Book]:
        """Return books sorted by borrow count (descending).

        Args:
            limit: Maximum number of books to return.

        Returns:
            Top *limit* most-borrowed books.
        """
        sorted_books = sorted(
            self.books, key=lambda b: b.borrow_count, reverse=True
        )
        return sorted_books[:limit]