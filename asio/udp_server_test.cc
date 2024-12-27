//
// Created by zouguowei on 2024/12/25.
//

#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

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
    std::cout << "Received: " << bytes_transferred << " bytes" << std::endl;
    std::cout << "Data: " << recv_buffer_.data() << std::endl;

    std::string message = "Got it!";
    socket_.async_send_to(
        boost::asio::buffer(message), remote_endpoint_,
        [this, message](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/) {
          StartReceive();
        });
  }

 private:
  udp::socket socket_;
  udp::endpoint remote_endpoint_;
  std::array<char, 1024> recv_buffer_;
};

int main(int argc, char *argv[]) {
  boost::asio::io_context io_context;
  UdpServer server(io_context, 12345);
  io_context.run();
  return 0;
}
