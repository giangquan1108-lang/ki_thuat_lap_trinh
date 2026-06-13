#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include "../../dsa/data_structure/LinkedList.h"
#include "../../dsa/data_structure/HashMap.h"
#include "../models/Book.h"
#include "../models/Reader.h"
#include "../models/BorrowRecord.h"

class LibraryManager {
private:
    // Kho dữ liệu lưu trữ đối tượng thực tế dưới dạng con trỏ để quản lý bộ nhớ an toàn
    LinkedList<Book*> books;
    LinkedList<Reader*> readers;
    LinkedList<BorrowRecord*> borrow_records;

    // Hash index giúp tìm kiếm dữ liệu siêu nhanh với độ phức tạp O(1)
    HashMap<std::string, Book*> book_index;
    HashMap<std::string, Reader*> reader_index;

    // Các tham số cấu hình hệ thống
    int student_borrow_limit;
    int teacher_borrow_limit;
    double fine_per_day;

    // Path data directory
    std::string data_dir;

public:
    LibraryManager();

    // Constructor with custom data directory
    LibraryManager(const std::string& data_directory);
    ~LibraryManager();

    // Biên mục Sách
    bool add_book(Book* book);
    bool remove_book(const std::string& book_id);
    bool update_book(const std::string& book_id, const std::string& title,
                     const std::string& author, const std::string& category, int quantity);
    Book* find_book(const std::string& book_id) const;
    void search_book_by_title(const std::string& title) const;
    void display_all_books() const;
    void show_top_books(int count = 5) const;
    size_t get_book_count() const;

    // Quản lý Bạn đọc
    bool add_reader(Reader* reader);
    bool remove_reader(const std::string& reader_id);
    Reader* find_reader(const std::string& reader_id) const;
    bool block_reader(const std::string& reader_id);
    bool unblock_reader(const std::string& reader_id);
    void display_all_readers() const;
    size_t get_reader_count() const;

    // Nghiệp vụ Mượn / Trả
    bool borrow_book(const std::string& reader_id, const std::string& book_id,
                     const std::string& borrow_date, const std::string& due_date);
    bool return_book(const std::string& record_id, const std::string& return_date);
    BorrowRecord* find_record(const std::string& record_id) const;
    void display_all_records() const;
    void show_overdue_books(const std::string& current_date) const;
    size_t get_record_count() const;

    // Đọc ghi File TXT
    void load_data();
    void save_data() const;

    // Báo cáo & Thống kê
    void generate_report() const;
    int get_borrowing_count() const;
};

#endif