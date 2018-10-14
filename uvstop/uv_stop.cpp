//
// Created by guowei on 2018/10/11.
//

#include <iostream>
#include <uv.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>

static int counter = 0;

void idle_callback(uv_idle_t *handle) {
  std::cout << "idle callback" << std::endl;
  counter ++;
  if (counter >=5) {
    uv_stop(uv_default_loop());
    std::cout << "uv_stop called" << std::endl;
  }
}

static void fake_download(uv_work_t* handle) {
  std::cout << "fake_download:" << std::this_thread::get_id() << std::endl;
}


static void after(uv_work_t* handle, int status) {
  std::cout << "after:" << std::this_thread::get_id() << std::endl;

}

void prepare_callback(uv_prepare_t *handle) {
  std::cout << "prepare callback" << std::endl;

  uv_work_t work_request;
  uv_queue_work(uv_default_loop(), &work_request, fake_download, after);
}

void timer_callback(uv_timer_t *handle) {
//  std::cout << "thread:" << std::this_thread::get_id() << std::endl;
  std::cout << "timer callback" << std::endl;
}


int main(int argc, char* argv[])
{
  uv_idle_t idler;
  uv_prepare_t prepare;

//  uv_idle_init(uv_default_loop(), &idler);
//  uv_idle_start(&idler, idle_callback);

  std::cout << "thread:" << std::this_thread::get_id() << std::endl;

  uv_prepare_init(uv_default_loop(), &prepare);
  uv_prepare_start(&prepare, prepare_callback);

//  uv_timer_t timer;
//  uv_timer_init(uv_default_loop(), &timer);
//  uv_timer_start(&timer, timer_callback, 5000, 2000);



  uv_run(uv_default_loop(), UV_RUN_DEFAULT);

  return 0;
}