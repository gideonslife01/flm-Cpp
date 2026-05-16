#include "rtmp_session.h"
#include "amf0.h"
#include <openssl/rand.h>
#include <iostream>
#include <cstring>

RtmpSession::RtmpSession(tcp::socket socket) : socket_(std::move(socket)) {
    flv_writer_ = std::make_unique<FlvWriter>("test.flv");
}

void RtmpSession::start() {
    do_handshake_c0c1();
}

void RtmpSession::do_handshake_c0c1() {
    auto self(shared_from_this());
    asio::async_read(socket_, asio::buffer(c0c1_, 1537),
        [this, self](std::error_code ec, std::size_t) {
            if (ec || c0c1_[0]!= 0x03) return;
            std::cout << "[Handshake] C0C1 received\n";
            do_handshake_s0s1s2();
        });
}

void RtmpSession::do_handshake_s0s1s2() {
    auto self(shared_from_this());
    s0s1s2_[0] = 0x03;
    uint32_t time = htonl(static_cast<uint32_t>(::time(nullptr)));
    std::memcpy(&s0s1s2_[1], &time, 4);
    std::memset(&s0s1s2_[5], 0, 4);
    RAND_bytes(&s0s1s2_[9], 1528);
    std::memcpy(&s0s1s2_[1537], &c0c1_[1], 1536);
    asio::async_write(socket_, asio::buffer(s0s1s2_, 3073),
        [this, self](std::error_code ec, std::size_t) {
            if (ec) return;
            std::cout << "[Handshake] S0S1S2 sent\n";
            do_handshake_c2();
        });
}

void RtmpSession::do_handshake_c2() {
    auto self(shared_from_this());
    asio::async_read(socket_, asio::buffer(c2_, 1536),
        [this, self](std::error_code ec, std::size_t) {
            if (ec) return;
            std::cout << "[Handshake] C2 received, Handshake Complete!\n";
            send_protocol_messages();
        });
}

void RtmpSession::send_protocol_messages() {
    auto self(shared_from_this());
    std::vector<uint8_t> pkt;
    pkt.insert(pkt.end(), {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x05, 0x00, 0x00, 0x00, 0x00});
    uint32_t ack = htonl(5000000);
    pkt.insert(pkt.end(), (uint8_t*)&ack, (uint8_t*)&ack + 4);
    pkt.insert(pkt.end(), {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x06, 0x00, 0x00, 0x00, 0x00});
    uint32_t bw = htonl(5000000);
    pkt.insert(pkt.end(), (uint8_t*)&bw, (uint8_t*)&bw + 4);
    pkt.push_back(0x02);
    pkt.insert(pkt.end(), {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00});
    uint32_t cs = htonl(4096);
    pkt.insert(pkt.end(), (uint8_t*)&cs, (uint8_t*)&cs + 4);
    asio::async_write(socket_, asio::buffer(pkt),
        [this, self](std::error_code ec, std::size_t) {
            if (ec) return;
            std::cout << "[Server] Protocol messages sent\n";
            read_chunk_header();
        });
}

void RtmpSession::read_chunk_header() {
    auto self(shared_from_this());
    socket_.async_read_some(asio::buffer(&chunk_basic_header_, 1),
        [this, self](std::error_code ec, std::size_t) {
            if (ec) {
                std::cout << "[Info] Connection closed: " << ec.message() << "\n";
                return;
            }
            uint8_t fmt = (chunk_basic_header_ >> 6) & 0x03;
            uint32_t cs_id = chunk_basic_header_ & 0x3F;
            if (cs_id == 0) {
                uint8_t ext;
                asio::async_read(socket_, asio::buffer(&ext, 1),
                    [this, self, fmt, ext](std::error_code ec, std::size_t) {
                        if (ec) return;
                        read_message_header(fmt, 64 + ext);
                    });
            } else if (cs_id == 1) {
                uint8_t buf[2];
                asio::async_read(socket_, asio::buffer(buf, 2),
                    [this, self, fmt, buf](std::error_code ec, std::size_t) {
                        if (ec) return;
                        read_message_header(fmt, 64 + buf[0] + (buf[1] << 8));
                    });
            } else {
                read_message_header(fmt, cs_id);
            }
        });
}

