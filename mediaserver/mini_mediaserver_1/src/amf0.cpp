#include "amf0.h"
#include <cstring>

std::string Amf0::parse_string(const std::vector<uint8_t>& buf, size_t& pos) {
    if (pos >= buf.size() || buf[pos]!= 0x02) return "";
    pos++;
    if (pos + 1 >= buf.size()) return "";
    uint16_t len = (buf[pos] << 8) | buf[pos+1];
    pos += 2;
    if (pos + len > buf.size()) return "";
    std::string str(buf.begin() + pos, buf.begin() + pos + len);
    pos += len;
    return str;
}

double Amf0::parse_number(const std::vector<uint8_t>& buf, size_t& pos) {
    if (pos >= buf.size() || buf[pos]!= 0x00) return 0;
    pos++;
    uint64_t val = 0;
    for (int i = 0; i < 8; i++) val = (val << 8) | buf[pos++];
    return *reinterpret_cast<double*>(&val);
}

void Amf0::parse_null(const std::vector<uint8_t>& buf, size_t& pos) {
    if (pos < buf.size() && buf[pos]==0x05) pos++;
}

void Amf0::parse_object(const std::vector<uint8_t>& buf, size_t& pos) {
    if (pos >= buf.size() || buf[pos]!= 0x03) return;
    pos++;
    while (pos + 2 < buf.size()) {
        uint16_t len = (buf[pos] << 8) | buf[pos+1];
        pos += 2;
        if (len == 0 && buf[pos] == 0x09) { pos++; break; }
        pos += len;
        if (buf[pos] == 0x02) { size_t tmp=pos; parse_string(buf, tmp); pos=tmp; }
        else if (buf[pos] == 0x00) { pos += 9; }
        else pos++;
    }
}

void Amf0::write_string(std::vector<uint8_t>& buf, const std::string& str) {
    buf.push_back(0x02);
    uint16_t len = str.size();
    buf.push_back((len >> 8) & 0xFF);
    buf.push_back(len & 0xFF);
    buf.insert(buf.end(), str.begin(), str.end());
}

void Amf0::write_number(std::vector<uint8_t>& buf, double val) {
    buf.push_back(0x00);
    uint64_t* p = (uint64_t*)&val;
    for (int i = 7; i >= 0; i--) buf.push_back((*p >> (i*8)) & 0xFF);
}

void Amf0::write_null(std::vector<uint8_t>& buf) { buf.push_back(0x05); }
void Amf0::write_undefined(std::vector<uint8_t>& buf) { buf.push_back(0x06); }
void Amf0::write_object_start(std::vector<uint8_t>& buf) { buf.push_back(0x03); }
void Amf0::write_object_end(std::vector<uint8_t>& buf) { buf.insert(buf.end(), {0x00, 0x00, 0x09}); }

void Amf0::write_object_property(std::vector<uint8_t>& buf, const std::string& key, const std::string& val) {
    uint16_t len = key.size();
    buf.push_back((len >> 8) & 0xFF);
    buf.push_back(len & 0xFF);
    buf.insert(buf.end(), key.begin(), key.end());
    write_string(buf, val);
}

void Amf0::write_object_property(std::vector<uint8_t>& buf, const std::string& key, double val) {
    uint16_t len = key.size();
    buf.push_back((len >> 8) & 0xFF);
    buf.push_back(len & 0xFF);
    buf.insert(buf.end(), key.begin(), key.end());
    write_number(buf, val);
}
