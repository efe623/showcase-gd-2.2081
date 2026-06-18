#include "get_level_replay_status.hpp"
#include "../managers/api_manager.hpp"
#include "Geode/utils/web.hpp"

GetLevelReplayStatusTask
getLevelReplayStatus(const LevelReplayStatusInput &input) {
  web::WebRequest req = web::WebRequest();

  matjson::Value inputJson = input;
  req.bodyJSON(inputJson);
  req.header("Content-Type", "application/json");

  req.timeout(std::chrono::seconds(5));
  auto response =
      co_await req.post(APIManager::get().getEndpoint("v3/getReplayStatus"));
  auto json = response.json();
  if (json.isErr()) {
    co_return Err(json.unwrapErr());
  }
  auto data = json.unwrap().as<LevelReplayStatus>();
  if (data.isErr()) {
    co_return Err(data.unwrapErr());
  }
  co_return Ok(data.unwrap());
}
