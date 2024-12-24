#include <iostream>
#include "util/spdlog_intializer.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

using namespace rtcserver;

int main(int argc, char *argv[]) {
  SpdlogInitializer::Init();

  std::cout << avformat_version() << std::endl;
  std::cout << avformat_configuration() << std::endl;

  const AVCodec* codec = NULL;
  void* codec_iterator = NULL;
  while (codec = av_codec_iterate(&codec_iterator)) {
    // LOG_INFO("Codec:{} Type:{}", codec->name, av_get_media_type_string(codec->type));
  }

  const AVInputFormat* input_format = NULL;
  void* input_format_iterator = NULL;
  while (input_format = av_demuxer_iterate(&input_format_iterator)) {
    // LOG_INFO("InputFormat:{}", input_format->name);
  }

  const AVOutputFormat* output_format = NULL;
  void* output_format_iterator = NULL;
  while (output_format = av_muxer_iterate(&output_format_iterator)) {
    LOG_INFO("OutputFormat:{}", output_format->name);
  }

  return 0;
}