#include <iostream>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavcodec/bsf.h>
}

void extract_sei(AVPacket *pkt) {
  uint8_t *data = pkt->data;
  int size = pkt->size;
  int nal_type = 0;
  std::cout << "extract_sei size:" << size << std::endl;
  while (size > 4) {

    uint32_t nal_size = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
    std::cout << "nal_size:" << nal_size << std::endl;

    nal_type = (data[4] & 0x1F); // For H.264
    std::cout << "nal_type:" << nal_type << std::endl;

    // if (data[0] == 0 && data[1] == 0 && data[2] == 1) {
    //   nal_type = (data[3] & 0x1F); // For H.264
    //   std::cout << "nal_type:" << nal_type << std::endl;
    // }

    size = size - nal_size - 4;
    data = data + nal_size + 4;
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "please input file" << std::endl;
    return 0;
  }
  char *file_name = argv[1];

  AVFormatContext *fmt_ctx = NULL;
  int ret = avformat_open_input(&fmt_ctx, file_name, NULL, NULL);
  std::cout << "avformat_open_input:" << ret << std::endl;
  if (ret != 0) {
    std::cout << "avformat_open_input fail:" << ret << std::endl;
    return -1;
  }

  avformat_find_stream_info(fmt_ctx, NULL);
  int video_stream_index =
      av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
  if (video_stream_index < 0) {
    printf("Could not find video stream\n");
    return -1;
  }

  std::cout << "video_stream_index:" << video_stream_index << std::endl;
  av_dump_format(fmt_ctx, 0, file_name, 0);

  AVPacket *pkt = av_packet_alloc();
  AVPacket *bsf_pkt = av_packet_alloc();
  while (av_read_frame(fmt_ctx, pkt) >= 0) {
    if (pkt->stream_index == video_stream_index) {
      if (pkt->flags & AV_PKT_FLAG_KEY) {
        uint8_t *extradata = fmt_ctx->streams[video_stream_index]->codecpar->extradata;
        int extradata_size = fmt_ctx->streams[video_stream_index]->codecpar->extradata_size;
        std::cout << "extradata_size:" << extradata_size << std::endl;

        std::cout << "version:" << (extradata[0] & 0xFF) << std::endl;
        int sps_number = (extradata[5] & 0x1F);
        std::cout << "sps_number:" << sps_number << std::endl;

        int sps_length = 0;
        int offset = 6;
        for (int i = 0; i < sps_number; i++) {
          sps_length = (extradata[6] << 8) | extradata[7];
          std::cout << "sps_length:" << sps_length << std::endl;
          offset = offset + 2 + sps_length;
        }

        std::cout << "pps_offset:" << offset << std::endl;
        int pps_number = (extradata[offset] & 0xFF);
        std::cout << "pps_number:" << pps_number << std::endl;
        offset ++;

        for (int i = 0; i < pps_number; i++) {
          int pps_length = (extradata[offset] << 8) | extradata[offset + 1];
          offset = offset + 2;
          std::cout << "pps_length:" << pps_length << std::endl;
        }
      }

      extract_sei(pkt);
    }
    av_packet_unref(pkt);
  }

  av_packet_free(&pkt);
  av_packet_free(&bsf_pkt);
  avformat_close_input(&fmt_ctx);

  std::cout << "parser exit" << std::endl;
  return 0;
}