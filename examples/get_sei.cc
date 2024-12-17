#include <iostream>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

void extract_sei(AVPacket *pkt) {
  uint8_t *data = pkt->data;
  int size = pkt->size;
  int nal_type = 0;
  // std::cout << "extract_sei size:" << size << std::endl;
  while (size > 4) {
    if (data[0] == 0 && data[1] == 0 && data[2] == 1) {
      nal_type = (data[3] & 0x1F); // For H.264
      std::cout << "nal_type:" << nal_type << std::endl;
    }

    data++;
    size--;
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

  AVPacket *pkt = av_packet_alloc();
  while (av_read_frame(fmt_ctx, pkt) >= 0) {
    if (pkt->stream_index == video_stream_index) {
      extract_sei(pkt);
    }
    av_packet_unref(pkt);
  }

  av_packet_free(&pkt);
  avformat_close_input(&fmt_ctx);
  return 0;
}