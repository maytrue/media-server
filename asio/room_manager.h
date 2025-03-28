//
// Created by zouguowei on 2025/1/8.
//

#pragma once

#include <iostream>
#include <memory>

namespace rtcserver {

class Participant {
 public:
  Participant(uint64_t pid, uint32_t audio_ssrc, uint32_t video_ssrc)
      : pid_(pid), audio_ssrc_(audio_ssrc), video_ssrc_(video_ssrc) {
  }

  ~Participant() = default;

 private:
  uint64_t pid_ = 0;
  uint32_t audio_ssrc_ = 0;
  uint32_t video_ssrc_ = 0;
};

class Room {
 public:
  explicit Room(uint64_t room_id) : room_id_(room_id) {

  }

  ~Room() = default;

  void AddParticipant(uint64_t pid, uint32_t audio_ssrc, uint32_t video_ssrc) {
    participants_.emplace_back(pid, audio_ssrc, video_ssrc);
  }

  uint64_t room_id() {
    return room_id_;
  }

 private:
  std::vector<Participant> participants_;
  uint64_t room_id_ = 0;
};

class RoomManager {
 public:
  RoomManager() {
  }

  ~RoomManager() = default;

  void AddRoom(uint64_t rid) {
    rooms_.insert(std::make_pair(rid, std::make_shared<Room>(rid)));
  }

  std::shared_ptr<Room> GetRoom(uint64_t rid) {
    auto it = rooms_.find(rid);
    if (it != rooms_.end()) {
      return it->second;
    }
    return nullptr;
  }

  void RemoveRoom(uint64_t rid) {
    rooms_.erase(rid);
  }

  std::vector<std::shared_ptr<Room>> GetRooms() {
    std::vector<std::shared_ptr<Room>> rooms;
    for (auto &room : rooms_) {
      rooms.push_back(room.second);
    }
    return rooms;
  }

 private:
  std::unordered_map<uint64_t, std::shared_ptr<Room>> rooms_;
};

}

