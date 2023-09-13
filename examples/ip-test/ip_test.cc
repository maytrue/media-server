#include <locale.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstring>
#include <iostream>
#include <iconv.h>

#include "util/IpQuery_QQWry.hpp"
#include "util/iconv_util.h"

// Usage: ./ip-test /home/ubuntu/code/media-server/data/qqwry_lastest.dat 36.112.207.26|8.8.8.8
int main(int argc, char* argv[]) {
  if (argc != 3) {
    return -1;
  }

  std::string dat_file = argv[1];
  std::string ip_addr = argv[2];

  IpQuery_QQWry ip_query(dat_file);
  auto [info0, info1, desp] = ip_query.find_info(ip_addr);

  std::cout << mediaserver::ConvertGB2312ToUtf8(info0) << std::endl;
  std::cout << mediaserver::ConvertGB2312ToUtf8(desp) << std::endl;

  return 0;
}
