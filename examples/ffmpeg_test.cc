#include <iostream>

extern "C" {
#include <libavformat/avformat.h>
}

int main(int argc, char *argv[]) {
  std::cout << avformat_version() << std::endl;
  std::cout << avformat_configuration() << std::endl;
  return 0;
}