//
// Created by zouguowei on 2024/12/25.
//

#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>

#include "modules/rtp_rtcp/source/rtp_packet_received.h"
#include "util/spdlog_intializer.h"

#include "room_manager.h"
#include "http_server.h"
#include "udp_server.h"

using boost::asio::ip::udp;
using boost::asio::ip::tcp;

namespace beast = boost::beast;
namespace http = beast::http;

using namespace webrtc;
using namespace nlohmann;
using namespace rtcserver;

int main(int argc, char *argv[]) {
  rtcserver::SpdlogInitializer::Init();

  auto room_manager = std::make_shared<RoomManager>();
  boost::asio::io_context io_context;
  UdpServer server(io_context, 12345);
  HttpServer http_server(io_context, 8080, room_manager);
  io_context.run();
  return 0;
}
