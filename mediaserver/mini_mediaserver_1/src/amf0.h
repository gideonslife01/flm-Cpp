#pragma once
#include <vector>
#include <string>
#include <cstdint>

class Amf0 {
public:
    static std::string parse_string(const std::vector<uint8_t>& buf, size_t& pos);
    static double parse_number(const std::vector<uint8_t>& buf, size_t& pos);
    static void parse_null(const std::vector<uint8_t>& buf, size_t& pos);
    static void parse_object(const std::vector<uint8_t>& buf, size_t& pos);

    static void write_string(std::vector<uint8_t>& buf, const std::string& str);
    static void write_number(std::vector<uint8_t>& buf, double val);
    static void write_null(std::vector<uint8_t>& buf);
    static void write_undefined(std::vector<uint8_t>& buf);
    static void write_object_start(std::vector<uint8_t>& buf);
    static void write_object_end(std::vector<uint8_t>& buf);
    static void write_object_property(std::vector<uint8_t>& buf, const std::string& key, const std::string& val);
    static void write_object_property(std::vector<uint8_t>& buf, const std::string& key, double val);
};
