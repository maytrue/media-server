#include "rtp_rtcp/rtp.h"

#include <arpa/inet.h>
#include <netinet/in.h>

#include "util/rtc_server_log.h"

static const char* RTPFILE_VERSION = "1.0";

PacketParser::PacketParser() {}

PacketParser::~PacketParser() {}

void PacketParser::ReadRtpPacketInfo(uint8_t* data, size_t len) {
  uint8_t* ptr = (uint8_t*)data;
  size_t bytes_remain = len;
  size_t pos = 0;
  if (!dump_header_parsed_) {
    char magic[80];
    sprintf(magic, "#!rtpplay%s ", RTPFILE_VERSION);

    if (bytes_remain < strlen(magic)) {
      return;
    }

    if (strncmp((char*)data, magic, strlen(magic)) != 0) {
      return;
    }

    std::string str;
    str.assign((char*)data, strlen(magic));
    LOG_INFO("magic str:{}", str);

    // skip magic
    ptr = ptr + strlen(magic);
    bytes_remain = bytes_remain - strlen(magic);
    pos += strlen(magic);

    // find new line
    while (bytes_remain > 0) {
      if (data[pos] == '\n') {
        break;
      }
      pos = pos + 1;
      bytes_remain = bytes_remain - 1;
      ptr = ptr + 1;
    }

    str.assign((char*)data, pos);
    LOG_INFO("byte:{:#04x} pos:{} str:{}", data[pos], pos, str);
    bytes_remain = bytes_remain - 1;
    ptr = ptr + 1;
    pos = pos + 1;

    if (bytes_remain < sizeof(RD_hdr_t)) {
      return;
    }

    RD_hdr_t* hdr = (RD_hdr_t*)ptr;
    struct in_addr in;
    in.s_addr = hdr->source;
    LOG_INFO("source:{} port:{}", inet_ntoa(in), ntohs(hdr->port));

    ptr = ptr + sizeof(RD_hdr_t);
    bytes_remain = bytes_remain - sizeof(RD_hdr_t);
    dump_header_parsed_ = true;
  }

  do {
    if (bytes_remain < sizeof(RD_packet_t)) {
      break;
    }

    RD_packet_t* packet = (RD_packet_t*)ptr;
    packet->length = ntohs(packet->length);
    size_t body_len = packet->length - sizeof(RD_packet_t);
    packet->plen = ntohs(packet->plen);
    LOG_INFO("packet_len:{} body_len:{} plen:{}", packet->length, body_len,
             packet->plen);
    if (bytes_remain < packet->length) {
      break;
    }
    ptr = ptr + sizeof(RD_packet_t);

    if (packet->plen == 0) {
      // RTCP
      RtcpHeader* head = (RtcpHeader*)ptr;
      uint8_t version = head->version;
      head->length = ntohs(head->length);
      LOG_INFO("RTCP version:{} packet_type:{} length:{}", version,
               head->packet_type, head->length);
      switch (head->packet_type) {
        case kRtcpPayloadTypeSR:
          RtcpSR* sr = (RtcpSR*)(ptr + sizeof(RtcpHeader));
          sr->ssrc = ntohl(sr->ssrc);
          LOG_INFO("RTCP sender report ssrc:{:#04x}", sr->ssrc);
          break;
      }
    } else {
      RtpCommonHeader* head = (RtpCommonHeader*)ptr;
      uint8_t version = head->version;
      head->seq = ntohs(head->seq);
      head->ssrc = ntohl(head->ssrc);
      LOG_INFO("RTP version:{} seq:{} ssrc:{:#04x}", version, head->seq,
               head->ssrc);
    }
    ptr = ptr + body_len;
    bytes_remain = bytes_remain - packet->length;
    LOG_INFO("bytes_remain:{}", bytes_remain);
  } while (bytes_remain > 0);
}