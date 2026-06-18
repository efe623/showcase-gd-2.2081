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
  GEODE_UNWRAP_INTO(matjson::Value json, response.json());
  GEODE_UNWRAP_INTO(LevelReplayStatus data, json.as<LevelReplayStatus>());
  co_return Ok(data);
}
