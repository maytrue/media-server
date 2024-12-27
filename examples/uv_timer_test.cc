//
// Created by zouguowei on 2024/12/27.
//

#include <iostream>
#include <uv.h>
#include <thread>

int main(int argc, char* argv[]) {
  uv_loop_t* loop = uv_loop_new();

  uv_timer_t timer;
  uv_timer_init(loop, &timer);
  uv_timer_start(&timer, [](uv_timer_t* handle) {
    std::cout << "timer callback" << std::endl;
  }, 1000, 2000);

  uv_run(loop, UV_RUN_DEFAULT);
  uv_loop_close(loop);
  uv_loop_delete(loop);
  return 0;
}