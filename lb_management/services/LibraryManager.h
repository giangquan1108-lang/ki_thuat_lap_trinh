#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include "../../dsa/data_structure/LinkedList.h"
#include "../../dsa/data_structure/HashMap.h"
#include "../models/Book.h"
#include "../models/Reader.h"
#include "../models/BorrowRecord.h"

class LibraryManager {
private:
    // Kho chứa toàn bộ object dạng con trỏ (LinkedList giữ quyền sở hữu)
    LinkedList<Book*> books;
    LinkedList<Reader*> readers;
    LinkedList<BorrowRecord*> borrow_records;

    // Bảng băm index theo ID, tìm O(1), ko sở hữu con trỏ (chỉ tham chiếu)
    HashMap<std::string, Book*> book_index;
    HashMap<std::string, Reader*> reader_index;

    // Tham số cấu hình
    int student_borrow_limit;
    int teacher_borrow_limit;
    double fine_per_day;

    // Thư mục chứa file dữ liệu
    std::string data_dir;

    // Bộ đếm tự tăng để sinh mã phiếu mượn không trùng
    int record_id_counter;

public:
    LibraryManager();

    // Dựng manager với thư mục data tùy chỉnh
    LibraryManager(const std::string& data_directory);
    ~LibraryManager();

    // ---------- Quản lý Sách ----------
    bool add_book(Book* book);
    bool remove_book(const std::string& book_id);
    bool update_book(const std::string& book_id, const std::string& title,
                     const std::string& author, const std::string& category, int quantity);
    Book* find_book(const std::string& book_id) const;
    void search_book_by_title(const std::string& title) const;
    void display_all_books() const;
    void show_top_books(int count = 5) const;
    size_t get_book_count() const;

    // ---------- Quản lý Bạn đọc ----------
    bool add_reader(Reader* reader);
    bool remove_reader(const std::string& reader_id);
    Reader* find_reader(const std::string& reader_id) const;
    bool block_reader(const std::string& reader_id);
    bool unblock_reader(const std::string& reader_id);
    void display_all_readers() const;
    size_t get_reader_count() const;

    // ---------- Nghiệp vụ Mượn / Trả ----------
    bool borrow_book(const std::string& reader_id, const std::string& book_id,
                     const std::string& borrow_date, const std::string& due_date);
    bool return_book(const std::string& record_id, const std::string& return_date);
    BorrowRecord* find_record(const std::string& record_id) const;
    void display_all_records() const;
    void show_overdue_books(const std::string& current_date) const;
    size_t get_record_count() const;

    // ---------- Đọc / Ghi file TXT ----------
    void load_data();
    void save_data() const;

    // ---------- Thống kê & Báo cáo ----------
    void generate_report() const;
    int get_borrowing_count() const;
};

#endif