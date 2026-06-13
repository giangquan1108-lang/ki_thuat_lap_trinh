#ifndef READER_H
#define READER_H

#include <string>
#include <iostream>

class Reader {
public:
    std::string student_id;
    std::string full_name;
    std::string reader_type; // "student" hoặc "teacher"
    int borrowed_count;
    bool is_blocked;
    std::string phone;
    std::string email;

    // Constructors
    Reader() : student_id(""), full_name(""), reader_type("student"), 
               borrowed_count(0), is_blocked(false), phone(""), email("") {}
               
    Reader(std::string id, std::string name, std::string type, std::string ph, std::string em)
        : student_id(id), full_name(name), reader_type(type), 
          borrowed_count(0), is_blocked(false), phone(ph), email(em) {}

    // Methods
    void display_info() const {
        std::cout << "MSV/MSG: " << student_id << " | Ten: " << full_name 
                  << " | Loai: " << reader_type << " | Dang muon: " << borrowed_count 
                  << " | Trang thai: " << (is_blocked ? "Bi KHOA" : "Hoat dong") << "\n";
    }

    bool can_borrow(int limit) const {
        return !is_blocked && (borrowed_count < limit);
    }

    void increase_borrow_count() {
        borrowed_count++;
    }

    void decrease_borrow_count() {
        borrowed_count--;
        if (borrowed_count < 0) borrowed_count = 0;
    }

    void block_reader() {
        is_blocked = true;
    }

    void unblock_reader() {
        is_blocked = false;
    }

    std::string to_json_string() const {
        std::string blocked_str = is_blocked ? "true" : "false";
        return "{\"student_id\":\"" + student_id + "\",\"full_name\":\"" + full_name + 
               "\",\"reader_type\":\"" + reader_type + "\",\"borrowed_count\":" + std::to_string(borrowed_count) + 
               ",\"is_blocked\":" + blocked_str + ",\"phone\":\"" + phone + "\",\"email\":\"" + email + "\"}";
    }

    void from_json_string(const std::string& json_str) {
        // Tự viết hàm bóc tách chuỗi JSON thô
    }
};

#endif