#pragma once

#include <uv.h>

#include <thread>

#include "util/spdlog_intializer.h"

class UdpThread {
 public:
  class UdpThreadListener {
   public:
    virtual void OnDataRecv(char* data, int len) = 0;
  };

  UdpThread(int local_port, UdpThreadListener* listener)
      : listener_(listener), local_port_(local_port) {
    uv_loop_ = new uv_loop_t;
    uv_loop_init(uv_loop_);

    // struct sockaddr_in broadcast_addr;
    // uv_ip4_addr("0.0.0.0", 0, &broadcast_addr);
    // uv_udp_bind(&uv_udp_send_, (const struct sockaddr *)&broadcast_addr,
    // UV_UDP_REUSEADDR);
    LOG_INFO("[UdpThread] construct");
  }

  ~UdpThread() {
    // thread_->Stop();
    LOG_INFO("[UdpThread] ~UdpThread");
    if (thread_.joinable()) {
      thread_.join();
      LOG_INFO("[UdpThread] ~UdpThread join");
    }
  }

  void Start() {
    uv_udp_init(uv_loop_, &uv_udp_recv_);
    uv_udp_recv_.data = this;

    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", local_port_, &addr);
    uv_udp_bind(&uv_udp_recv_, (const struct sockaddr*)&addr, UV_UDP_REUSEADDR);
    uv_udp_recv_start(&uv_udp_recv_, AllocBuffer, OnRead);

    uv_udp_init(uv_loop_, &uv_udp_send_);
    uv_async_.data = this;
    uv_async_init(uv_loop_, &uv_async_, OnAsyncTask);

    LOG_INFO("[UdpThread] start loop");
    thread_ = std::thread([=] {
      LOG_INFO("[UdpThread] running loop");
      uv_run(uv_loop_, UV_RUN_DEFAULT);
      LOG_INFO("[UdpThread] close loop");
    });
  }

  void SendTo(const std::string& dest_ip, int dest_port, char* send_buf,
              int len) {
    LOG_INFO("[UdpThread] SendTo ip:{} port:{} len:{}", dest_ip, dest_port,
             len);

    struct sockaddr_in send_addr;
    uv_ip4_addr(dest_ip.c_str(), dest_port, &send_addr);

    uv_buf_t buf = uv_buf_init(send_buf, len);
    uv_udp_send_t* req = (uv_udp_send_t*)malloc(sizeof(uv_udp_send_t));
    uv_udp_send(req, &uv_udp_send_, &buf, 1, (const struct sockaddr*)&send_addr,
                OnSend);
  }

  void Close() {
    LOG_INFO("[UdpThread] Close");
    uv_async_send(&uv_async_);
  }

  static void AllocBuffer(uv_handle_t* handle, size_t suggested_size,
                          uv_buf_t* buf) {
    LOG_INFO("[UdpThread] AllocBuffer:{}", suggested_size);
    if (suggested_size > 0) {
      buf->base = (char*)malloc(suggested_size);
      buf->len = suggested_size;
    }
  }

  static void OnRead(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf,
                     const struct sockaddr* addr, unsigned flags) {
    LOG_INFO("[UdpThread] OnRead:{}", nread);
    UdpThread* udp_thread = (UdpThread*)handle->data;
    UdpThreadListener* listener = udp_thread->listener_;
    if (nread > 0) {
      char sender_ip[128] = {0};
      uv_ip4_name((const struct sockaddr_in*)addr, sender_ip, 16);
      LOG_INFO("[UdpThread] OnRead src:{}", sender_ip);

      uv_udp_send_t* req = (uv_udp_send_t*)malloc(sizeof(uv_udp_send_t));
      uv_buf_t buff = uv_buf_init(buf->base, nread);
      uv_udp_send(req, handle, &buff, 1, addr, OnSend);

      if (listener) {
        listener->OnDataRecv(buf->base, nread);
      }
      free(buf->base);
    }
  }

  static void OnSend(uv_udp_send_t* req, int status) {
    LOG_INFO("[UdpThread] OnSend status:{}", status);
    if (req) {
      free(req);
      req = nullptr;
    }
  }

  static void OnAsyncTask(uv_async_t* handle) {
    // these function (uv_stop) must be called from uv thread
    LOG_INFO("[UdpThread] OnAsyncTask");
    UdpThread* udp_thread = (UdpThread*)handle->data;

    if (uv_is_active((uv_handle_t*)&udp_thread->uv_udp_recv_)) {
      uv_udp_recv_stop(&udp_thread->uv_udp_recv_);
    }

    if (uv_is_closing((uv_handle_t*)&udp_thread->uv_udp_recv_) == 0) {
      uv_close((uv_handle_t*)&udp_thread->uv_udp_recv_,
               [](uv_handle_t* handle) {
                 LOG_INFO("[UdpThread] OnAsyncTask onClose");
               });
    }
    uv_stop(udp_thread->uv_loop_);
  }

 private:
  uv_loop_t* uv_loop_;
  uv_udp_t uv_udp_recv_;
  uv_udp_t uv_udp_send_;
  uv_async_t uv_async_;
  std::thread thread_;
  UdpThreadListener* listener_;
  int local_port_ = 0;
};
