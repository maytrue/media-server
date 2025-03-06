//
// Created by zouguowei on 2025/1/9.
//

#include <iostream>
#include <libavfilter/avfilter.h>

struct LayoutConfig {
  uint32_t x = 0;
  uint32_t y = 0;
  uint32_t width = 0;
  uint32_t height = 0;
};

class RawVideoFrame {
 public:
  RawVideoFrame(uint32_t width, uint32_t height)
      : width_(width), height_(height) {
    frame_ = av_frame_alloc();
    frame_->width = width;
    frame_->height = height;
    frame_->format = AV_PIX_FMT_YUV420P;
    av_frame_get_buffer(frame_, 0);
  }

  ~RawVideoFrame() {
    if (frame_) {
      av_frame_free(&frame_);
    }
  }

  AVFrame* frame() {
    return frame_;
  }

 private:
  AVFrame* frame_ = nullptr;
  uint32_t width_ = 0;
  uint32_t height_ = 0;
};

class VideoOverlayer {
 public:
  VideoOverlayer(uint32_t width, uint32_t height)
      : canvas_width_(width), canvas_heigth_(height) {
    if (!filter_graph_) {
      filter_graph_ = avfilter_graph_alloc();
    }
  }

  ~VideoOverlayer() {
    if (filter_graph_) {
      avfilter_graph_free(&filter_graph_);
    }
  }

  // source_id start from 1, if main_source_id_ = 0 means source id not set
  void SetMainSource(uint32_t source_id) {
    main_source_id_ = source_id;
  }

  // source_id start from 1
  void AddSource(uint32_t source_id, const LayoutConfig& layout_config) {
    layout_configs_[source_id] = layout_config;
    frames_[source_id] = nullptr;
  }

  void InputVideoFrame(const std::shared_ptr<RawVideoFrame>& frame, uint32_t source_id) {
    frames_[source_id] = frame;
    if (main_source_id_ == 0) {
      return;
    }
    // check all source is ready
    for (const auto& it : frames_) {
      if (!it.second) {
        return;
      }
    }

    // main source drive overlay
    if (source_id == main_source_id_) {
      Overlay();
    }
  }

  void Overlay() {
  }

 private:
  AVFilterGraph* filter_graph_ = nullptr;
  uint32_t canvas_width_ = 0;
  uint32_t canvas_heigth_ = 0;

  std::unordered_map<uint32_t, std::shared_ptr<RawVideoFrame>> frames_;
  std::unordered_map<uint32_t, LayoutConfig> layout_configs_;
  uint32_t main_source_id_ = 0;
};

int main(int argc, char *argv[]) {
  std::cout << "Hello, World!" << std::endl;


  return 0;
}