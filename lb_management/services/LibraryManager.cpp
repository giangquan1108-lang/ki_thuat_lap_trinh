#include "LibraryManager.h"
#include "../../dsa/algorithms/sorting.h"
#include "../../dsa/algorithms/searching.h"
#include <fstream>
#include <sstream>
#include <iostream>

// ==========================================
// CÁC HÀM TIỆN ÍCH PHỤ TRỢ (HELPER FUNCTIONS)
// ==========================================

// Hàm tự chế để tách chuỗi bằng ký tự '|' và nạp vào LinkedList
void split_string(const std::string& str, char delimiter, LinkedList<std::string>& tokens) {
    std::string token = "";
    for (char c : str) {
        if (c == delimiter) {
            tokens.insertAtTail(token);
            token = "";
        } else {
            token += c;
        }
    }
    tokens.insertAtTail(token); // Nạp phần tử cuối cùng
}

// Kiểm tra năm nhuận
bool is_leap_year(int y) {
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

// Lấy số ngày trong tháng
int get_days_in_month(int m, int y) {
    if (m == 2) return is_leap_year(y) ? 29 : 28;
    if (m == 4 || m == 6 || m == 9 || m == 11) return 30;
    return 31;
}

// Chuyển đổi ngày định dạng DD/MM/YYYY sang số ngày tuyệt đối tính từ năm 1
int date_to_absolute_days(const std::string& date_str) {
    if (date_str.length() < 10) return 0;
    int d = std::stoi(date_str.substr(0, 2));
    int m = std::stoi(date_str.substr(3, 2));
    int y = std::stoi(date_str.substr(6, 4));

    int total_days = d;
    for (int i = 1; i < y; ++i) {
        total_days += is_leap_year(i) ? 366 : 365;
    }
    for (int i = 1; i < m; ++i) {
        total_days += get_days_in_month(i, y);
    }
    return total_days;
}

// Tính khoảng cách giữa hai ngày (ngày 2 - ngày 1)
int calculate_date_difference(const std::string& date1, const std::string& date2) {
    return date_to_absolute_days(date2) - date_to_absolute_days(date1);
}

// ==========================================
// TRIỂN KHAI CÁC PHƯƠNG THỨC LỚP LIBRARYMANAGER
// ==========================================

LibraryManager::LibraryManager() {
    student_borrow_limit = 3;
    teacher_borrow_limit = 5;
    fine_per_day = 5000.0; // Phạt 5,000 VND một ngày trễ hạn
    data_dir = "../data/";
}

LibraryManager::LibraryManager(const std::string& data_directory) {
    student_borrow_limit = 3;
    teacher_borrow_limit = 5;
    fine_per_day = 5000.0;
    data_dir = data_directory;
}

// Hàm hủy giải phóng toàn bộ vùng nhớ Heap đã cấp phát động cho các đối tượng
LibraryManager::~LibraryManager() {
    // Giải phóng Sách
    Node<Book*>* curr_book = books.getHead();
    while (curr_book != nullptr) {
        delete curr_book->data;
        curr_book = curr_book->next;
    }

    // Giải phóng Bạn đọc
    Node<Reader*>* curr_reader = readers.getHead();
    while (curr_reader != nullptr) {
        delete curr_reader->data;
        curr_reader = curr_reader->next;
    }

    // Giải phóng Phiếu mượn
    Node<BorrowRecord*>* curr_record = borrow_records.getHead();
    while (curr_record != nullptr) {
        delete curr_record->data;
        curr_record = curr_record->next;
    }
}

// ==========================================
// BIÊN MỤC SÁCH
// ==========================================

bool LibraryManager::add_book(Book* book) {
    if (book_index.get(book->id) != nullptr) {
        return false; // Mã sách đã tồn tại
    }
    books.insertAtTail(book);
    book_index.put(book->id, book);
    return true;
}

bool LibraryManager::remove_book(const std::string& book_id) {
    Book* b = book_index.get(book_id);
    if (b == nullptr) return false;

    book_index.remove(book_id);
    books.remove(b);
    delete b;
    return true;
}

bool LibraryManager::update_book(const std::string& book_id, const std::string& title,
                                  const std::string& author, const std::string& category, int quantity) {
    Book* b = book_index.get(book_id);
    if (b == nullptr) return false;

    if (!title.empty()) b->title = title;
    if (!author.empty()) b->author = author;
    if (!category.empty()) b->category = category;
    if (quantity >= 0) b->quantity = quantity;
    return true;
}

Book* LibraryManager::find_book(const std::string& book_id) const {
    return book_index.get(book_id);
}

void LibraryManager::search_book_by_title(const std::string& title) const {
    size_t sz = books.getSize();
    if (sz == 0) {
        std::cout << "\n(Khong co sach nao trong he thong.)\n";
        return;
    }

    // Chuyển LinkedList sang mảng tạm để dùng thuật toán tìm kiếm từ dsa/algorithms/searching.h
    Book** arr = new Book*[sz];
    Node<Book*>* curr = books.getHead();
    size_t idx = 0;
    while (curr != nullptr) {
        arr[idx++] = curr->data;
        curr = curr->next;
    }

    // Tạo Book tạm chỉ chứa title để làm target cho linearSearch với custom comparator
    Book targetBook("", title, "", "", 0);
    Book* target = &targetBook;

    // Custom match function: kiểm tra title của sách có chứa từ khóa tìm kiếm không
    auto matchTitle = [](Book* a, Book* b) -> bool {
        return a->title.find(b->title) != std::string::npos;
    };

    // Gọi linearSearch từ searching.h để tìm bản ghi đầu tiên khớp
    int firstMatch = linearSearch(arr, sz, target, matchTitle);

    std::cout << "\n--- KET QUA TIM KIEM SACH ---\n";
    if (firstMatch == -1) {
        std::cout << "Khong tim thay sach nao co ten chua tu khoa: " << title << "\n";
    } else {
        // Duyệt qua toàn bộ mảng để in tất cả kết quả khớp
        for (size_t i = 0; i < sz; ++i) {
            if (matchTitle(arr[i], target)) {
                arr[i]->display_info();
            }
        }
    }

    delete[] arr;
}

void LibraryManager::display_all_books() const {
    Node<Book*>* curr = books.getHead();
    if (curr == nullptr) {
        std::cout << "\n(Khong co sach nao trong he thong.)\n";
        return;
    }
    std::cout << "\n--- DANH SACH TAT CA SACH (" << books.getSize() << " cuon) ---\n";
    int i = 1;
    while (curr != nullptr) {
        std::cout << "  " << i++ << ". ";
        curr->data->display_info();
        curr = curr->next;
    }
}

void LibraryManager::show_top_books(int count) const {
    size_t sz = books.getSize();
    if (sz == 0) {
        std::cout << "\nKho thu vien hien tai chua co sach!\n";
        return;
    }

    Book** arr = new Book*[sz];
    Node<Book*>* curr = books.getHead();
    size_t idx = 0;
    while (curr != nullptr) {
        arr[idx++] = curr->data;
        curr = curr->next;
    }

    // Sắp xếp giảm dần theo borrow_count bằng thuật toán Quick Sort từ dsa/algorithms/sorting.h
    // Comparator: a đứng trước b khi a->borrow_count > b->borrow_count (giảm dần)
    auto cmpDesc = [](Book* a, Book* b) -> bool {
        return a->borrow_count > b->borrow_count;
    };
    quickSort(arr, sz, cmpDesc);

    std::cout << "\n--- TOP " << count << " SACH DUOC MUON NHIEU NHAT ---\n";
    size_t top_limit = (sz < (size_t)count) ? sz : (size_t)count;
    for (size_t i = 0; i < top_limit; ++i) {
        std::cout << "  " << (i + 1) << ". [" << arr[i]->id << "] " << arr[i]->title
                  << " | Luot muon: " << arr[i]->borrow_count << "\n";
    }

    delete[] arr;
}

size_t LibraryManager::get_book_count() const {
    return books.getSize();
}

// ==========================================
// QUẢN LÝ BẠN ĐỌC
// ==========================================

bool LibraryManager::add_reader(Reader* reader) {
    if (reader_index.get(reader->student_id) != nullptr) {
        return false; // Mã bạn đọc đã tồn tại
    }
    readers.insertAtTail(reader);
    reader_index.put(reader->student_id, reader);
    return true;
}

bool LibraryManager::remove_reader(const std::string& reader_id) {
    Reader* r = reader_index.get(reader_id);
    if (r == nullptr) return false;

    reader_index.remove(reader_id);
    readers.remove(r);
    delete r;
    return true;
}

Reader* LibraryManager::find_reader(const std::string& reader_id) const {
    return reader_index.get(reader_id);
}

bool LibraryManager::block_reader(const std::string& reader_id) {
    Reader* r = reader_index.get(reader_id);
    if (r == nullptr) return false;
    if (r->is_blocked) return false; // Already blocked
    r->block_reader();
    return true;
}

bool LibraryManager::unblock_reader(const std::string& reader_id) {
    Reader* r = reader_index.get(reader_id);
    if (r == nullptr) return false;
    if (!r->is_blocked) return false; // Already unblocked
    r->unblock_reader();
    return true;
}

void LibraryManager::display_all_readers() const {
    Node<Reader*>* curr = readers.getHead();
    if (curr == nullptr) {
        std::cout << "\n(Khong co ban doc nao trong he thong.)\n";
        return;
    }
    std::cout << "\n--- DANH SACH TAT CA BAN DOC (" << readers.getSize() << " nguoi) ---\n";
    int i = 1;
    while (curr != nullptr) {
        std::cout << "  " << i++ << ". ";
        curr->data->display_info();
        curr = curr->next;
    }
}

size_t LibraryManager::get_reader_count() const {
    return readers.getSize();
}

// ==========================================
// NGHIỆP VỤ MƯỢN / TRẢ
// ==========================================

bool LibraryManager::borrow_book(const std::string& reader_id, const std::string& book_id,
                                 const std::string& borrow_date, const std::string& due_date) {
    Reader* r = find_reader(reader_id);
    Book* b = find_book(book_id);

    if (r == nullptr) {
        std::cout << "Loi: Khong tim thay thong tin ban doc!\n";
        return false;
    }
    if (b == nullptr) {
        std::cout << "Loi: Khong tim thay sach!\n";
        return false;
    }

    if (r->is_blocked) {
        std::cout << "Loi: Tai khoan ban doc dang bi KHOA, khong the muon sach!\n";
        return false;
    }

    if (!b->is_available()) {
        std::cout << "Loi: Sach nay da het trong kho!\n";
        return false;
    }

    // Kiểm tra trễ hạn nợ chưa trả
    Node<BorrowRecord*>* curr = borrow_records.getHead();
    while (curr != nullptr) {
        BorrowRecord* rec = curr->data;
        if (rec->reader_id == reader_id && rec->status != "returned") {
            if (calculate_date_difference(rec->due_date, borrow_date) > 0) {
                std::cout << "Loi: Ban doc dang no sach qua han chua tra (Phieu muon: "
                          << rec->record_id << ")! Khong the muon tiep.\n";
                return false;
            }
        }
        curr = curr->next;
    }

    // Kiểm tra hạn mức mượn
    int limit = (r->reader_type == "teacher") ? teacher_borrow_limit : student_borrow_limit;
    if (!r->can_borrow(limit)) {
        std::cout << "Loi: Ban doc da dat den han muc muon toi da cua minh (" << limit << " cuon)!\n";
        return false;
    }

    // Tạo phiếu mượn mới
    std::string new_record_id = "REC" + std::to_string(borrow_records.getSize() + 1001);
    BorrowRecord* new_record = new BorrowRecord(new_record_id, book_id, reader_id, borrow_date, due_date);

    borrow_records.insertAtTail(new_record);

    b->update_quantity(-1);
    b->increase_borrow_count();
    r->increase_borrow_count();

    std::cout << "Muon sach thanh cong! Ma phieu muon la: " << new_record_id << "\n";
    std::cout << "Han tra: " << due_date << "\n";
    return true;
}

bool LibraryManager::return_book(const std::string& record_id, const std::string& return_date) {
    Node<BorrowRecord*>* curr = borrow_records.getHead();
    BorrowRecord* target_record = nullptr;

    while (curr != nullptr) {
        if (curr->data->record_id == record_id) {
            target_record = curr->data;
            break;
        }
        curr = curr->next;
    }

    if (target_record == nullptr) {
        std::cout << "Loi: Khong tim thay ma phieu muon nay!\n";
        return false;
    }

    if (target_record->status == "returned") {
        std::cout << "Loi: Phieu muon nay da duoc hoan thanh tra truoc day!\n";
        return false;
    }

    target_record->mark_returned(return_date);

    int diff = calculate_date_difference(target_record->due_date, return_date);
    if (diff > 0) {
        target_record->late_days = diff;
        target_record->calculate_fine(fine_per_day);
        target_record->status = "overdue";
        std::cout << "Canh bao: Sach tra tre han " << diff << " ngay! So tien phat la: "
                  << (long long)target_record->fine << " VND.\n";
    } else {
        std::cout << "Tra sach dung han. Cam on ban doc!\n";
    }

    Book* b = find_book(target_record->book_id);
    if (b != nullptr) b->update_quantity(1);

    Reader* r = find_reader(target_record->reader_id);
    if (r != nullptr) r->decrease_borrow_count();

    return true;
}

BorrowRecord* LibraryManager::find_record(const std::string& record_id) const {
    Node<BorrowRecord*>* curr = borrow_records.getHead();
    while (curr != nullptr) {
        if (curr->data->record_id == record_id) {
            return curr->data;
        }
        curr = curr->next;
    }
    return nullptr;
}

void LibraryManager::display_all_records() const {
    Node<BorrowRecord*>* curr = borrow_records.getHead();
    if (curr == nullptr) {
        std::cout << "\n(Khong co phieu muon nao trong he thong.)\n";
        return;
    }
    std::cout << "\n--- DANH SACH TAT CA PHIEU MUON (" << borrow_records.getSize() << " phieu) ---\n";
    int i = 1;
    while (curr != nullptr) {
        std::cout << "  " << i++ << ". ";
        curr->data->display_info();
        curr = curr->next;
    }
}

void LibraryManager::show_overdue_books(const std::string& current_date) const {
    std::cout << "\n--- DANH SACH PHIEU MUON QUA HAN CHUA TRA ---\n";
    Node<BorrowRecord*>* curr = borrow_records.getHead();
    bool found = false;

    while (curr != nullptr) {
        BorrowRecord* rec = curr->data;
        if (rec->status != "returned") {
            int diff = calculate_date_difference(rec->due_date, current_date);
            if (diff > 0) {
                rec->display_info();
                std::cout << "  -> Tre han: " << diff << " ngay | Tien phat uoc tinh: "
                          << (long long)(diff * fine_per_day) << " VND\n";
                found = true;
            }
        }
        curr = curr->next;
    }
    if (!found) {
        std::cout << "Khong co phieu muon nao qua han tinh den ngay " << current_date << ".\n";
    }
}

size_t LibraryManager::get_record_count() const {
    return borrow_records.getSize();
}

int LibraryManager::get_borrowing_count() const {
    int count = 0;
    Node<BorrowRecord*>* curr = borrow_records.getHead();
    while (curr != nullptr) {
        if (curr->data->status != "returned") {
            count++;
        }
        curr = curr->next;
    }
    return count;
}

// ==========================================================
// ĐỌC VÀ GHI FILE TEXT PHÂN TÁCH BẰNG DẤU ĐỨNG "|" (Pipe-Delimited)
// ==========================================================

void LibraryManager::save_data() const {
    // 1. Lưu danh sách Sách
    std::ofstream book_file(data_dir + "books.txt");
    if (book_file.is_open()) {
        Node<Book*>* curr = books.getHead();
        while (curr != nullptr) {
            Book* b = curr->data;
            book_file << b->id << "|" << b->title << "|" << b->author << "|"
                      << b->category << "|" << b->quantity << "|" << b->borrow_count << "\n";
            curr = curr->next;
        }
        book_file.close();
    }

    // 2. Lưu danh sách Bạn đọc
    std::ofstream reader_file(data_dir + "readers.txt");
    if (reader_file.is_open()) {
        Node<Reader*>* curr = readers.getHead();
        while (curr != nullptr) {
            Reader* r = curr->data;
            reader_file << r->student_id << "|" << r->full_name << "|" << r->reader_type << "|"
                        << r->borrowed_count << "|" << (r->is_blocked ? "1" : "0") << "|"
                        << r->phone << "|" << r->email << "\n";
            curr = curr->next;
        }
        reader_file.close();
    }

    // 3. Lưu danh sách Phiếu mượn
    std::ofstream record_file(data_dir + "records.txt");
    if (record_file.is_open()) {
        Node<BorrowRecord*>* curr = borrow_records.getHead();
        while (curr != nullptr) {
            BorrowRecord* rec = curr->data;
            record_file << rec->record_id << "|" << rec->book_id << "|" << rec->reader_id << "|"
                        << rec->borrow_date << "|" << rec->due_date << "|" << rec->return_date << "|"
                        << rec->status << "|" << rec->fine << "|" << rec->late_days << "\n";
            curr = curr->next;
        }
        record_file.close();
    }
}

void LibraryManager::load_data() {
    std::string line;

    // 1. Đọc danh sách Sách từ file "books.txt"
    std::ifstream book_file(data_dir + "books.txt");
    if (book_file.is_open()) {
        while (std::getline(book_file, line)) {
            if (line.empty()) continue;
            LinkedList<std::string> tokens;
            split_string(line, '|', tokens);

            Node<std::string>* curr_token = tokens.getHead();
            if (curr_token == nullptr) continue;

            Book* b = new Book();
            b->id = curr_token->data; curr_token = curr_token->next; if (!curr_token) { delete b; continue; }
            b->title = curr_token->data; curr_token = curr_token->next; if (!curr_token) { delete b; continue; }
            b->author = curr_token->data; curr_token = curr_token->next; if (!curr_token) { delete b; continue; }
            b->category = curr_token->data; curr_token = curr_token->next; if (!curr_token) { delete b; continue; }
            b->quantity = std::stoi(curr_token->data); curr_token = curr_token->next; if (!curr_token) { delete b; continue; }
            b->borrow_count = std::stoi(curr_token->data);

            books.insertAtTail(b);
            book_index.put(b->id, b);
        }
        book_file.close();
    }

    // 2. Đọc danh sách Bạn đọc từ file "readers.txt"
    std::ifstream reader_file(data_dir + "readers.txt");
    if (reader_file.is_open()) {
        while (std::getline(reader_file, line)) {
            if (line.empty()) continue;
            LinkedList<std::string> tokens;
            split_string(line, '|', tokens);

            Node<std::string>* curr_token = tokens.getHead();
            if (curr_token == nullptr) continue;

            Reader* r = new Reader();
            r->student_id = curr_token->data; curr_token = curr_token->next; if (!curr_token) { delete r; continue; }
            r->full_name = curr_token->data; curr_token = curr_token->next; if (!curr_token) { delete r; continue; }
            r->reader_type = curr_token->data; curr_token = curr_token->next; if (!curr_token) { delete r; continue; }
            r->borrowed_count = std::stoi(curr_token->data); curr_token = curr_token->next; if (!curr_token) { delete r; continue; }
            r->is_blocked = (curr_token->data == "1"); curr_token = curr_token->next; if (!curr_token) { delete r; continue; }
            r->phone = curr_token->data; curr_token = curr_token->next; if (!curr_token) { delete r; continue; }
            r->email = curr_token->data;

            readers.insertAtTail(r);
            reader_index.put(r->student_id, r);
        }
        reader_file.close();
    }

    // 3. Đọc danh sách Phiếu mượn từ file "records.txt"
    std::ifstream record_file(data_dir + "records.txt");
    if (record_file.is_open()) {
        while (std::getline(record_file, line)) {
            if (line.empty()) continue;
            LinkedList<std::string> tokens;
            split_string(line, '|', tokens);

            Node<std::string>* curr_token = tokens.getHead();
            if (curr_token == nullptr) continue;

            BorrowRecord* rec = new BorrowRecord();
            rec->record_id = curr_token->data; curr_token = curr_token->next; if (!curr_token) { delete rec; continue; }
            rec->book_id = curr_token->data; curr_token = curr_token->next; if (!curr_token) { delete rec; continue; }
            rec->reader_id = curr_token->data; curr_token = curr_token->next; if (!curr_token) { delete rec; continue; }
            rec->borrow_date = curr_token->data; curr_token = curr_token->next; if (!curr_token) { delete rec; continue; }
            rec->due_date = curr_token->data; curr_token = curr_token->next; if (!curr_token) { delete rec; continue; }
            rec->return_date = curr_token->data; curr_token = curr_token->next; if (!curr_token) { delete rec; continue; }
            rec->status = curr_token->data; curr_token = curr_token->next; if (!curr_token) { delete rec; continue; }
            rec->fine = std::stod(curr_token->data); curr_token = curr_token->next; if (!curr_token) { delete rec; continue; }
            rec->late_days = std::stoi(curr_token->data);

            borrow_records.insertAtTail(rec);
        }
        record_file.close();
    }
}

// ==========================================================
// CÁC HÀM THỐNG KÊ BÁO CÁO
// ==========================================================

void LibraryManager::generate_report() const {
    std::cout << "\n============================================\n";
    std::cout << "           BAO CAO THONG KE HE THONG        \n";
    std::cout << "============================================\n";
    std::cout << "Tong so sach dang quan ly: " << books.getSize() << "\n";
    std::cout << "Tong so ban doc dang ky:   " << readers.getSize() << "\n";
    std::cout << "Tong so giao dich muon tra: " << borrow_records.getSize() << "\n";

    int borrowing_count = 0;
    Node<BorrowRecord*>* curr = borrow_records.getHead();
    while (curr != nullptr) {
        if (curr->data->status != "returned") {
            borrowing_count++;
        }
        curr = curr->next;
    }
    std::cout << "So sach dang duoc muon thuc te: " << borrowing_count << "\n";

    // Count available books
    int available = 0;
    Node<Book*>* curr_book = books.getHead();
    while (curr_book != nullptr) {
        if (curr_book->data->is_available()) {
            available++;
        }
        curr_book = curr_book->next;
    }
    std::cout << "So sach con trong kho: " << available << "\n";
    std::cout << "============================================\n";
}