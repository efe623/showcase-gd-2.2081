#include "upload_submissions.hpp"
#include "Geode/utils/web.hpp"
#include <matjson.hpp>
#include "../managers/api_manager.hpp"

UploadSubmissionsTask
uploadSubmissions(const UploadSubmissionsInput &input,
                  const std::vector<std::vector<uint8_t>> &submissions) {

  web::WebRequest req = web::WebRequest();
  matjson::Value inputJson = input;

  web::MultipartForm form;
  form.param("metadata", inputJson.dump(matjson::NO_INDENTATION));
  for (const std::vector<uint8_t>& submissionData : submissions) {
    form.file("submissions", submissionData, "submission", "application/octet-stream");
  }

  req.bodyMultipart(form);
  req.header("Content-Type", form.getHeader());

  req.timeout(std::chrono::seconds(5));

  auto response = co_await req.post(APIManager::get().getEndpoint("v3/submit"));
  if (response.ok()) {
    co_return Ok();
  }
  co_return Err("Not submitted with code {}.", response.code());
}
