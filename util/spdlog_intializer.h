//
// Created by guowei on 2022/11/23.
//

#pragma once

#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#define LOG_INFO(...) spdlog::info(__VA_ARGS__);
#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__);
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__);

namespace rtcserver {

class SpdlogInitializer {
 public:
  static void Init() {
    auto daily_sink =
        std::make_shared<spdlog::sinks::daily_file_sink_mt>("logfile", 23, 59);
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    // do not need to set log pattern here
    // spdlog::set_pattern("[%Y-%m-%d %H:%M:%S:%e] [tid:%t] %v");

    auto logger = std::make_shared<spdlog::logger>(
        "multi_sink", spdlog::sinks_init_list({console_sink, daily_sink}));
    logger->set_pattern("[%Y-%m-%d %H:%M:%S:%e] [tid:%t] %v");
    spdlog::set_default_logger(logger);
  }
};

}  // namespace rtcserver
