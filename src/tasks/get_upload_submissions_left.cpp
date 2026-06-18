#include "get_upload_submissions_left.hpp"
#include "Geode/utils/web.hpp"
#include "../managers/api_manager.hpp"

GetUploadSubmissionsLeftTask
getUploadSubmissionsLeft(const GetUploadSubmissionsLeftInput &input) {
  web::WebRequest req = web::WebRequest();

  matjson::Value inputJson = input;
  req.bodyJSON(inputJson);
  req.header("Content-Type", "application/json");

  req.timeout(std::chrono::seconds(5));

  auto response =
      co_await req.post(APIManager::get().getEndpoint("v3/getSubmissionsLeft"));
  auto json = response.json();
  if (json.isErr()) {
    co_return Err(json.unwrapErr());
  }
  auto data = json.unwrap().as<UploadSubmissionsLeftResponse>();
  if (data.isErr()) {
    co_return Err(data.unwrapErr());
  }
  co_return Ok(data.unwrap());
}