void RtmpSession::read_message_header(uint8_t fmt, uint32_t cs_id) {
    auto self(shared_from_this());
    size_t header_size = (fmt == 0)? 11 : (fmt == 1)? 7 : (fmt == 2)? 3 : 0;
    if (header_size == 0) {
        read_chunk_data();
        return;
    }
    asio::async_read(socket_, asio::buffer(msg_header_buf_, header_size),
        [this, self, fmt, cs_id, header_size](std::error_code ec, std::size_t) {
            if (ec) return;

            uint32_t timestamp = 0;
            if (fmt <= 2) {
                timestamp = (msg_header_buf_[0] << 16) |
                            (msg_header_buf_[1] << 8) |
                            msg_header_buf_[2];
            }

            if (fmt == 0) {
                cur_msg_timestamp_ = timestamp;
                if (timestamp == 0xFFFFFF && header_size >= 11) {
                    cur_msg_timestamp_ = (msg_header_buf_[7] << 24) |
                                         (msg_header_buf_[8] << 16) |
                                         (msg_header_buf_[9] << 8) |
                                         msg_header_buf_[10];
                }
                cur_msg_len_ = (msg_header_buf_[3] << 16) | (msg_header_buf_[4] << 8) | msg_header_buf_[5];
                cur_msg_type_ = msg_header_buf_[6];
                cur_msg_stream_ = msg_header_buf_[7] | (msg_header_buf_[8] << 8) |
                                  (msg_header_buf_[9] << 16) | (msg_header_buf_[10] << 24);
                payload_buf_.clear();
                payload_bytes_read_ = 0;
            } else if (fmt == 1) {
                cur_msg_timestamp_ += timestamp;
                cur_msg_len_ = (msg_header_buf_[3] << 16) | (msg_header_buf_[4] << 8) | msg_header_buf_[5];
                cur_msg_type_ = msg_header_buf_[6];
                payload_buf_.clear();
                payload_bytes_read_ = 0;
            } else if (fmt == 2) {
                cur_msg_timestamp_ += timestamp;
            }

            read_chunk_data();
        });
}

void RtmpSession::read_chunk_data() {
    auto self(shared_from_this());
    uint32_t remaining = cur_msg_len_ - payload_bytes_read_;
    uint32_t to_read = std::min(remaining, chunk_size_in_);

    if (to_read == 0) {
        process_complete_message();
        read_chunk_header();
        return;
    }

    size_t offset = payload_buf_.size();
    payload_buf_.resize(offset + to_read);
    asio::async_read(socket_, asio::buffer(payload_buf_.data() + offset, to_read),
        [this, self, to_read](std::error_code ec, std::size_t bytes_read) {
            if (ec) {
                std::cout << "[Warn] Read error: " << ec.message() << "\n";
                return;
            }
            payload_bytes_read_ += to_read;
            bytes_received_ += bytes_read; // 누적

            // 5MB마다 ACK 보내기
            if (bytes_received_ - last_ack_sent_ >= ack_window_size_) {
                send_acknowledgement();
            }

            if (payload_bytes_read_ >= cur_msg_len_) {
                process_complete_message();
            }
            read_chunk_header();
        });
}

void RtmpSession::process_complete_message() {
    if (cur_msg_type_ == 1) {
        chunk_size_in_ = (payload_buf_[0] << 24) | (payload_buf_[1] << 16) |
                         (payload_buf_[2] << 8) | payload_buf_[3];
        ack_window_size_ = chunk_size_in_;
        std::cout << "[Chunk] Client Set Chunk Size: " << chunk_size_in_ << "\n";
    } else if (cur_msg_type_ == 4) { // 이 부분 추가
        uint16_t event_type = (payload_buf_[0] << 8) | payload_buf_[1];
        if (event_type == 6) { // PingRequest
            std::vector<uint8_t> pong;
            pong.push_back(0x02);
            pong.insert(pong.end(), {0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x04});
            pong.insert(pong.end(), {0x00, 0x00, 0x00, 0x00});
            pong.insert(pong.end(), {0x00, 0x07}); // Pong
            pong.insert(pong.end(), payload_buf_.begin() + 2, payload_buf_.begin() + 6);
            asio::write(socket_, asio::buffer(pong));
            std::cout << "[Server] Pong sent\n";
        }
    } else if (cur_msg_type_ == 20) {
        handle_command();
    } else if (cur_msg_type_ == 8) {
        if (payload_buf_.size() >= 1 && payload_buf_[0] >> 4 == 10) {
            flv_writer_->write_tag(0x08, cur_msg_timestamp_, payload_buf_);
            std::cout << "[Data] Audio ts=" << cur_msg_timestamp_ << " " << payload_buf_.size() << " bytes\n";
        }
    } else if (cur_msg_type_ == 9) {
        if (payload_buf_.size() >= 1 && (payload_buf_[0] & 0x0F) == 7) {
            flv_writer_->write_tag(0x09, cur_msg_timestamp_, payload_buf_);
            //... 로그
        }
    }
}

void RtmpSession::handle_command() {
    size_t pos = 0;
    std::string cmd = Amf0::parse_string(payload_buf_, pos);
    double trans_id = Amf0::parse_number(payload_buf_, pos);
    std::cout << " Command: " << cmd << " tid: " << trans_id << "\n";

    if (cmd == "connect") {
        Amf0::parse_object(payload_buf_, pos);
        send_connect_response(trans_id);
    } else if (cmd == "releaseStream") {
        Amf0::parse_null(payload_buf_, pos);
        Amf0::parse_string(payload_buf_, pos);
        send_simple_result(trans_id);
    } else if (cmd == "FCPublish") {
        Amf0::parse_null(payload_buf_, pos);
        Amf0::parse_string(payload_buf_, pos);
        send_simple_result(trans_id);
    } else if (cmd == "createStream") {
        Amf0::parse_null(payload_buf_, pos);
        send_create_stream_response(trans_id);
    } else if (cmd == "publish") {
        Amf0::parse_null(payload_buf_, pos);
        std::string key = Amf0::parse_string(payload_buf_, pos);
        std::string type = Amf0::parse_string(payload_buf_, pos);
        std::cout << " Publish key: " << key << " type: " << type << "\n";
        send_publish_response();
    } else if (cmd == "FCUnpublish" || cmd == "deleteStream") {
        send_simple_result(trans_id);
    }
}

