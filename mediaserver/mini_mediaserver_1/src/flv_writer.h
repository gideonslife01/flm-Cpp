#pragma once
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>

class FlvWriter {
public:
    FlvWriter(const std::string& filename);
    ~FlvWriter();
    void write_tag(uint8_t type, uint32_t timestamp, const std::vector<uint8_t>& data);

private:
    void write_header();
    void write_be24(std::ofstream& f, uint32_t val);
    void write_be32(std::ofstream& f, uint32_t val);

    std::ofstream file_;
    bool header_written_ = false;
};
