#include "flv_writer.h"
#include <iostream>

FlvWriter::FlvWriter(const std::string& filename) {
    file_.open(filename, std::ios::binary);
    std::cout << "[FLV] " << filename << " opened\n";
}

FlvWriter::~FlvWriter() {
    if (file_.is_open()) {
        file_.close();
        std::cout << "[FLV] closed\n";
    }
}

void FlvWriter::write_be24(std::ofstream& f, uint32_t val) {
    f.put((val >> 16) & 0xFF);
    f.put((val >> 8) & 0xFF);
    f.put(val & 0xFF);
}

void FlvWriter::write_be32(std::ofstream& f, uint32_t val) {
    f.put((val >> 24) & 0xFF);
    f.put((val >> 16) & 0xFF);
    f.put((val >> 8) & 0xFF);
    f.put(val & 0xFF);
}

void FlvWriter::write_header() {
    if (header_written_) return;
    file_.put('F'); file_.put('L'); file_.put('V');
    file_.put(0x01);
    file_.put(0x05); // Audio + Video
    write_be32(file_, 9);
    write_be32(file_, 0);
    header_written_ = true;
    std::cout << "[FLV] Header written\n";
}

void FlvWriter::write_tag(uint8_t type, uint32_t timestamp, const std::vector<uint8_t>& data) {
    write_header();
    file_.put(type);
    write_be24(file_, data.size());
    write_be24(file_, timestamp);
    file_.put((timestamp >> 24) & 0xFF);
    write_be24(file_, 0);
    file_.write((char*)data.data(), data.size());
    write_be32(file_, data.size() + 11);
}
