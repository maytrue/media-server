#include <iconv.h>
#include <iostream>

namespace mediaserver {

std::string ConvertGB2312ToUtf8(const std::string& in) {
  char* in_buf = (char*)in.data();
  size_t in_len = in.length();

  char out_buffer[1024];
  size_t out_len = 1024;
  memset(out_buffer, 0, 1024);
  char* out_buf = (char*)out_buffer;

  iconv_t conv = iconv_open("UTF-8", "GBK");
  if (conv) {
    int ret = iconv(conv, &in_buf, &in_len, &out_buf, &out_len);
    std::cout << "iconv ret:" << ret << std::endl;
    iconv_close(conv);
  }
  return std::string(out_buffer);
}

}  // namespace mediaserver