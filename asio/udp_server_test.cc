//
// Created by zouguowei on 2024/12/25.
//

#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include "modules/rtp_rtcp/source/rtp_packet_received.h"
#include "util/spdlog_intializer.h"

using boost::asio::ip::udp;
using boost::asio::ip::tcp;
using namespace webrtc;

namespace beast = boost::beast;
namespace http = beast::http;

class Participant;
class Room;
class RoomManager;

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

class HttpSession : public std::enable_shared_from_this<HttpSession> {
 public:
  HttpSession(tcp::socket socket) : socket_(std::move(socket)) {
    LOG_INFO("[HttpSession] HttpSession this:{}", fmt::ptr(this));
  }

  virtual ~HttpSession() {
    LOG_INFO("[HttpSession] ~HttpSession this:{}", fmt::ptr(this));
  }

  void Start() {
    LOG_INFO("[HttpSession] HttpSession Start this:{}", fmt::ptr(this));
    DoRead();
  }

 private:
  void DoRead() {
    auto self = shared_from_this();
    http::async_read(socket_, buffer_, request_, [this, self](boost::system::error_code ec, std::size_t length) {
      if (!ec) {
        HandleRequest();
      }
    });
  }

  void HandleRequest() {
    response_.version(request_.version());
    response_.keep_alive(false);
    response_.set(http::field::content_type, "text/plain");
    response_.body() = "Hello, World!";
    response_.prepare_payload();

    auto self = shared_from_this();
    http::async_write(socket_, response_,
                             [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                                self->socket_.shutdown(tcp::socket::shutdown_send, ec);
                             });
  }

 private:
  tcp::socket socket_;
  beast::flat_buffer buffer_;
  http::request<http::string_body> request_;
  http::response<http::string_body> response_;
};

class HttpServer {
 public:
  HttpServer(boost::asio::io_context &io_context, short port)
      : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
    StartAccept();
  }

 private:
  void StartAccept() {
    LOG_INFO("[HttpServer] StartAccept this:{}", fmt::ptr(this));
    // tcp::socket socket(acceptor_.get_executor());
    // acceptor_.async_accept(socket, [this, &socket](const boost::system::error_code &error) {
    //   if (!error) {
    //     std::make_shared<HttpSession>(std::move(socket))->Start();
    //   }
    //   StartAccept();
    // });

    acceptor_.async_accept(
        [this](beast::error_code ec, tcp::socket socket) {
          if (!ec) {
            std::make_shared<HttpSession>(std::move(socket))->Start();
          }
          StartAccept();
        });
  }

 private:
  tcp::acceptor acceptor_;
};

class RoomManager {
 public:
  RoomManager() = default;
  ~RoomManager() = default;

  void AddRoom(uint64_t rid) {
    rooms_.insert(std::make_pair(rid, std::make_shared<Room>(rid)));
  }

  std::shared_ptr<Room> GetRoom(uint64_t rid) {
    auto it = rooms_.find(rid);
    if (it != rooms_.end()) {
      return it->second;
    }
    return nullptr;
  }

 private:
  std::unordered_map<uint64_t, std::shared_ptr<Room>> rooms_;
};

class Room {
 public:
  explicit Room(uint64_t room_id) : room_id_(room_id) {

  }

  ~Room() = default;

  void AddParticipant(uint64_t pid, uint32_t audio_ssrc, uint32_t video_ssrc) {
    participants_.emplace_back(pid, audio_ssrc, video_ssrc);
  }

 private:
  std::vector<Participant> participants_;
  uint64_t room_id_ = 0;
};


class Participant {
 public:
  Participant(uint64_t pid, uint32_t audio_ssrc, uint32_t video_ssrc)
      : pid_(pid), audio_ssrc_(audio_ssrc), video_ssrc_(video_ssrc) {
  }

  ~Participant() = default;


 private:
  uint64_t pid_ = 0;
  uint32_t audio_ssrc_ = 0;
  uint32_t video_ssrc_ = 0;
};

int main(int argc, char *argv[]) {
  rtcserver::SpdlogInitializer::Init();

  auto room_manager = std::make_shared<RoomManager>();
  boost::asio::io_context io_context;
  UdpServer server(io_context, 12345);
  HttpServer http_server(io_context, 8080);
  io_context.run();
  return 0;
}
