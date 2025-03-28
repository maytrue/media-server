#include "rtp_rtcp/rtp.h"

#include <iostream>

#include "rtp_rtcp/rtp.h"
#include "util/spdlog_intializer.h"

using namespace rtcserver;

int main(int argc, char *argv[]) {
  SpdlogInitializer::Init();

  uint8_t data[12] = {0x80, 0xe3, 0x5d, 0x25, 0x00, 0x00, 0x03, 0xC0, 0x04, 0x3E, 0xee, 0x04};
  RtpCommonHeader* rtp_header = (RtpCommonHeader*)data;
  rtp_header->timestamp = ntohl(rtp_header->timestamp);
  rtp_header->ssrc = ntohl(rtp_header->ssrc);
  rtp_header->seq = ntohs(rtp_header->seq);

  std::cout << "version: " << int(rtp_header->version) << std::endl;
  std::cout << "seq: " << rtp_header->seq << std::endl;
  std::cout << "timestamp: " << rtp_header->timestamp << std::endl;
  std::cout << "ssrc: " << rtp_header->ssrc << std::endl;

  // if (argc != 2) {
  //   LOG_ERROR("please input rtp file");
  //   return 0;
  // }
  //
  // char *file_name = argv[1];
  // FILE *fp = fopen(file_name, "rb");
  // if (!fp) {
  //   LOG_ERROR("file:{} open error", file_name);
  //   return 0;
  // }
  //
  // fseek(fp, 0, SEEK_END);
  // size_t file_size = ftell(fp);
  // rewind(fp);
  //
  // auto packet_parser = std::make_shared<PacketParser>();
  // std::unique_ptr<uint8_t[]> buf(new uint8_t[file_size]);
  // size_t read_size = fread(buf.get(), 1, file_size, fp);
  // if (read_size == file_size) {
  //   packet_parser->ReadRtpPacketInfo((uint8_t *)buf.get(), file_size);
  // } else {
  //   LOG_ERROR("file read error read_size:{}, file_size:{}", read_size,
  //             file_size);
  // }
  //
  // fclose(fp);
  return 0;
}