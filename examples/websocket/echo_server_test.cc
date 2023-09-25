/* We simply call the root header file "App.h", giving you uWS::App and
 * uWS::SSLApp */
#include <nlohmann/json.hpp>

#include "App.h"
using json = nlohmann::json;
/* This is a simple WebSocket echo server example.
 * You may compile it with "WITH_OPENSSL=1 make" or with "make" */

int main() {
  /* ws->getUserData returns one of these */
  struct PerSocketData {
    /* Fill with user data */
    uint64_t user_id = 0;
  };

  std::unordered_map<uint64_t, uWS::WebSocket<false, true, PerSocketData> *> ws_map;

  /* Keep in mind that uWS::SSLApp({options}) is the same as uWS::App() when
   * compiled without SSL support. You may swap to using uWS:App() if you don't
   * need SSL */
  uWS::App({/* There are example certificates in uWebSockets.js repo */
            .key_file_name = "misc/key.pem",
            .cert_file_name = "misc/cert.pem",
            .passphrase = "1234"})
      .get("/*", [&ws_map](auto* res, auto* req) {
        json result;
        result["result"] = "ok";
        result["userCount"] = ws_map.size();
        json array = json::array();
        for (auto it = ws_map.begin(); it != ws_map.end(); it++) {
          array.push_back(it->first);
        }
        result["userList"] = array;
        res->end(result.dump());
      }).ws<PerSocketData>(
          "/*",
          {/* Settings */
           .compression = uWS::CompressOptions(uWS::DEDICATED_COMPRESSOR_4KB |
                                               uWS::DEDICATED_DECOMPRESSOR),
           .maxPayloadLength = 100 * 1024 * 1024,
           .idleTimeout = 16,
           .maxBackpressure = 100 * 1024 * 1024,
           .closeOnBackpressureLimit = false,
           .resetIdleTimeoutOnSend = false,
           .sendPingsAutomatically = true,
           /* Handlers */
           .upgrade = nullptr,
           .open =
               [](auto *ws) {
                 /* Open event here, you may access ws->getUserData() which
                  * points to a PerSocketData struct */
                 //  PerSocketData *user_data = ws->getUserData();
                 //  user_data->handle = (void *)ws;
               },
           .message =
               [&ws_map](auto *ws, std::string_view message,
                         uWS::OpCode opCode) {
                 /* This is the opposite of what you probably want; compress if
                  * message is LARGER than 16 kb the reason we do the opposite
                  * here; compress if SMALLER than 16 kb is to allow for
                  * benchmarking of large message sending without compression */
                 nlohmann::json j = json::parse(message);
                 if (j.is_discarded()) {
                   std::cout << "message:" << message << " is invalid"
                             << std::endl;
                   return;
                 }

                 std::string message_type = j["messageType"];
                 if (message_type.compare("login") == 0) {
                   if (j.find("userId") == j.end()) {
                     std::cout << "user_id is empty" << std::endl;
                     return;
                   }
                   // login message
                   uint64_t user_id = j["userId"];
                   auto it_to = ws_map.find(user_id);
                   if (it_to == ws_map.end()) {
                    PerSocketData* socket_data = ws->getUserData();
                    socket_data->user_id = user_id;
                     ws_map.emplace(std::make_pair(
                         user_id,
                         (uWS::WebSocket<false, true, PerSocketData> *)ws));
                     nlohmann::json result;
                     result["result"] = "ok";
                     result["code"] = 0;
                     ws->send(result.dump(), opCode, result.dump().length() < 16 * 1024);
                   } else {
                     nlohmann::json result;
                     result["result"] = "dumplicate login";
                     result["code"] = -1;
                     ws->send(result.dump(), opCode, result.dump().length() < 16 * 1024);
                   }
                 } else if (message_type.compare("logout") == 0) {
                    if (j.find("userId") == j.end()) {
                     return;
                   }
                   uint64_t user_id = j["userId"];
                   auto it_to = ws_map.find(user_id);
                   if (it_to != ws_map.end()) {
                    ws_map.erase(user_id);
                   }
                } else if (message_type.compare("privateChat") == 0) {
                   if (j.find("toUserId") == j.end() ||
                       j.find("message") == j.end() ||
                       j.find("userId") == j.end()) {
                     return;
                   }

                   uint64_t to_user_id = j["toUserId"];
                   nlohmann::json result;
                   result["message"] = j["message"];
                   result["fromUserId"] = j["userId"];

                   auto it_to = ws_map.find(to_user_id);
                   if (it_to != ws_map.end()) {
                     it_to->second->send(result.dump(), opCode,
                                         result.dump().length() < 16 * 1024);
                   }
                 }
               },
           .drain =
               [](auto * /*ws*/) {
                 /* Check ws->getBufferedAmount() here */
               },
           .ping =
               [](auto * /*ws*/, std::string_view) {
                 /* Not implemented yet */
               },
           .pong =
               [](auto * /*ws*/, std::string_view) {
                 /* Not implemented yet */
               },
           .close =
               [&ws_map](auto * ws, int /*code*/, std::string_view /*message*/) {
                 /* You may access ws->getUserData() here */
                 PerSocketData *socket_data = ws->getUserData();
                 uint64_t user_id = socket_data->user_id;
                 auto it_to = ws_map.find(user_id);
                 if (it_to != ws_map.end()) {
                   ws_map.erase(user_id);
                 }
               }})
      .listen(8080,
              [](auto *listen_socket) {
                if (listen_socket) {
                  std::cout << "Listening on port " << 8080 << std::endl;
                }
              })
      .run();
}