void RtmpSession::send_connect_response(double trans_id) {
    std::vector<uint8_t> amf;
    Amf0::write_string(amf, "_result");
    Amf0::write_number(amf, trans_id);
    Amf0::write_object_start(amf);
    Amf0::write_object_property(amf, "fmsVer", "FMS/5,0,1,1");
    Amf0::write_object_property(amf, "capabilities", 31.0);
    Amf0::write_object_end(amf);
    Amf0::write_object_start(amf);
    Amf0::write_object_property(amf, "level", "status");
    Amf0::write_object_property(amf, "code", "NetConnection.Connect.Success");
    Amf0::write_object_property(amf, "description", "Connection succeeded.");
    Amf0::write_object_end(amf);
    send_rtmp_message(3, 20, 0, amf);
    std::cout << "[Server] Connect Success sent\n";
}

void RtmpSession::send_create_stream_response(double trans_id) {
    std::vector<uint8_t> amf;
    Amf0::write_string(amf, "_result");
    Amf0::write_number(amf, trans_id);
    Amf0::write_null(amf);
    Amf0::write_number(amf, 1.0);
    send_rtmp_message(3, 20, 0, amf);
    std::cout << "[Server] CreateStream Success sent\n";
}

void RtmpSession::send_simple_result(double trans_id) {
    std::vector<uint8_t> amf;
    Amf0::write_string(amf, "_result");
    Amf0::write_number(amf, trans_id);
    Amf0::write_null(amf);
    Amf0::write_undefined(amf);
    send_rtmp_message(3, 20, 0, amf);
}

void RtmpSession::send_publish_response() {
    std::vector<uint8_t> stream_begin;
    stream_begin.push_back(0x02);
    stream_begin.insert(stream_begin.end(), {0x00, 0x00, 0x00});
    stream_begin.insert(stream_begin.end(), {0x00, 0x00, 0x06});
    stream_begin.push_back(0x04);
    stream_begin.insert(stream_begin.end(), {0x00, 0x00, 0x00, 0x00});
    stream_begin.insert(stream_begin.end(), {0x00, 0x00});
    stream_begin.insert(stream_begin.end(), {0x00, 0x00, 0x00, 0x01});
    asio::write(socket_, asio::buffer(stream_begin));
    std::cout << "[Server] Stream Begin sent\n";

    std::vector<uint8_t> amf;
    Amf0::write_string(amf, "onStatus");
    Amf0::write_number(amf, 0.0);
    Amf0::write_null(amf);
    Amf0::write_object_start(amf);
    Amf0::write_object_property(amf, "level", "status");
    Amf0::write_object_property(amf, "code", "NetStream.Publish.Start");
    Amf0::write_object_property(amf, "description", "Start publishing");
    Amf0::write_object_end(amf);
    send_rtmp_message(5, 20, 1, amf);
    std::cout << "[Server] Publish Start sent - 방송 시작!\n";
}

void RtmpSession::send_rtmp_message(uint32_t cs_id, uint8_t msg_type, uint32_t msg_stream_id, const std::vector<uint8_t>& payload) {
    std::vector<uint8_t> pkt;
    pkt.push_back(cs_id);
    pkt.insert(pkt.end(), {0x00, 0x00, 0x00});
    uint32_t len = payload.size();
    pkt.push_back((len >> 16) & 0xFF);
    pkt.push_back((len >> 8) & 0xFF);
    pkt.push_back(len & 0xFF);
    pkt.push_back(msg_type);
    pkt.push_back(msg_stream_id & 0xFF);
    pkt.push_back((msg_stream_id >> 8) & 0xFF);
    pkt.push_back((msg_stream_id >> 16) & 0xFF);
    pkt.push_back((msg_stream_id >> 24) & 0xFF);
    pkt.insert(pkt.end(), payload.begin(), payload.end());
    asio::write(socket_, asio::buffer(pkt));
}

// obs에 받응 용량 정보 전송
void RtmpSession::send_acknowledgement() {
    std::vector<uint8_t> pkt;
    pkt.push_back(0x02); // chunk stream id
    pkt.insert(pkt.end(), {0x00, 0x00, 0x00}); // timestamp
    pkt.insert(pkt.end(), {0x00, 0x00, 0x04}); // msg length = 4
    pkt.push_back(0x03); // type = Acknowledgement
    pkt.insert(pkt.end(), {0x00, 0x00, 0x00, 0x00}); // stream id

    uint32_t seq = htonl(bytes_received_);
    pkt.insert(pkt.end(), (uint8_t*)&seq, (uint8_t*)&seq + 4);

    asio::write(socket_, asio::buffer(pkt));
    last_ack_sent_ = bytes_received_;
    std::cout << "[Server] Ack sent: " << bytes_received_ << " bytes\n";
}
