//
// Created by zouguowei on 2025/1/13.
//

#include <memory>

extern "C" {
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
}

class VideoOverlayer {
 public:
  VideoOverlayer(uint32_t width, uint32_t height)
      : canvas_width_(width), canvas_heigth_(height) {

    InitOverlayFilter(&filter_graph_, &buffersrc_ctx_, &buffersrc_overlay_ctx_,
                      &buffersink_ctx_, nullptr, nullptr);
  }

  int InitOverlayFilter(AVFilterGraph **graph,
                        AVFilterContext **buffersrc_ctx,
                        AVFilterContext **buffersrc_overlay_ctx,
                        AVFilterContext **buffersink_ctx,
                        AVCodecContext *dec_ctx,
                        AVCodecContext *overlay_dec_ctx) {
    char args[512];
    int ret = 0;
    const AVFilter *buffersrc = avfilter_get_by_name("buffer");
    const AVFilter *buffersrc_overlay = avfilter_get_by_name("buffer");
    const AVFilter *overlay = avfilter_get_by_name("overlay");
    const AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs = avfilter_inout_alloc();

    *graph = avfilter_graph_alloc();
    if (!outputs || !inputs || !*graph) {
      ret = AVERROR(ENOMEM);
      goto end;
    }

    /* Main video input */
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
             dec_ctx->time_base.num, dec_ctx->time_base.den,
             dec_ctx->sample_aspect_ratio.num,
             dec_ctx->sample_aspect_ratio.den);

    ret = avfilter_graph_create_filter(buffersrc_ctx, buffersrc, "in",
                                       args, NULL, *graph);
    if (ret < 0) {
      av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
      goto end;
    }

    /* Overlay video input */
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             overlay_dec_ctx->width, overlay_dec_ctx->height,
             overlay_dec_ctx->pix_fmt,
             overlay_dec_ctx->time_base.num, overlay_dec_ctx->time_base.den,
             overlay_dec_ctx->sample_aspect_ratio.num,
             overlay_dec_ctx->sample_aspect_ratio.den);

    ret = avfilter_graph_create_filter(buffersrc_overlay_ctx, buffersrc_overlay,
                                       "overlay", args, NULL, *graph);
    if (ret < 0) {
      av_log(NULL, AV_LOG_ERROR, "Cannot create overlay buffer source\n");
      goto end;
    }

    /* Buffer video sink */
    ret = avfilter_graph_create_filter(buffersink_ctx, buffersink, "out",
                                       NULL, NULL, *graph);
    if (ret < 0) {
      av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
      goto end;
    }

    /* Endpoints for the filter graph */
    outputs->name = av_strdup("in");
    outputs->filter_ctx = *buffersrc_ctx;
    outputs->pad_idx = 0;
    outputs->next = NULL;

    inputs->name = av_strdup("out");
    inputs->filter_ctx = *buffersink_ctx;
    inputs->pad_idx = 0;
    inputs->next = NULL;

    /* Construct the filtergraph */
    snprintf(args, sizeof(args),
             "[in][overlay]overlay=x=10:y=10[out]");

    ret = avfilter_graph_parse_ptr(*graph, args,
                                   &inputs, &outputs, NULL);
    if (ret < 0)
      goto end;

    ret = avfilter_graph_config(*graph, NULL);
    if (ret < 0)
      goto end;

    end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    return ret;
  }

  int ProcessFrame(AVFrame *frame, AVFrame *overlay_frame) {
    int ret = 0;
    if (frame) {
      ret = av_buffersrc_add_frame_flags(buffersrc_ctx_, frame,
                                         AV_BUFFERSRC_FLAG_KEEP_REF);
      if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error feeding main video\n");
        return ret;
      }
    }

    if (overlay_frame) {
      ret = av_buffersrc_add_frame_flags(buffersrc_overlay_ctx_, overlay_frame,
                                         AV_BUFFERSRC_FLAG_KEEP_REF);
      if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error feeding overlay video\n");
        return ret;
      }
    }

    while (1) {
      AVFrame *filt_frame = av_frame_alloc();
      ret = av_buffersink_get_frame(buffersink_ctx_, filt_frame);
      if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        break;
      if (ret < 0)
        return ret;

      // Process filtered frame here

      av_frame_unref(filt_frame);
    }

    return ret;
  }

 private:
  uint32_t canvas_width_ = 0;
  uint32_t canvas_heigth_ = 0;
  AVFilterGraph *filter_graph_ = nullptr;
  AVFilterContext *buffersrc_ctx_ = nullptr;
  AVFilterContext *buffersrc_overlay_ctx_ = nullptr;
  AVFilterContext *buffersink_ctx_ = nullptr;
};

AVFrame *create_frame_from_yuv420p(int width, int height,
                                   const uint8_t *y_data, const uint8_t *u_data, const uint8_t *v_data,
                                   int y_stride, int u_stride, int v_stride) {
  AVFrame *frame = av_frame_alloc();
  if (!frame) {
    return nullptr;
  }

  frame->format = AV_PIX_FMT_YUV420P;
  frame->width = width;
  frame->height = height;

  int ret = av_frame_get_buffer(frame, 0);
  if (ret < 0) {
    av_frame_free(&frame);
    return nullptr;
  }

  for (int i = 0; i < height; i++) {
    memcpy(frame->data[0] + i * frame->linesize[0], y_data + i * y_stride, width);
  }

  for (int i = 0; i < height / 2; i++) {
    memcpy(frame->data[1] + i * frame->linesize[1], u_data + i * u_stride, width / 2);
    memcpy(frame->data[2] + i * frame->linesize[2], v_data + i * v_stride, width / 2);
  }

  return frame;
}

AVFrame *LoadYuv420FromFile(const char *filename, int width, int height) {
  // Allocate buffers for YUV planes
  size_t y_size = width * height;
  size_t u_size = y_size / 4;
  size_t v_size = y_size / 4;

  std::unique_ptr<uint8_t[]> y_data(new uint8_t[y_size]);
  std::unique_ptr<uint8_t[]> u_data (new uint8_t[u_size]);
  std::unique_ptr<uint8_t[]> v_data(new uint8_t[v_size]);

  if (!y_data || !u_data || !v_data) {
    return nullptr;
  }

  FILE *file = fopen(filename, "rb");
  if (!file) {
    return nullptr;
  }

  // Read YUV data from file
  if (fread(y_data.get(), 1, y_size, file) != y_size ||
      fread(u_data.get(), 1, u_size, file) != u_size ||
      fread(v_data.get(), 1, v_size, file) != v_size) {
    fclose(file);
    return nullptr;
  }

  fclose(file);

  // Create frame from the loaded data
  AVFrame *frame = create_frame_from_yuv420p(width, height,
                                             y_data.get(), u_data.get(), v_data.get(),
                                             width, width / 2, width / 2);

  return frame;
}

int main(int argc, char *argv[]) {
  auto overlay = std::make_shared<VideoOverlayer>(1080, 960);

  AVFrame *frame = av_frame_alloc();
  AVFrame *overlay_frame = av_frame_alloc();

  overlay->ProcessFrame(frame, overlay_frame);

  av_frame_unref(frame);
  av_frame_free(&frame);

  av_frame_unref(overlay_frame);
  av_frame_free(&overlay_frame);

  return 0;
}
