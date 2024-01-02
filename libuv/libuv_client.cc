#include <iostream>

#include "libuv_thread.h"
#include "util/spdlog_intializer.h"

class UdpThreadListenerImpl : public UdpThread::UdpThreadListener {
 public:
  void OnDataRecv(char* data, int len) override {
    LOG_INFO("[UdpThreadListenerImpl] OnDataRecv data:{}", data);
  }
};

int main(int argc, char const* argv[]) {
  rtcserver::SpdlogInitializer::Init();

  auto udp_client_listener = std::make_shared<UdpThreadListenerImpl>();
  auto udp_client =
      std::make_shared<UdpThread>(12345, udp_client_listener.get());

  udp_client->Start();

  std::string quit;
  do {
    std::cin >> quit;
    if (!quit.compare("quit")) {
      break;
    }

    if (!quit.compare("send")) {
      std::string message = "hello webrtc";
      udp_client->SendTo("127.0.0.1", 12346, (char*)message.c_str(),
                         message.length());
    }
  } while (true);

  udp_client->Close();

  return 0;
}
