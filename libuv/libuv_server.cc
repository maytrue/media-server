#include <iostream>

#include "libuv_thread.h"
#include "util/spdlog_intializer.h"

class UdpThreadListenerImpl : public UdpThread::UdpThreadListener {
 public:
  void OnDataRecv(char* data, int len) override {
    LOG_INFO("[UdpThreadListenerImpl] OnDataRecv data:{} len:{}", data, len);
  }
};

int main(int argc, char const* argv[]) {
  rtcserver::SpdlogInitializer::Init();

  auto udp_listener = std::make_shared<UdpThreadListenerImpl>();
  auto udp_server = std::make_shared<UdpThread>(12346, udp_listener.get());

  udp_server->Start();

  std::string quit;
  do {
    std::cin >> quit;
    if (!quit.compare("quit")) {
      break;
    }

    if (!quit.compare("send")) {
      std::string message = "hello webrtc";
      udp_server->SendTo("127.0.0.1", 12345, (char*)message.c_str(),
                         message.length());
    }
  } while (true);

  udp_server->Close();

  return 0;
}
