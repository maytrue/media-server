//
// Created by zouguowei on 2025/3/28.
//

#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include "room_manager.h"

using boost::asio::ip::udp;
using boost::asio::ip::tcp;

namespace beast = boost::beast;
namespace http = beast::http;

namespace rtcserver {

using namespace nlohmann;

class HttpSession : public std::enable_shared_from_this<HttpSession> {
 public:
  HttpSession(tcp::socket socket, const std::shared_ptr<RoomManager> &room_manager)
      : socket_(std::move(socket)),
        room_manager_(room_manager) {
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
    response_.result(http::status::ok);
    response_.set(http::field::content_type, "application/json");

    if (request_.method() == http::verb::get && request_.target() == "/api/rooms") {
      std::vector<std::shared_ptr<Room>> rooms = room_manager_->GetRooms();
      json result;
      json json_array = json::array();

      for (auto& room : rooms) {
        json_array.push_back(room->room_id());
      }

      result["result"] = 0;
      result["rooms"] = json_array;
      response_.body() = result.dump();
    } else if (request_.method() == http::verb::post && request_.target() == "/api/room") {
      json body = json::parse(request_.body());
      uint64_t rid = body["rid"];


      json result;
      result["result"] = 0;
      response_.body() = result.dump();
    } else {
      response_.result(http::status::not_found);
      json result;
      result["result"] = -1;
      response_.body() = result.dump();
    }

    response_.version(request_.version());
    response_.keep_alive(false);
    // response_.set(http::field::content_type, "text/plain");
    // response_.body() = "Hello, World!";
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
  std::shared_ptr<RoomManager> room_manager_;
};

class HttpServer {
public:
  HttpServer(boost::asio::io_context &io_context, short port, const std::shared_ptr<RoomManager> &room_manager)
      : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
        room_manager_(room_manager) {
    StartAccept();
  }

private:
  void StartAccept() {
    LOG_INFO("[HttpServer] StartAccept this:{}", fmt::ptr(this));
    acceptor_.async_accept(
        [this](beast::error_code ec, tcp::socket socket) {
          if (!ec) {
            std::make_shared<HttpSession>(std::move(socket), room_manager_)->Start();
          }
          StartAccept();
        });
  }

private:
  tcp::acceptor acceptor_;
  std::shared_ptr<RoomManager> room_manager_;
};

}
