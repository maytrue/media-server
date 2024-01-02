#pragma once

#include <iostream>

struct RtcpHeader {
  uint8_t count : 5;
  uint8_t padding : 1;
  uint8_t version : 2;

  uint8_t packet_type;
  uint16_t length;
};

struct RtpCommonHeader {
  uint8_t csrc_count : 4;
  uint8_t extension : 1;
  uint8_t padding : 1;
  uint8_t version : 2;

  uint8_t payload : 7;
  uint8_t mark : 1;

  uint16_t seq;
  uint32_t timestamp;
  uint32_t ssrc;
};

struct RtcpSR {
  uint32_t ssrc;    /* sender generating this report */
  uint32_t ntp_sec; /* NTP timestamp */
  uint32_t ntp_frac;
  uint32_t rtp_ts; /* RTP timestamp */
  uint32_t psent;  /* packets sent */
  uint32_t osent;  /* octets sent */
};

typedef struct {
  struct timeval32 {
    uint32_t tv_sec;  /* start of recording (GMT) (seconds) */
    uint32_t tv_usec; /* start of recording (GMT) (microseconds)*/
  } start;
  uint32_t source;  /* network source (multicast address) */
  uint16_t port;    /* UDP port */
  uint16_t padding; /* padding */
} RD_hdr_t;

typedef struct {
  uint16_t length; /* length of packet, including this header (may
                      be smaller than plen if not whole packet recorded) */
  uint16_t plen;   /* actual header+payload length for RTP, 0 for RTCP */
  uint32_t offset; /* milliseconds since the start of recording */
} RD_packet_t;

enum RtcpPayloadType {
  kRtcpPayloadTypeSR = 200,
  kRtcpPayloadTypeRR = 201,
  kRtcpPayloadTypeSDES = 202,
  kRtcpPayloadTypeBYE = 203,
  kRtcpPayloadTypeApp = 204
};

class PacketParser {
 public:
  PacketParser();
  ~PacketParser();

  void ReadRtpPacketInfo(uint8_t* data, size_t len);

 private:
  bool dump_header_parsed_ = false;
};
