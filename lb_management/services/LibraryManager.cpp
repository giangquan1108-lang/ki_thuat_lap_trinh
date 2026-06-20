#include "LibraryManager.h"
#include "../../dsa/algorithms/sorting.h"
#include "../../dsa/algorithms/searching.h"
#include <fstream>
#include <sstream>
#include <iostream>

// ==========================================
// MẤY HÀM TIỆN ÍCH LẶT VẶT
// ==========================================

// Cắt chuỗi theo ký tự delimiter, tống vô LinkedList
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
    tokens.insertAtTail(token); // Nhét nốt khúc cuối
}

// Check năm nhuận (tháng 2 có 29 ngày ko)
bool is_leap_year(int y) {
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

// Đếm số ngày trong 1 tháng
int get_days_in_month(int m, int y) {
    if (m == 2) return is_leap_year(y) ? 29 : 28;
    if (m == 4 || m == 6 || m == 9 || m == 11) return 30;
    return 31;
}

// Đổi ngày DD/MM/YYYY ra số ngày tuyệt đối (tính từ năm số 1)
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

// Tính khoảng cách ngày (date2 - date1)
int calculate_date_difference(const std::string& date1, const std::string& date2) {
    return date_to_absolute_days(date2) - date_to_absolute_days(date1);
}

// ==========================================
// CÀI ĐẶT CÁC PHƯƠNG THỨC LIBRARYMANAGER
// ==========================================

// Dựng manager với tham số mặc định
LibraryManager::LibraryManager() {
    student_borrow_limit = 3;
    teacher_borrow_limit = 5;
    fine_per_day = 5000.0; // 5k VND / ngày trễ
    data_dir = "../data/";
    record_id_counter = 1001;
}

// Dựng manager với đường dẫn data tự chọn
LibraryManager::LibraryManager(const std::string& data_directory) {
    student_borrow_limit = 3;
    teacher_borrow_limit = 5;
    fine_per_day = 5000.0;
    data_dir = data_directory;
    record_id_counter = 1001;
}

// Hủy: gỡ hết con trỏ đã cấp trên Heap, tránh rò rỉ bộ nhớ
LibraryManager::~LibraryManager() {
    // Xóa sạch Sách
    Node<Book*>* curr_book = books.getHead();
    while (curr_book != nullptr) {
        delete curr_book->data;
        curr_book = curr_book->next;
    }

    // Xóa sạch Bạn đọc
    Node<Reader*>* curr_reader = readers.getHead();
    while (curr_reader != nullptr) {
        delete curr_reader->data;
        curr_reader = curr_reader->next;
    }

    // Xóa sạch Phiếu mượn
    Node<BorrowRecord*>* curr_record = borrow_records.getHead();
    while (curr_record != nullptr) {
        delete curr_record->data;
        curr_record = curr_record->next;
    }
}

// ==========================================
// QUẢN LÝ SÁCH
// ==========================================

bool LibraryManager::add_book(Book* book) {
    if (book_index.get(book->id) != nullptr) {
        return false; // Trùng mã sách rồi
    }
    // Chặn số lượng âm
    if (book->quantity < 0) {
        std::cout << "Loi: So luong sach khong duoc am!\n";
        return false;
    }
    books.insertAtTail(book);
    book_index.put(book->id, book);
    return true;
}

bool LibraryManager::remove_book(const std::string& book_id) {
    Book* b = book_index.get(book_id);
    if (b == nullptr) return false;

    // Duyệt coi có ai đang mượn cuốn này ko
    Node<BorrowRecord*>* curr = borrow_records.getHead();
    while (curr != nullptr) {
        if (curr->data->book_id == book_id && curr->data->status != "returned") {
            std::cout << "Loi: Khong the xoa sach nay vi dang co nguoi muon (Phieu: "
                      << curr->data->record_id << ")!\n";
            return false;
        }
        curr = curr->next;
    }

    book_index.remove(book_id);
    books.remove(b);
    delete b;
    return true;
}

bool LibraryManager::update_book(const std::string& book_id, const std::string& title,
                                  const std::string& author, const std::string& category, int quantity) {
    Book* b = book_index.get(book_id);
    if (b == nullptr) return false;

    // Chỉ cập nhật field nào có truyền vô (ko rỗng / ko âm)
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

    // Dồn LinkedList ra mảng tạm để xài thuật toán tìm kiếm bên dsa
    Book** arr = new Book*[sz];
    Node<Book*>* curr = books.getHead();
    size_t idx = 0;
    while (curr != nullptr) {
        arr[idx++] = curr->data;
        curr = curr->next;
    }

    // Dựng sách giả chỉ chứa tên để đưa vô linearSearch
    Book targetBook("", title, "", "", 0);
    Book* target = &targetBook;

    // Hàm so trùng: kiểm tra tên sách có chứa từ khóa ko
    auto matchTitle = [](Book* a, Book* b) -> bool {
        return a->title.find(b->title) != std::string::npos;
    };

    // Gọi linearSearch tìm bản ghi đầu tiên trùng
    int firstMatch = linearSearch(arr, sz, target, matchTitle);

    std::cout << "\n--- KET QUA TIM KIEM SACH ---\n";
    if (firstMatch == -1) {
        std::cout << "Khong tim thay sach nao co ten chua tu khoa: " << title << "\n";
    } else {
        // Quét lại hết mảng để in tất cả kết quả khớp
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

    // QuickSort giảm dần theo số lượt mượn
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
        return false; // Trùng mã
    }
    // Validate email (nếu có nhập)
    if (!reader->email.empty()) {
        size_t at_pos = reader->email.find('@');
        if (at_pos == std::string::npos || at_pos == 0 || at_pos == reader->email.length() - 1) {
            std::cout << "Loi: Email khong hop le (thieu @ hoac domain)!\n";
            return false;
        }
        // Phải có dấu chấm sau @
        if (reader->email.find('.', at_pos + 1) == std::string::npos) {
            std::cout << "Loi: Email khong hop le (domain thieu dau '.')\n";
            return false;
        }
    }
    // Validate số điện thoại (nếu có nhập, phải >= 10 số, toàn chữ số)
    if (!reader->phone.empty()) {
        if (reader->phone.length() < 10) {
            std::cout << "Loi: So dien thoai phai co it nhat 10 chu so!\n";
            return false;
        }
        for (char c : reader->phone) {
            if (c < '0' || c > '9') {
                std::cout << "Loi: So dien thoai chi duoc chua chu so!\n";
                return false;
            }
        }
    }
    readers.insertAtTail(reader);
    reader_index.put(reader->student_id, reader);
    return true;
}

bool LibraryManager::remove_reader(const std::string& reader_id) {
    Reader* r = reader_index.get(reader_id);
    if (r == nullptr) return false;

    // Cấm xóa nếu đang mượn sách
    Node<BorrowRecord*>* curr = borrow_records.getHead();
    while (curr != nullptr) {
        if (curr->data->reader_id == reader_id && curr->data->status != "returned") {
            std::cout << "Loi: Khong the xoa ban doc nay vi dang muon sach (Phieu: "
                      << curr->data->record_id << ")! Hay tra sach truoc.\n";
            return false;
        }
        curr = curr->next;
    }

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
    if (r->is_blocked) return false; // Khóa rồi, khỏi khóa nữa
    r->block_reader();
    return true;
}

bool LibraryManager::unblock_reader(const std::string& reader_id) {
    Reader* r = reader_index.get(reader_id);
    if (r == nullptr) return false;
    if (!r->is_blocked) return false; // Đang mở mà đòi mở nữa
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
// NGHIỆP VỤ MƯỢN / TRẢ SÁCH
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

    // Quét coi bạn đọc có nợ sách quá hạn chưa trả ko
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

    // Check giới hạn mượn
    int limit = (r->reader_type == "teacher") ? teacher_borrow_limit : student_borrow_limit;
    if (!r->can_borrow(limit)) {
        std::cout << "Loi: Ban doc da dat den han muc muon toi da cua minh (" << limit << " cuon)!\n";
        return false;
    }

    // Tạo phiếu mượn mới, ID tự sinh từ counter
    std::string new_record_id = "REC" + std::to_string(record_id_counter++);
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

    // Cộng lại 1 cuốn vô kho
    Book* b = find_book(target_record->book_id);
    if (b != nullptr) b->update_quantity(1);

    // Giảm số sách đang mượn của bạn đọc
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
// ĐỌC / GHI FILE TEXT (PHÂN CÁCH BỞI DẤU "|")
// ==========================================================

void LibraryManager::save_data() const {
    // 1. Ghi danh sách Sách
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

    // 2. Ghi danh sách Bạn đọc
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

    // 3. Ghi danh sách Phiếu mượn
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
    // Dọn sạch dữ liệu cũ trước khi load — tránh trùng lặp
    // 1. Xóa hết Book
    Node<Book*>* curr_book = books.getHead();
    while (curr_book != nullptr) {
        delete curr_book->data;
        curr_book = curr_book->next;
    }
    books.clear();
    book_index.clear();

    // 2. Xóa hết Reader
    Node<Reader*>* curr_reader = readers.getHead();
    while (curr_reader != nullptr) {
        delete curr_reader->data;
        curr_reader = curr_reader->next;
    }
    readers.clear();
    reader_index.clear();

    // 3. Xóa hết BorrowRecord
    Node<BorrowRecord*>* curr_record = borrow_records.getHead();
    while (curr_record != nullptr) {
        delete curr_record->data;
        curr_record = curr_record->next;
    }
    borrow_records.clear();

    // Reset counter về mốc
    record_id_counter = 1001;

    std::string line;

    // 1. Load Sách từ file books.txt
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

    // 2. Load Bạn đọc từ file readers.txt
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

    // 3. Load Phiếu mượn từ file records.txt
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

            // Canh lại counter để lần sau sinh ID ko bị đụng
            // Bóc phần số từ "REC1234"
            if (rec->record_id.length() > 3 && rec->record_id.substr(0, 3) == "REC") {
                try {
                    int id_num = std::stoi(rec->record_id.substr(3));
                    if (id_num >= record_id_counter) {
                        record_id_counter = id_num + 1;
                    }
                } catch (...) {
                    // Bỏ qua mấy ID lỗi
                }
            }
        }
        record_file.close();
    }
}

// ==========================================================
// THỐNG KÊ & BÁO CÁO
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

    // Đếm sách còn trong kho
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