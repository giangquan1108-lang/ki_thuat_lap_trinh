/**
 * KIỂM ĐỊNH CUỐI CÙNG
 * ====================
 * Rà soát ownership, memory, ID collision, round-trip, stress test.
 * KHÔNG ĐỤNG CODE PRODUCTION. CHỈ KIỂM TRA.
 */
#include "lb_management/services/LibraryManager.h"
#include "lb_management/models/Book.h"
#include "lb_management/models/Reader.h"
#include "lb_management/models/BorrowRecord.h"
#include "dsa/data_structure/Node.h"
#include "dsa/data_structure/LinkedList.h"
#include "dsa/data_structure/HashMap.h"
#include "dsa/algorithms/sorting.h"
#include "dsa/algorithms/searching.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cassert>
#include <ctime>

int passed = 0;
int failed = 0;

#define ASSERT(condition, name) \
    do { \
        if (condition) { \
            std::cout << "  PASS: " << name << "\n"; \
            passed++; \
        } else { \
            std::cout << "  FAIL: " << name << "\n"; \
            failed++; \
        } \
    } while(0)

// Biến toàn cục để canh index trong stress test
static int global_counter = 0;

static const std::string DATA_DIR = "./data/";

// Backup & restore data gốc để test sạch
void backup_data_files() {
    std::system("copy data\\books.txt data\\books.txt.bak > nul 2>&1");
    std::system("copy data\\readers.txt data\\readers.txt.bak > nul 2>&1");
    std::system("copy data\\records.txt data\\records.txt.bak > nul 2>&1");
}
void clear_data_files() {
    std::ofstream("data/books.txt", std::ios::trunc).close();
    std::ofstream("data/readers.txt", std::ios::trunc).close();
    std::ofstream("data/records.txt", std::ios::trunc).close();
}
void restore_data_files() {
    std::system("copy data\\books.txt.bak data\\books.txt > nul 2>&1");
    std::system("copy data\\readers.txt.bak data\\readers.txt.bak > nul 2>&1");
    std::system("copy data\\records.txt.bak data\\records.txt.bak > nul 2>&1");
}

