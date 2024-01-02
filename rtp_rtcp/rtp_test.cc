#include "rtp_rtcp/rtp.h"

#include <iostream>

#include "rtp_rtcp/rtp.h"
#include "util/rtc_server_log.h"

int main(int argc, char* argv[]) {
  rtcserver::RtcServerLog::Init();
  if (argc != 2) {
    LOG_ERROR("please input rtp file");
    return 0;
  }

  char* file_name = argv[1];
  FILE* fp = fopen(file_name, "rb");
  if (!fp) {
    LOG_ERROR("file:{} open error", file_name);
    return 0;
  }

  fseek(fp, 0, SEEK_END);
  size_t file_size = ftell(fp);
  rewind(fp);

  auto packet_parser = std::make_shared<PacketParser>();
  std::unique_ptr<uint8_t[]> buf(new uint8_t[file_size]);
  size_t read_size = fread(buf.get(), 1, file_size, fp);
  if (read_size == file_size) {
    packet_parser->ReadRtpPacketInfo((uint8_t*)buf.get(), file_size);
  } else {
    LOG_ERROR("file read error read_size:{}, file_size:{}", read_size,
              file_size);
  }

  fclose(fp);
  return 0;
}