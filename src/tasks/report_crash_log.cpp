#include "report_crash_log.hpp"
#include "Geode/utils/web.hpp"
#include <matjson.hpp>
#include "../managers/api_manager.hpp"

ReportCrashLogTask
reportCrashLog(const ReportCrashLogInput &input) {
  web::WebRequest req = web::WebRequest();

  matjson::Value inputJson = input;
  req.bodyJSON(inputJson);
  req.header("Content-Type", "application/json");

  req.timeout(std::chrono::seconds(5));

  auto response =
      co_await req.post(APIManager::get().getEndpoint("v3/reportCrashLog"));
  if (response.ok()) {
    co_return Ok();
  }
  co_return Err("Not reported crash log with code {}.", response.code());
}
