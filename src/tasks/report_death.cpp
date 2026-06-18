#include "report_death.hpp"
#include "Geode/utils/web.hpp"
#include <matjson.hpp>
#include "../managers/api_manager.hpp"

ReportDeathTask
reportDeath(const ReportDeathInput &input) {
  web::WebRequest req = web::WebRequest();

  matjson::Value inputJson = input;
  req.bodyJSON(inputJson);
  req.header("Content-Type", "application/json");

  req.timeout(std::chrono::seconds(5));

  auto response =
      co_await req.post(APIManager::get().getEndpoint("v3/reportDeath"));
  if (response.ok()) {
    co_return Ok();
  }
  co_return Err("Not reported death with code {}.", response.code());
}
