/**
 * Library Management System - Enhanced Console UI
 *
 * Business Logic, Data Structure, TXT Storage, Algorithms: UNCHANGED.
 * Only Console UI layer upgraded.
 */
#include "lb_management/services/LibraryManager.h"
#include "lb_management/models/Book.h"
#include "lb_management/models/Reader.h"
#include "lb_management/models/BorrowRecord.h"
#include "dsa/data_structure/Node.h"
#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>

// ==========================================================
// UI CONSTANTS & HELPERS
// ==========================================================

const int PAGE_SIZE = 10;
const int COL_WIDTH_ID = 8;
const int COL_WIDTH_TITLE = 28;
const int COL_WIDTH_AUTHOR = 18;
const int COL_WIDTH_CATEGORY = 14;
const int COL_WIDTH_QTY = 8;
const int COL_WIDTH_STATUS = 14;

// Cross-platform clear screen
void clear_screen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void print_line(char ch = '=', int width = 60) {
    std::cout << std::string(width, ch) << "\n";
}

void print_header(const std::string& title) {
    print_line();
    std::cout << "  " << title << "\n";
    print_line();
}

// Message boxes
void show_success(const std::string& msg) {
    std::cout << "\n  [THANH CONG] " << msg << "\n";
}
void show_error(const std::string& msg) {
    std::cout << "\n  [LOI] " << msg << "\n";
}
void show_warning(const std::string& msg) {
    std::cout << "\n  [CANH BAO] " << msg << "\n";
}
void show_info(const std::string& msg) {
    std::cout << "\n  [INFO] " << msg << "\n";
}

// ==========================================================
// UNIFIED INPUT SYSTEM — all input via getline(), no cin>>
// No buffer issues, no blocking, no double-Enter bugs.
// ==========================================================

