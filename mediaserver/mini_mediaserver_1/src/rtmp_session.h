#pragma once
#include <asio.hpp>
#include <memory>
#include <vector>
#include "flv_writer.h"

using asio::ip::tcp;

class RtmpSession : public std::enable_shared_from_this<RtmpSession> {
public:
    RtmpSession(tcp::socket socket);
    void start();

private:
    void do_handshake_c0c1();
    void do_handshake_s0s1s2();
    void do_handshake_c2();
    void send_protocol_messages();
    void read_chunk_header();
    void read_message_header(uint8_t fmt, uint32_t cs_id);
    void read_chunk_data();
    void process_complete_message();
    void handle_command();
    void send_connect_response(double trans_id);
    void send_create_stream_response(double trans_id);
    void send_simple_result(double trans_id);
    void send_publish_response();
    void send_rtmp_message(uint32_t cs_id, uint8_t msg_type, uint32_t msg_stream_id, const std::vector<uint8_t>& payload);
    void send_acknowledgement();

    tcp::socket socket_;
    uint8_t c0c1_[1537], s0s1s2_[3073], c2_[1536];
    uint8_t chunk_basic_header_, msg_header_buf_[11];
    std::vector<uint8_t> payload_buf_;
    uint32_t chunk_size_in_ = 128;
    uint32_t payload_bytes_read_ = 0;

    uint32_t cur_msg_len_ = 0, cur_msg_stream_ = 0, cur_msg_timestamp_ = 0;
    uint8_t cur_msg_type_ = 0;

    uint32_t bytes_received_ = 0;
    uint32_t ack_window_size_ = 5000000;
    uint32_t last_ack_sent_ = 0;

    std::unique_ptr<FlvWriter> flv_writer_;
};
