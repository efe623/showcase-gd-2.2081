#pragma once

#include "geode.hpp"
#include <functional>
#include <memory>

template <class TaskT, class Callback>
void pollTask(TaskT task, Callback&& callback) {
  struct State {
    TaskT task;
    std::function<void(typename TaskT::Value*)> callback;
    std::function<void()> tick;
  };

  auto state = std::make_shared<State>();
  state->task = task;
  state->callback = std::forward<Callback>(callback);
  state->tick = [state] {
    if (auto value = state->task.getFinishedValue()) {
      state->callback(value);
      return;
    }

    if (state->task.isCancelled() || state->task.isNull()) {
      return;
    }

    queueInMainThread(state->tick);
  };

  queueInMainThread(state->tick);
}
