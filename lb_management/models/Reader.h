#ifndef READER_H
#define READER_H

#include <string>
#include <iostream>

class Reader {
public:
    std::string student_id;
    std::string full_name;
    std::string reader_type; // phân loại: "student" hoặc "teacher"
    int borrowed_count;
    bool is_blocked;
    std::string phone;
    std::string email;

    // Khởi tạo bạn đọc rỗng
    Reader() : student_id(""), full_name(""), reader_type("student"), 
               borrowed_count(0), is_blocked(false), phone(""), email("") {}
    // Khởi tạo bạn đọc với đầy đủ tham số           
    Reader(std::string id, std::string name, std::string type, std::string ph, std::string em)
        : student_id(id), full_name(name), reader_type(type), 
          borrowed_count(0), is_blocked(false), phone(ph), email(em) {}

    // In thông tin bạn đọc ra màn hình
    void display_info() const {
        std::cout << "MSV/MSG: " << student_id << " | Ten: " << full_name 
                  << " | Loai: " << reader_type << " | Dang muon: " << borrowed_count 
                  << " | Trang thai: " << (is_blocked ? "Bi KHOA" : "Hoat dong") << "\n";
    }

    // Còn slot mượn ko?
    bool can_borrow(int limit) const {
        return !is_blocked && (borrowed_count < limit);
    }

    // Tăng số sách đang mượn lên 1
    void increase_borrow_count() {
        borrowed_count++;
    }

    // Giảm số sách đang mượn đi 1, ko cho âm
    void decrease_borrow_count() {
        borrowed_count--;
        if (borrowed_count < 0) borrowed_count = 0;
    }

    // Khóa thẻ bạn đọc
    void block_reader() {
        is_blocked = true;
    }

    // Mở khóa thẻ
    void unblock_reader() {
        is_blocked = false;
    }

    // Xuất ra chuỗi JSON thô
    std::string to_json_string() const {
        std::string blocked_str = is_blocked ? "true" : "false";
        return "{\"student_id\":\"" + student_id + "\",\"full_name\":\"" + full_name + 
               "\",\"reader_type\":\"" + reader_type + "\",\"borrowed_count\":" + std::to_string(borrowed_count) + 
               ",\"is_blocked\":" + blocked_str + ",\"phone\":\"" + phone + "\",\"email\":\"" + email + "\"}";
    }

    // Parse JSON ngược về object (tự cắt chuỗi thủ công)
    void from_json_string(const std::string& json_str) {
        // Bóc từng field giữa mấy cặp ngoặc kép
    }
};

#endif