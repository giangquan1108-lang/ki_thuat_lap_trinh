#ifndef BORROWRECORD_H
#define BORROWRECORD_H

#include <string>
#include <iostream>

class BorrowRecord {
public:
    std::string record_id;
    std::string book_id;
    std::string reader_id;
    std::string borrow_date;      // Định dạng: DD/MM/YYYY
    std::string due_date;         // Hạn trả: DD/MM/YYYY
    std::string return_date;      // Ngày trả thực tế (Rỗng nếu chưa trả)
    std::string status;           // "borrowing", "returned", "overdue"
    double fine;
    int late_days;

    // Constructors
    BorrowRecord() : record_id(""), book_id(""), reader_id(""), borrow_date(""), 
                     due_date(""), return_date(""), status("borrowing"), fine(0.0), late_days(0) {}

    BorrowRecord(std::string rec_id, std::string b_id, std::string r_id, std::string b_date, std::string d_date)
        : record_id(rec_id), book_id(b_id), reader_id(r_id), borrow_date(b_date), 
          due_date(d_date), return_date(""), status("borrowing"), fine(0.0), late_days(0) {}

    // Methods
    void display_info() const {
        std::cout << "Phieu: " << record_id << " | Doc gia: " << reader_id 
                  << " | Sach: " << book_id << " | Ngay muon: " << borrow_date 
                  << " | Han tra: " << due_date << " | TT: " << status;
        if (!return_date.empty()) {
            std::cout << " | Ngay tra: " << return_date;
        }
        if (fine > 0) {
            std::cout << " | Phat: " << fine << " VND";
        }
        std::cout << "\n";
    }

    void mark_returned(const std::string& ret_date) {
        return_date = ret_date;
        status = "returned";
    }

    void calculate_late_days(const std::string& current_date) {
        // Tự viết hàm chuyển DD/MM/YYYY sang số ngày tuyệt đối để tính độ chênh lệch
    }

    void calculate_fine(double fine_per_day) {
        if (late_days > 0) {
            fine = late_days * fine_per_day;
        } else {
            fine = 0.0;
        }
    }

    bool is_overdue(const std::string& current_date) {
        // So sánh hạn trả với ngày hiện tại
        return false; 
    }

    std::string to_json_string() const {
        return "{\"record_id\":\"" + record_id + "\",\"book_id\":\"" + book_id + 
               "\",\"reader_id\":\"" + reader_id + "\",\"borrow_date\":\"" + borrow_date + 
               "\",\"due_date\":\"" + due_date + "\",\"return_date\":\"" + return_date + 
               "\",\"status\":\"" + status + "\",\"fine\":" + std::to_string(fine) + 
               ",\"late_days\":" + std::to_string(late_days) + "}";
    }

    void from_json_string(const std::string& json_str) {
        // Tự bóc tách chuỗi JSON
    }
};

#endif