void press_enter_to_continue() {
    std::cout << "\n  Nhan Enter de tiep tuc...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

std::string read_string(const std::string& prompt, bool allow_empty = false) {
    std::string value;
    while (true) {
        std::cout << "  " << prompt;
        std::getline(std::cin, value);
        size_t s = value.find_first_not_of(" \t");
        if (s == std::string::npos) {
            if (allow_empty) return "";
            show_warning("Gia tri khong duoc de trong. Vui long nhap lai.");
            continue;
        }
        size_t e = value.find_last_not_of(" \t");
        value = value.substr(s, e - s + 1);
        return value;
    }
}

int read_int(const std::string& prompt, int min_val = 0) {
    while (true) {
        std::cout << "  " << prompt;
        std::string line;
        std::getline(std::cin, line);
        try {
            int v = std::stoi(line);
            if (v >= min_val) return v;
            show_warning("Gia tri phai >= " + std::to_string(min_val) + ". Vui long nhap lai.");
        } catch (...) {
            show_warning("So khong hop le. Vui long nhap so nguyen.");
        }
    }
}

int read_menu_choice(int max_option) {
    while (true) {
        std::cout << "\n  Nhap lua chon (0-" << max_option << "): ";
        std::string line;
        std::getline(std::cin, line);
        try {
            int c = std::stoi(line);
            if (c >= 0 && c <= max_option) return c;
        } catch (...) {}
        show_warning("Lua chon khong hop le. Vui long chon tu 0 den " + std::to_string(max_option) + ".");
    }
}

bool confirm_action(const std::string& prompt) {
    std::string yn = read_string(prompt + " (y/n): ");
    return (yn == "y" || yn == "Y" || yn == "yes" || yn == "YES");
}

// ==========================================================
// FORMAT HELPERS
// ==========================================================

std::string pad_right(const std::string& s, int w) {
    if ((int)s.length() >= w) return s.substr(0, w);
    return s + std::string(w - s.length(), ' ');
}

std::string pad_center(const std::string& s, int w) {
    if ((int)s.length() >= w) return s.substr(0, w);
    int left = (w - (int)s.length()) / 2;
    int right = w - (int)s.length() - left;
    return std::string(left, ' ') + s + std::string(right, ' ');
}

std::string book_status_label(Book* b) {
    if (b->quantity <= 0) return "Het sach     ";
    if (b->quantity == 1) return "Sap het      ";
    return "Con san       ";
}

std::string reader_status_label(Reader* r) {
    if (r->is_blocked) return "Bi KHOA        ";
    int limit = (r->reader_type == "teacher") ? 5 : 3;
    if (r->borrowed_count >= limit) return "Dat Gioi Han   ";
    return "Hoat Dong      ";
}

// ==========================================================
// TABLE DISPLAY HELPERS
// ==========================================================

void print_table_header(const std::vector<std::string>& cols, const std::vector<int>& widths) {
    std::cout << "  +";
    for (size_t i = 0; i < widths.size(); ++i)
        std::cout << std::string(widths[i] + 2, '-') << "+";
    std::cout << "\n  |";
    for (size_t i = 0; i < cols.size(); ++i)
        std::cout << " " << pad_center(cols[i], widths[i]) << " |";
    std::cout << "\n  +";
    for (size_t i = 0; i < widths.size(); ++i)
        std::cout << std::string(widths[i] + 2, '-') << "+";
    std::cout << "\n";
}

void print_table_separator(const std::vector<int>& widths) {
    std::cout << "  +";
    for (size_t i = 0; i < widths.size(); ++i)
        std::cout << std::string(widths[i] + 2, '-') << "+";
    std::cout << "\n";
}

void print_table_row(const std::vector<std::string>& row, const std::vector<int>& widths) {
    std::cout << "  |";
    for (size_t i = 0; i < row.size(); ++i)
        std::cout << " " << pad_right(row[i], widths[i]) << " |";
    std::cout << "\n";
}

void print_table_footer(const std::vector<int>& widths) {
    std::cout << "  +";
    for (size_t i = 0; i < widths.size(); ++i)
        std::cout << std::string(widths[i] + 2, '-') << "+";
    std::cout << "\n";
}

// ==========================================================
// DASHBOARD
// ==========================================================

void show_dashboard(LibraryManager& mgr) {
    clear_screen();
    print_header("LIBRARY MANAGEMENT SYSTEM");
    int total_books = (int)mgr.get_book_count();
    int total_readers = (int)mgr.get_reader_count();
    int active_borrows = 0;
    int overdue_count = 0;

    // Count from records
    // We iterate borrowed_records via the manager's display_all_records
    // Since we can't directly access the list, we use get_borrowing_count
    active_borrows = mgr.get_borrowing_count();

    // Count overdue - we need to scan records
    // show_overdue_books prints to cout, we'll just print the header line
    std::cout << "\n";
    std::cout << "  Sach trong thu vien:     " << std::setw(6) << total_books << "\n";
    std::cout << "  Ban doc dang ky:         " << std::setw(6) << total_readers << "\n";
    std::cout << "  Sach dang duoc muon:     " << std::setw(6) << active_borrows << "\n";
    std::cout << "  Sach con trong kho:      " << std::setw(6) << (total_books) << "\n";
    std::cout << "\n";
    print_line();
    std::cout << "\n  CHUC NANG CHINH\n\n";
    std::cout << "    1. Quan ly Sach (" << total_books << " cuon)\n";
    std::cout << "    2. Quan ly Ban doc (" << total_readers << " nguoi)\n";
    std::cout << "    3. Quan ly Muon Tra (" << active_borrows << " dang muon)\n";
    std::cout << "    4. Thong ke & Bao cao\n";
    std::cout << "\n    9. Luu du lieu\n";
    std::cout << "    0. Luu du lieu & Thoat\n";
    std::cout << "\n";
}

// ==========================================================
// BOOK MENU (Enhanced)
// ==========================================================

void show_books_table_paged(LibraryManager& mgr) {
    // Collect all books into vector for paging
    std::vector<Book*> book_list;
    // Accessing private data... we need to use public API only.
    // We'll call display_all_books with paging logic built in here.
    // But the manager already has display_all_books() which prints all.
    // We need to enhance it with paging here at UI level.
    // Since LinkedList is private, we must iterate differently.
    // Use the existing method but intercept output? No - need direct access.
    // We'll create a helper that uses find_book? No, that's O(n^2).
    // Best approach: We'll display with simple paging by capturing IDs
    // We know books are in the linked list. We need access.
    // Actually, we can use get_book_count() and iterate from 0...
    // No, LinkedList has getHead() but it's private to manager.
    // Simplest: just display all - but show count and sections.
    // For full paging, we'd need to add an API to LibraryManager.
    // BUT constraint says NO change to business logic.
    // Let's use what we have: display_all_books() is the only option.
    // We'll still format nicely but just call display_all_books() for now.
    // The enhanced table printing will happen inside display functions.
    mgr.display_all_books();
}

void book_menu(LibraryManager& mgr) {
    while (true) {
        clear_screen();
        print_header("QUAN LY SACH");
        size_t count = mgr.get_book_count();
        std::cout << "  Tong so sach: " << count << " cuon\n";
        print_line('-');
        std::cout << "\n    1. Them sach moi\n";
        std::cout << "    2. Tim sach (theo ma / ten)\n";
        std::cout << "    3. Xem danh sach sach\n";
        if (count > 0) {
            std::cout << "    4. Cap nhat thong tin sach\n";
            std::cout << "    5. Xoa sach\n";
            std::cout << "    6. Top sach duoc muon nhieu nhat\n";
        }
        std::cout << "\n    0. Quay lai\n\n";

        int max_opt = (count > 0) ? 6 : 3;
        std::string valid_opts = "(" + std::to_string(max_opt) + " lua chon)";
        int choice = read_menu_choice(max_opt);

        if (choice == 0) return;

        try {
            if (choice == 1) {
                // Add Book
                clear_screen();
                print_header("THEM SACH MOI");
                std::string id = read_string("Ma sach (ID): ");
                if (mgr.find_book(id) != nullptr) {
                    show_error("Ma sach '" + id + "' da ton tai trong he thong!");
                    press_enter_to_continue();
                    continue;
                }
                std::string title = read_string("Ten sach: ");
                std::string author = read_string("Tac gia: ");
                std::string category = read_string("The loai: ");
                int qty = read_int("So luong: ", 1);

                Book* nb = new Book(id, title, author, category, qty);
                if (mgr.add_book(nb)) {
                    show_success("Da them sach '" + title + "' thanh cong!");
                } else {
                    show_error("Khong the them sach!");
                    delete nb;
                }
                press_enter_to_continue();
            }
            else if (choice == 2) {
                // Search Book
                clear_screen();
                print_header("TIM SACH");
                std::cout << "\n    1. Tim theo ma sach\n";
                std::cout << "    2. Tim theo ten sach\n";
                std::cout << "\n    0. Quay lai\n\n";
                int sc = read_menu_choice(2);
                if (sc == 0) continue;
                if (sc == 1) {
                    std::string id = read_string("Nhap ma sach: ");
                    Book* b = mgr.find_book(id);
                    if (b == nullptr) {
                        show_error("Khong tim thay sach co ma: " + id);
                    } else {
                        std::cout << "\n  ";
                        b->display_info();
                        std::cout << "  Trang thai: " << book_status_label(b) << "\n";
                    }
                    press_enter_to_continue();
                } else {
                    std::string kw = read_string("Nhap tu khoa ten sach: ");
                    mgr.search_book_by_title(kw);
                    press_enter_to_continue();
                }
            }
            else if (choice == 3) {
                // View All Books
                clear_screen();
                print_header("DANH SACH SACH (" + std::to_string(count) + " cuon)");
                mgr.display_all_books();
                press_enter_to_continue();
            }
            else if (choice == 4 && count > 0) {
                // Update Book
                clear_screen();
                print_header("CAP NHAT THONG TIN SACH");
                std::string id = read_string("Ma sach can cap nhat: ");
                Book* b = mgr.find_book(id);
                if (b == nullptr) {
                    show_error("Khong tim thay sach co ma: " + id);
                    press_enter_to_continue();
                    continue;
                }
                std::cout << "\n  Thong tin hien tai:\n  ";
                b->display_info();
                std::cout << "  Trang thai: " << book_status_label(b) << "\n\n";
                std::cout << "  (De trong neu khong muon thay doi)\n";
                std::string title = read_string("Ten sach moi: ", true);
                std::string author = read_string("Tac gia moi: ", true);
                std::string category = read_string("The loai moi: ", true);
                std::string qty_str = read_string("So luong moi (>=0): ", true);
                int qty = -1;
                if (!qty_str.empty()) {
                    try { qty = std::stoi(qty_str); } catch (...) { qty = -1; }
                }
                if (title.empty() && author.empty() && category.empty() && qty < 0) {
                    show_info("Khong co thay doi nao.");
                } else {
                    mgr.update_book(id, title, author, category, qty);
                    show_success("Da cap nhat sach thanh cong!");
                }
                press_enter_to_continue();
            }
            else if (choice == 5 && count > 0) {
                // Delete Book
                clear_screen();
                print_header("XOA SACH");
                std::string id = read_string("Ma sach can xoa: ");
                Book* b = mgr.find_book(id);
                if (b == nullptr) {
                    show_error("Khong tim thay sach co ma: " + id);
                    press_enter_to_continue();
                    continue;
                }
                std::cout << "\n  ";
                b->display_info();
                std::cout << "\n";
                if (confirm_action("Ban co chac chan muon xoa sach nay?")) {
                    if (mgr.remove_book(id)) {
                        show_success("Da xoa sach thanh cong!");
                    } else {
                        show_error("Khong the xoa sach (sach dang duoc muon).");
                    }
                } else {
                    show_info("Da huy thao tac xoa.");
                }
                press_enter_to_continue();
            }
            else if (choice == 6 && count > 0) {
                clear_screen();
                print_header("TOP SACH DUOC MUON NHIEU NHAT");
                int n = read_int("So luong sach muon xem: ", 1);
                mgr.show_top_books(n);
                press_enter_to_continue();
            }
        } catch (const std::exception& e) {
            show_error(std::string("Loi he thong: ") + e.what());
            press_enter_to_continue();
        }
    }
}

// ==========================================================
// READER MENU (Enhanced)
// ==========================================================

void reader_menu(LibraryManager& mgr) {
    while (true) {
        clear_screen();
        print_header("QUAN LY BAN DOC");
        size_t count = mgr.get_reader_count();
        std::cout << "  Tong so ban doc: " << count << " nguoi\n";
        print_line('-');
        std::cout << "\n    1. Them ban doc moi\n";
        std::cout << "    2. Tim ban doc theo ma\n";
        std::cout << "    3. Xem danh sach ban doc\n";
        if (count > 0) {
            std::cout << "    4. Khoa ban doc\n";
            std::cout << "    5. Mo khoa ban doc\n";
            std::cout << "    6. Xoa ban doc\n";
        }
        std::cout << "\n    0. Quay lai\n\n";

        int max_opt = (count > 0) ? 6 : 3;
        int choice = read_menu_choice(max_opt);

        if (choice == 0) return;

        try {
            if (choice == 1) {
                clear_screen();
                print_header("THEM BAN DOC MOI");
                std::string id = read_string("Ma ban doc (MSV/MSG): ");
                if (mgr.find_reader(id) != nullptr) {
                    show_error("Ma ban doc '" + id + "' da ton tai!");
                    press_enter_to_continue();
                    continue;
                }
                std::string name = read_string("Ho ten: ");
                std::string type;
                while (true) {
                    type = read_string("Loai (student/teacher): ");
                    if (type == "student" || type == "teacher") break;
                    show_warning("Chi chap nhan 'student' hoac 'teacher'.");
                }
                std::string phone = read_string("So dien thoai: ", true);
                std::string email = read_string("Email: ", true);

                Reader* nr = new Reader(id, name, type, phone, email);
                if (mgr.add_reader(nr)) {
                    show_success("Da them ban doc '" + name + "' thanh cong!");
                } else {
                    show_error("Khong the them ban doc!");
                    delete nr;
                }
                press_enter_to_continue();
            }
            else if (choice == 2) {
                clear_screen();
                print_header("TIM BAN DOC");
                std::string id = read_string("Nhap ma ban doc: ");
                Reader* r = mgr.find_reader(id);
                if (r == nullptr) {
                    show_error("Khong tim thay ban doc co ma: " + id);
                } else {
                    std::cout << "\n  ";
                    r->display_info();
                    std::cout << "  Trang thai: " << reader_status_label(r) << "\n";
                }
                press_enter_to_continue();
            }
            else if (choice == 3) {
                clear_screen();
                print_header("DANH SACH BAN DOC (" + std::to_string(count) + " nguoi)");
                mgr.display_all_readers();
                press_enter_to_continue();
            }
            else if (choice == 4 && count > 0) {
                clear_screen();
                print_header("KHOA BAN DOC");
                std::string id = read_string("Ma ban doc can khoa: ");
                Reader* r = mgr.find_reader(id);
                if (r == nullptr) {
                    show_error("Khong tim thay ban doc!");
                } else if (r->is_blocked) {
                    show_warning("Ban doc nay da bi khoa truoc do.");
                } else {
                    if (confirm_action("Xac nhan khoa ban doc '" + id + "'?")) {
                        mgr.block_reader(id);
                        show_success("Da khoa ban doc thanh cong!");
                    }
                }
                press_enter_to_continue();
            }
            else if (choice == 5 && count > 0) {
                clear_screen();
                print_header("MO KHOA BAN DOC");
                std::string id = read_string("Ma ban doc can mo khoa: ");
                Reader* r = mgr.find_reader(id);
                if (r == nullptr) {
                    show_error("Khong tim thay ban doc!");
                } else if (!r->is_blocked) {
                    show_warning("Ban doc nay khong bi khoa.");
                } else {
                    if (confirm_action("Xac nhan mo khoa ban doc '" + id + "'?")) {
                        mgr.unblock_reader(id);
                        show_success("Da mo khoa ban doc thanh cong!");
                    }
                }
                press_enter_to_continue();
            }
            else if (choice == 6 && count > 0) {
                clear_screen();
                print_header("XOA BAN DOC");
                std::string id = read_string("Ma ban doc can xoa: ");
                Reader* r = mgr.find_reader(id);
                if (r == nullptr) {
                    show_error("Khong tim thay ban doc!");
                } else {
                    std::cout << "\n  ";
                    r->display_info();
                    if (r->borrowed_count > 0) {
                        show_warning("Ban doc dang muon " + std::to_string(r->borrowed_count) + " sach. Hay tra sach truoc khi xoa.");
                        press_enter_to_continue();
                        continue;
                    }
                    if (confirm_action("Ban co chac chan muon xoa ban doc nay?")) {
                        if (mgr.remove_reader(id)) {
                            show_success("Da xoa ban doc thanh cong!");
                        } else {
                            show_error("Khong the xoa ban doc (dang muon sach).");
                        }
                    } else {
                        show_info("Da huy thao tac xoa.");
                    }
                }
                press_enter_to_continue();
            }
        } catch (const std::exception& e) {
            show_error(std::string("Loi he thong: ") + e.what());
            press_enter_to_continue();
        }
    }
}

// ==========================================================
// BORROW MENU (Enhanced)
// ==========================================================

void borrow_menu(LibraryManager& mgr) {
    while (true) {
        clear_screen();
        print_header("QUAN LY MUON TRA");
        int active = mgr.get_borrowing_count();
        size_t total = mgr.get_record_count();
        std::cout << "  Dang muon: " << active << " | Tong phieu: " << total << "\n";
        print_line('-');
        std::cout << "\n    1. Muon sach\n";
        std::cout << "    2. Tra sach\n";
        std::cout << "    3. Tim phieu muon\n";
        if (total > 0) {
            std::cout << "    4. Xem danh sach phieu muon\n";
            std::cout << "    5. Xem sach qua han\n";
        }
        std::cout << "\n    0. Quay lai\n\n";

        int max_opt = (total > 0) ? 5 : 3;
        int choice = read_menu_choice(max_opt);

        if (choice == 0) return;

        try {
            if (choice == 1) {
                clear_screen();
                print_header("MUON SACH");
                std::string rid = read_string("Ma ban doc: ");
                std::string bid = read_string("Ma sach: ");
                
                // Preview before borrow
                Reader* r = mgr.find_reader(rid);
                Book* b = mgr.find_book(bid);
                if (r != nullptr && b != nullptr) {
                    std::cout << "\n  Ban doc: " << r->full_name << " | Loai: " << r->reader_type << "\n";
                    std::cout << "  Sach: " << b->title << " | Con: " << b->quantity << " cuon\n";
                    if (r->is_blocked) {
                        show_error("Ban doc dang bi KHOA, khong the muon sach!");
                        press_enter_to_continue();
                        continue;
                    }
                    if (!b->is_available()) {
                        show_error("Sach da het trong kho!");
                        press_enter_to_continue();
                        continue;
                    }
                }
                
                std::string bdate = read_string("Ngay muon (DD/MM/YYYY): ");
                std::string ddate = read_string("Han tra (DD/MM/YYYY): ");
                
                if (mgr.borrow_book(rid, bid, bdate, ddate)) {
                    show_success("Muon sach thanh cong!");
                }
                press_enter_to_continue();
            }
            else if (choice == 2) {
                clear_screen();
                print_header("TRA SACH");
                std::string rec_id = read_string("Ma phieu muon: ");
                std::string rdate = read_string("Ngay tra (DD/MM/YYYY): ");
                if (mgr.return_book(rec_id, rdate)) {
                    // Success message already in business logic
                }
                press_enter_to_continue();
            }
            else if (choice == 3) {
                clear_screen();
                print_header("TIM PHIEU MUON");
                std::string rid = read_string("Ma phieu muon: ");
                BorrowRecord* rec = mgr.find_record(rid);
                if (rec == nullptr) {
                    show_error("Khong tim thay phieu muon: " + rid);
                } else {
                    std::cout << "\n  ";
                    rec->display_info();
                }
                press_enter_to_continue();
            }
            else if (choice == 4 && total > 0) {
                clear_screen();
                print_header("DANH SACH PHIEU MUON (" + std::to_string(total) + " phieu)");
                mgr.display_all_records();
                press_enter_to_continue();
            }
            else if (choice == 5 && total > 0) {
                clear_screen();
                print_header("SACH QUA HAN");
                std::string cur_date = read_string("Nhap ngay hien tai (DD/MM/YYYY): ");
                mgr.show_overdue_books(cur_date);
                press_enter_to_continue();
            }
        } catch (const std::exception& e) {
            show_error(std::string("Loi he thong: ") + e.what());
            press_enter_to_continue();
        }
    }
}

// ==========================================================
// STATISTICS MENU (Enhanced)
// ==========================================================

void statistics_menu(LibraryManager& mgr) {
    while (true) {
        clear_screen();
        print_header("THONG KE & BAO CAO");
        
        // Live statistics
        size_t books = mgr.get_book_count();
        size_t readers = mgr.get_reader_count();
        size_t records = mgr.get_record_count();
        int active = mgr.get_borrowing_count();
        
        std::cout << "\n  TONG QUAN HE THONG\n";
        print_line('-', 50);
        std::cout << "  Tong so sach:              " << std::setw(6) << books << "\n";
        std::cout << "  Tong so ban doc:           " << std::setw(6) << readers << "\n";
        std::cout << "  Tong so giao dich:         " << std::setw(6) << records << "\n";
        std::cout << "  Sach dang duoc muon:       " << std::setw(6) << active << "\n";
        print_line('-', 50);
        
        std::cout << "\n    1. Bao cao chi tiet\n";
        std::cout << "    2. Xem sach qua han\n";
        std::cout << "    3. Luu du lieu xuong file\n";
        std::cout << "    4. Tai du lieu tu file\n";
        std::cout << "\n    0. Quay lai\n\n";
        
        int choice = read_menu_choice(4);
        
        if (choice == 0) return;
        
        try {
            if (choice == 1) {
                clear_screen();
                mgr.generate_report();
                press_enter_to_continue();
            }
            else if (choice == 2) {
                clear_screen();
                print_header("SACH QUA HAN");
                std::string cd = read_string("Nhap ngay hien tai (DD/MM/YYYY): ");
                mgr.show_overdue_books(cd);
                press_enter_to_continue();
            }
            else if (choice == 3) {
                mgr.save_data();
                show_success("Da luu du lieu thanh cong!");
                std::cout << "  Sach: " << mgr.get_book_count() << "\n";
                std::cout << "  Ban doc: " << mgr.get_reader_count() << "\n";
                std::cout << "  Phieu muon: " << mgr.get_record_count() << "\n";
                press_enter_to_continue();
            }
            else if (choice == 4) {
                size_t bb = mgr.get_book_count();
                size_t br = mgr.get_reader_count();
                size_t b_rec = mgr.get_record_count();
                mgr.load_data();
                show_success("Da tai du lieu thanh cong!");
                std::cout << "  Sach: " << mgr.get_book_count() << " (truoc: " << bb << ")\n";
                std::cout << "  Ban doc: " << mgr.get_reader_count() << " (truoc: " << br << ")\n";
                std::cout << "  Phieu muon: " << mgr.get_record_count() << " (truoc: " << b_rec << ")\n";
                press_enter_to_continue();
            }
        } catch (const std::exception& e) {
            show_error(std::string("Loi he thong: ") + e.what());
            press_enter_to_continue();
        }
    }
}

// ==========================================================
// MAIN
// ==========================================================

int main() {
    LibraryManager manager("./data/");

    // Startup - load data
    manager.load_data();

    while (true) {
        show_dashboard(manager);
        int choice = read_menu_choice(4);

        if (choice == 0) {
            // Save and exit
            manager.save_data();
            clear_screen();
            print_header("TAM BIET!");
            std::cout << "\n  Du lieu da duoc luu an toan.\n";
            std::cout << "  Hen gap lai!\n\n";
            print_line();
            break;
        }
        else if (choice == 9) {
            // Save only
            manager.save_data();
            show_success("Da luu du lieu thanh cong!");
            press_enter_to_continue();
        }
        else if (choice == 1) {
            book_menu(manager);
        }
        else if (choice == 2) {
            reader_menu(manager);
        }
        else if (choice == 3) {
            borrow_menu(manager);
        }
        else if (choice == 4) {
            statistics_menu(manager);
        }
    }

    return 0;
}
