#include "get_replay.hpp"
#include "Geode/utils/web.hpp"
#include "../managers/api_manager.hpp"

GetReplayTask getReplay(const GetReplayInput& input) {
  web::WebRequest req = web::WebRequest();

  matjson::Value inputJson = input;
  req.bodyJSON(inputJson);
  req.header("Content-Type", "application/json");

  req.timeout(std::chrono::seconds(5));

  auto response =
      co_await req.post(APIManager::get().getEndpoint("v3/getReplay"));
  ByteVector gdr2Raw = response.data();
  std::span<uint8_t> gdr2RawSpan(gdr2Raw);
  auto gdr2 = ShowcaseBotReplay::importData(gdr2RawSpan);
  if (gdr2.isErr()) {
    co_return Err(gdr2.unwrapErr());
  }
  co_return Ok(gdr2.unwrap());
}