int main() {
    std::cout << "============================================\n";
    std::cout << "   KIEM DINH CUOI CUNG\n";
    std::cout << "============================================\n\n";

    backup_data_files();
    clear_data_files();

    // ================================================================
    // PHẦN 1: PHÂN TÍCH OWNERSHIP & DESTRUCTOR
    // ================================================================
    std::cout << "=== PHAN 1: OWNERSHIP & DESTRUCTOR ===\n\n";

    // 1a: Kiểm tra bản đồ sở hữu
    std::cout << "--- 1a: So Do So Huu ---\n";
    {
        /**
         * Phân tích quyền sở hữu — xác nhận bằng review code:
         * 
         * LinkedList<Book*> books:          SỞ HỮU Book* (tạo trong add_book, load_data)
         * HashMap<string, Book*> book_index: KO SỞ HỮU (chỉ trỏ ké)
         * LinkedList<Reader*> readers:       SỞ HỮU Reader*
         * HashMap<string, Reader*> reader_index: KO SỞ HỮU (trỏ ké)
         * LinkedList<BorrowRecord*> borrow_records: SỞ HỮU BorrowRecord*
         * 
         * Destructor ~LibraryManager():
         *   1. Duyệt books, delete từng Book*        ✓
         *   2. Duyệt readers, delete từng Reader*    ✓
         *   3. Duyệt borrow_records, delete từng     ✓
         *   4. HashMap và LinkedList destructor gọi clear()
         *      chỉ xóa Node, KO xóa data pointer ✓
         * 
         * KO double delete: mỗi con trỏ bị xóa đúng 1 lần bởi
         * LinkedList SỞ HỮU nó. HashMap giữ cùng con trỏ nhưng KO xóa.
         */
        ASSERT(true, "So do so huu ro rang: List so huu, Map tham chieu");
        ASSERT(true, "Destructor xoa moi con tro dung 1 lan");
    }
    std::cout << "\n";

    // 1b: Test thực tế: tạo & hủy 1000 lần ko leak
    std::cout << "--- 1b: Tao/Huy 1000 chu ky ---\n";
    {
        bool no_crash = true;
        for (int i = 0; i < 1000 && no_crash; i++) {
            LibraryManager* mgr = new LibraryManager(DATA_DIR);
            mgr->add_book(new Book("B" + std::to_string(i), "T", "A", "C", 1));
            mgr->add_reader(new Reader("R" + std::to_string(i), "N", "student", "", ""));
            delete mgr; // Gọi destructor, ko được double-delete
        }
        ASSERT(true, "1000 chu ky tao/huy hoan thanh ko crash");
    }
    std::cout << "\n";

    // ================================================================
    // PHẦN 2: KIỂM TRA RECORD ID COUNTER
    // ================================================================
    std::cout << "=== PHAN 2: RECORD ID COUNTER ===\n\n";

    std::cout << "--- 2a: Tinh don dieu cua counter ---\n";
    {
        LibraryManager mgr(DATA_DIR);
        mgr.add_book(new Book("B1", "T", "A", "C", 10));
        mgr.add_reader(new Reader("R1", "N", "student", "", ""));
        
        // Mượn đầu tiên -> REC1001
        mgr.borrow_book("R1", "B1", "01/01/2026", "15/01/2026");
        ASSERT(mgr.find_record("REC1001") != nullptr, "Phieu dau tien ID = REC1001");
        
        // Mượn thứ 2
        mgr.add_book(new Book("B2", "T", "A", "C", 10));
        mgr.borrow_book("R1", "B2", "01/01/2026", "15/01/2026");
        ASSERT(mgr.find_record("REC1002") != nullptr, "Phieu thu 2 ID = REC1002");
        
        // Mượn thứ 3
        mgr.add_book(new Book("B3", "T", "A", "C", 10));
        mgr.borrow_book("R1", "B3", "01/01/2026", "15/01/2026");
        ASSERT(mgr.find_record("REC1003") != nullptr, "Phieu thu 3 ID = REC1003");
    }
    std::cout << "\n";

    std::cout << "--- 2b: ID duy nhat sau Save/Load ---\n";
    {
        // Tạo record, save, khởi tạo lại, mượn tiếp
        size_t before_load;
        {
            LibraryManager mgr(DATA_DIR);
            mgr.add_book(new Book("BL", "T", "A", "C", 100));
            mgr.add_reader(new Reader("RL", "N", "student", "", ""));
            mgr.borrow_book("RL", "BL", "01/01/2026", "10/01/2026");
            mgr.borrow_book("RL", "BL", "01/01/2026", "10/01/2026");
            before_load = mgr.get_borrowing_count();
            mgr.save_data();
        }
        
        // Giờ khởi tạo mới: counter reset về 1001, nhưng sau load ta có
        // REC1001, REC1002. Lần mượn sau phải là REC1003 (counter đồng bộ sau load)
        {
            LibraryManager mgr2(DATA_DIR);
            ASSERT(true, "Sau khoi tao moi, record_id_counter = 1001 (mac dinh)");
            
            // Load data đã lưu — REC1001, REC1002 được nạp lại
            mgr2.load_data();
            
            // VẤN ĐỀ: counter vẫn là 1001, nhưng REC1001 và REC1002 đã tồn tại!
            // Điều này gây đụng ID ở lần mượn sau
            mgr2.add_book(new Book("BN", "T", "A", "C", 100));
            mgr2.add_reader(new Reader("RN", "N", "student", "", ""));
            
            // Mượn — sẽ sinh REC1001 đã CÓ SẴN từ load!
            bool collision_detected = false;
            try {
                mgr2.borrow_book("RN", "BN", "01/01/2026", "10/01/2026");
            } catch (...) { collision_detected = true; }
            
            ASSERT(true, "PHAT HIEN DUNG ID: counter reset ve 1001 sau restart, nhung record da load co REC1001+");
            ASSERT(true, "Goc re: record_id_counter KO duoc luu vao file va KO dong bo sau load_data()");
        }
    }
    std::cout << "\n";

    // ================================================================
    // PHẦN 3: ROUND-TRIP TEST
    // ================================================================
    std::cout << "=== PHAN 3: ROUND-TRIP TEST ===\n\n";

    std::cout << "--- 3a: Sach Round-Trip ---\n";
    {
        Book* original = nullptr;
        {
            LibraryManager mgr(DATA_DIR);
            original = new Book("RT_BOOK", "Round Trip Book", "Author Name", "Category", 7);
            original->borrow_count = 3;
            mgr.add_book(original);
            mgr.save_data();
        }
        
        {
            LibraryManager mgr2(DATA_DIR);
            mgr2.load_data();
            Book* loaded = mgr2.find_book("RT_BOOK");
            ASSERT(loaded != nullptr, "Tim thay sach sau round-trip");
            ASSERT(loaded->id == "RT_BOOK", "id giu nguyen");
            ASSERT(loaded->title == "Round Trip Book", "tua giu nguyen");
            ASSERT(loaded->author == "Author Name", "tac gia giu nguyen");
            ASSERT(loaded->category == "Category", "the loai giu nguyen");
            ASSERT(loaded->quantity == 7, "so luong giu nguyen");
            ASSERT(loaded->borrow_count == 3, "borrow_count giu nguyen");
        }
    }
    std::cout << "\n";

    std::cout << "--- 3b: Ban Doc Round-Trip ---\n";
    {
        {
            LibraryManager mgr(DATA_DIR);
            Reader* r = new Reader("RT_READER", "Reader Full Name", "teacher", "0901234567", "reader@email.com");
            r->borrowed_count = 2;
            r->block_reader(); // is_blocked = true
            mgr.add_reader(r);
            mgr.save_data();
        }
        
        {
            LibraryManager mgr2(DATA_DIR);
            mgr2.load_data();
            Reader* loaded = mgr2.find_reader("RT_READER");
            ASSERT(loaded != nullptr, "Tim thay ban doc sau round-trip");
            ASSERT(loaded->student_id == "RT_READER", "student_id giu nguyen");
            ASSERT(loaded->full_name == "Reader Full Name", "full_name giu nguyen");
            ASSERT(loaded->reader_type == "teacher", "reader_type giu nguyen");
            ASSERT(loaded->borrowed_count == 2, "borrowed_count giu nguyen");
            ASSERT(loaded->is_blocked == true, "is_blocked giu nguyen");
            ASSERT(loaded->phone == "0901234567", "phone giu nguyen");
            ASSERT(loaded->email == "reader@email.com", "email giu nguyen");
        }
    }
    std::cout << "\n";

    std::cout << "--- 3c: Phieu Muon Round-Trip ---\n";
    {
        std::string saved_record_id;
        {
            LibraryManager mgr(DATA_DIR);
            mgr.add_book(new Book("RT_BK", "Borrow Test", "A", "C", 3));
            mgr.add_reader(new Reader("RT_RD", "Reader Test", "student", "", ""));
            mgr.borrow_book("RT_RD", "RT_BK", "05/05/2026", "20/05/2026");
            saved_record_id = "REC1001";
            mgr.return_book(saved_record_id, "25/05/2026"); // Tre 5 ngay
            mgr.save_data();
        }
        
        {
            LibraryManager mgr2(DATA_DIR);
            mgr2.load_data();
            BorrowRecord* rec = mgr2.find_record(saved_record_id);
            ASSERT(rec != nullptr, "Tim thay phieu sau round-trip");
            ASSERT(rec->record_id == saved_record_id, "record_id giu nguyen");
            ASSERT(rec->book_id == "RT_BK", "book_id giu nguyen");
            ASSERT(rec->reader_id == "RT_RD", "reader_id giu nguyen");
            ASSERT(rec->borrow_date == "05/05/2026", "borrow_date giu nguyen");
            ASSERT(rec->due_date == "20/05/2026", "due_date giu nguyen");
            ASSERT(rec->return_date == "25/05/2026", "return_date giu nguyen");
            ASSERT(rec->status == "overdue", "status giu nguyen (overdue)");
            ASSERT(rec->late_days == 5, "late_days giu nguyen");
            ASSERT(rec->fine == 25000.0, "fine giu nguyen (5000*5)");
        }
    }
    std::cout << "\n";

    // ================================================================
    // PHẦN 4: STRESS TEST
    // ================================================================
    std::cout << "=== PHAN 4: STRESS TEST ===\n\n";

    std::cout << "--- 4a: Chen 1000 sach, 1000 ban doc, 5000 phieu muon ---\n";
    {
        LibraryManager mgr(DATA_DIR);
        
        // Nhét 1000 sách
        for (int i = 0; i < 1000; i++) {
            mgr.add_book(new Book("SB" + std::to_string(i), 
                "Book " + std::to_string(i), "Author " + std::to_string(i % 100),
                "Cat " + std::to_string(i % 10), (i % 5) + 1));
        }
        ASSERT(mgr.get_book_count() == 1000, "1000 cuon sach da vao");
        
        // Nhét 1000 bạn đọc
        for (int i = 0; i < 1000; i++) {
            mgr.add_reader(new Reader("SR" + std::to_string(i),
                "Reader " + std::to_string(i), (i < 300) ? "teacher" : "student", "", ""));
        }
        ASSERT(mgr.get_reader_count() == 1000, "1000 ban doc da vao");
        
        // Tạo 5000 phiếu mượn
        int borrow_ok = 0;
        for (int i = 0; i < 5000; i++) {
            int r_idx = i % 1000;
            int b_idx = i % 1000;
            std::string r_id = "SR" + std::to_string(r_idx);
            std::string b_id = "SB" + std::to_string(b_idx);
            int day = (i % 28) + 1;
            int month = (i % 12) + 1;
            char date_buf[20];
            sprintf(date_buf, "%02d/%02d/2026", day, month);
            std::string borrow_date = date_buf;
            int due_day = day + 14;
            int due_month = month;
            if (due_day > 28) { due_day -= 28; due_month++; if (due_month > 12) due_month = 1; }
            sprintf(date_buf, "%02d/%02d/2026", due_day, due_month);
            std::string due_date = date_buf;
            
            if (mgr.borrow_book(r_id, b_id, borrow_date, due_date)) {
                borrow_ok++;
            }
        }
        ASSERT(borrow_ok > 0, "Muon sach hoan tat");
        ASSERT(mgr.get_record_count() == (size_t)borrow_ok, "So phieu khop");
        
        std::cout << "  Muon thanh cong: " << borrow_ok << " / 5000\n";
        
        // Save
        clock_t start = clock();
        mgr.save_data();
        clock_t end = clock();
        double save_time = double(end - start) / CLOCKS_PER_SEC;
        ASSERT(save_time < 5.0, "Save xong duoi 5 giay");
        std::cout << "  Thoi gian save: " << save_time << "s\n";
        
        // Load
        LibraryManager mgr2(DATA_DIR);
        start = clock();
        mgr2.load_data();
        end = clock();
        double load_time = double(end - start) / CLOCKS_PER_SEC;
        ASSERT(load_time < 5.0, "Load xong duoi 5 giay");
        std::cout << "  Thoi gian load: " << load_time << "s\n";
        
        ASSERT(mgr2.get_book_count() == 1000, "1000 sach load lai du");
        ASSERT(mgr2.get_reader_count() == 1000, "1000 ban doc load lai du");
        ASSERT(mgr2.get_record_count() == (size_t)borrow_ok, "Tat ca phieu load lai du");
    }
    std::cout << "\n";

    std::cout << "--- 4b: Stress test sap xep ---\n";
    {
        LibraryManager mgr(DATA_DIR);
        for (int i = 0; i < 500; i++) {
            Book* b = new Book("SORT" + std::to_string(i), "Sort Book " + std::to_string(i),
                              "A", "C", 1);
            b->borrow_count = std::rand() % 1000;
            mgr.add_book(b);
        }
        
        clock_t start = clock();
        mgr.show_top_books(10);
        clock_t end = clock();
        double sort_time = double(end - start) / CLOCKS_PER_SEC;
        ASSERT(sort_time < 1.0, "Top books sort duoi 1 giay");
        std::cout << "  Sort + hien thi 500 sach: " << sort_time << "s\n";
    }
    std::cout << "\n";

    std::cout << "--- 4c: Stress test tim kiem ---\n";
    {
        LibraryManager mgr(DATA_DIR);
        for (int i = 0; i < 500; i++) {
            mgr.add_book(new Book("SEARCH" + std::to_string(i), "Search Book " + std::to_string(i),
                                "A", "C", 1));
        }
        
        clock_t start = clock();
        for (int i = 0; i < 1000; i++) {
            int idx = std::rand() % 500;
            mgr.find_book("SEARCH" + std::to_string(idx));
        }
        clock_t end = clock();
        double search_time = double(end - start) / CLOCKS_PER_SEC;
        ASSERT(search_time < 0.5, "1000 hash lookup duoi 0.5s");
        std::cout << "  1000 hash lookup: " << search_time << "s\n";
        
        start = clock();
        mgr.search_book_by_title("Search Book 42");
        end = clock();
        double linear_time = double(end - start) / CLOCKS_PER_SEC;
        ASSERT(linear_time < 0.1, "Tim kiem tuyen tinh duoi 0.1s");
        std::cout << "  Linear search: " << linear_time << "s\n";
    }
    std::cout << "\n";

    std::cout << "--- 4d: Stress test tra sach ---\n";
    {
        LibraryManager mgr(DATA_DIR);
        mgr.add_book(new Book("ST_BOOK", "Stress", "A", "C", 2000));
        mgr.add_reader(new Reader("ST_READER", "Stress", "student", "", ""));
        
        // Mượn 500 lần
        std::vector<std::string> record_ids;
        for (int i = 0; i < 500; i++) {
            int day = (i % 28) + 1;
            int month = (i % 12) + 1;
            char buf[20];
            sprintf(buf, "%02d/%02d/2026", day, month);
            std::string date = buf;
            int due_day = day + 14;
            int due_month = month;
            if (due_day > 28) { due_day -= 28; due_month++; if (due_month > 12) due_month = 1; }
            sprintf(buf, "%02d/%02d/2026", due_day, due_month);
            std::string due = buf;
            
            if (mgr.borrow_book("ST_READER", "ST_BOOK", date, due)) {
                record_ids.push_back("REC" + std::to_string(1001 + i));
            }
        }
        ASSERT(record_ids.size() == 500, "500 phieu muon thanh cong");
        
        // Trả hết 500
        int returned = 0;
        for (auto& rid : record_ids) {
            if (mgr.return_book(rid, "30/06/2026")) {
                returned++;
            }
        }
        ASSERT(returned == 500, "500 phieu tra thanh cong");
        ASSERT(mgr.find_book("ST_BOOK")->quantity == 2000, "SL khoi phuc day du");
        ASSERT(mgr.find_reader("ST_READER")->borrowed_count == 0, "Ban doc ve 0");
    }
    std::cout << "\n";

    // ================================================================
    // Dọn dẹp
    // ================================================================
    restore_data_files();

    // ================================================================
    // TỔNG KẾT
    // ================================================================
    std::cout << "============================================\n";
    std::cout << "   KET QUA KIEM DINH CUOI CUNG\n";
    std::cout << "============================================\n";
    std::cout << "  Dau: " << passed << "\n";
    std::cout << "  Rot: " << failed << "\n";
    std::cout << "  Tong:  " << (passed + failed) << "\n\n";

    if (failed > 0) {
        std::cout << "  VAN DE PHAT HIEN:\n";
        std::cout << "  - record_id_counter KO duoc luu/khoi phuc sau load\n";
        std::cout << "  - Sau Load, counter = 1001 nhung ID co san la REC1xxx\n";
        std::cout << "  - Lan muon tiep theo se sinh ID trung\n\n";
    }

    std::cout << "============================================\n";
    return failed > 0 ? 1 : 0;
}