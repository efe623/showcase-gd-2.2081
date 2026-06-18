#include "managers/api_manager.hpp"
#include "managers/crash_log_manager.hpp"

$execute {
  CrashLogManager::get().init();
  APIManager::get().init();
}
