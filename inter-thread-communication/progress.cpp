//
// Created by guowei on 2018/10/11.
//

#include <iostream>
#include <uv.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>

typedef struct DownloadInfo
{
  int size;
  uv_async_t* async;
} DownloadInfo;


static void print_progress(uv_async_t* handle)
{
  std::cout << "print_progress:" << std::this_thread::get_id() << std::endl;

  double percentage = *((double*)(handle->data));
  printf("Downloaded: %.2f\n", percentage);
}

static void fake_download(uv_work_t* handle)
{
  double  percentage = 0.0;

  std::cout << "fake_download:" << std::this_thread::get_id() << std::endl;
//
  DownloadInfo info = *((DownloadInfo *)handle->data);
  int size = info.size;
  uv_async_t* async = info.async;

//  int size = *((int*)handle->data);

  int downloaded = 0;
  while (downloaded < size) {
      percentage = downloaded * 100.0 / size;
      (*async).data = (void *)&percentage;

      uv_async_send(async);
      std::this_thread::sleep_for(std::chrono::seconds(1));
      downloaded += (200 + random()) % 1000;
   }
}

static void after(uv_work_t* handle, int status)
{
  std::cout << "after:" << std::this_thread::get_id() << std::endl;
  std::cout << "Download complete" << std::endl;

  DownloadInfo info = *((DownloadInfo *)handle->data);
  int size = info.size;
  uv_async_t* async = info.async;

  uv_close((uv_handle_t *)async, NULL);
}


int main(int argc, char* argv[])
{
  uv_work_t work_request;
  int size = 10240;

  std::cout << "MainThread:" << std::this_thread::get_id() << std::endl;

  uv_async_t async;
  uv_async_init(uv_default_loop(), &async, print_progress);

  DownloadInfo info;
  info.size = size;
  info.async = &async;
//
  work_request.data = (void*)&info;
//  work_request.data = (void*)&size;
  uv_queue_work(uv_default_loop(), &work_request, fake_download, after);

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);

  return 0;
}

