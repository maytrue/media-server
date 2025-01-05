//
// Created by zouguowei on 2024/12/25.
//

#include <iostream>
#include <boost/asio.hpp>

#include "modules/rtp_rtcp/source/rtp_packet_received.h"
#include "util/spdlog_intializer.h"

using boost::asio::ip::udp;
using namespace webrtc;

class UdpServer {
 public:
  UdpServer(boost::asio::io_context &io_context, short port)
      : socket_(io_context, udp::endpoint(udp::v4(), port)) {
    StartReceive();
  }

 private:
  void StartReceive() {
    socket_.async_receive_from(
        boost::asio::buffer(recv_buffer_), remote_endpoint_,
        [this](const boost::system::error_code &error, std::size_t bytes_transferred) {
          if (!error && bytes_transferred > 0) {
            HandleReceive(bytes_transferred);
          } else {
            StartReceive();
          }
        });
  }

  void HandleReceive(std::size_t bytes_transferred) {
    LOG_INFO("[UdpServer] HandleReceive Received:{} bytes", bytes_transferred);
    RtpPacketReceived packet_received;
    bool ret = packet_received.Parse((const uint8_t*)recv_buffer_.data(), bytes_transferred);
    if (ret) {
      LOG_INFO("[UdpServer] HandleReceive PayloadType:{} seqNo:{} Ssrc:{}",
               packet_received.PayloadType(),
               packet_received.SequenceNumber(),
               packet_received.Ssrc());
    }

    StartReceive();
    // std::string message = "Got it!";
    // socket_.async_send_to(
    //     boost::asio::buffer(message), remote_endpoint_,
    //     [this, message](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/) {
    //       StartReceive();
    //     });
  }

 private:
  udp::socket socket_;
  udp::endpoint remote_endpoint_;
  std::array<char, 2048> recv_buffer_;
};

int main(int argc, char *argv[]) {
  rtcserver::SpdlogInitializer::Init();
  boost::asio::io_context io_context;
  UdpServer server(io_context, 12345);
  io_context.run();
  return 0;
}
