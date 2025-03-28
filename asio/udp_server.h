//
// Created by zouguowei on 2025/3/28.
//

#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "modules/rtp_rtcp/source/rtp_packet_received.h"

namespace rtcserver {

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
    webrtc::RtpPacketReceived packet_received;
    bool ret = packet_received.Parse((const uint8_t *) recv_buffer_.data(), bytes_transferred);
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


}