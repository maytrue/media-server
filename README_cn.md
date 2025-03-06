# 多路视频合流

项目目标是实现多路视频合流，将多路视频流合并成一路视频流并直播出去。

## 架构图

https://excalidraw.com/#json=ILj9QZ2p_7UCBEAuLzcNh,1rq0yuzcevkr3wd0MqK3QQ


## 源码组织

### vcpkg 依赖管理

项目主要使用 vcpkg 进行 c++ 依赖管理，如果 vcpkg 没有对应的库，则通过 cmake 管理第三方库，如 webrtc rtp-rtcp 等。

vcpkg 添加第三方库的方法如下：

```shell
# 添加 boost-asio
vcpkg add port boost-asio
# 添加 boost-beast
vcpkg add port boost-beast 
```