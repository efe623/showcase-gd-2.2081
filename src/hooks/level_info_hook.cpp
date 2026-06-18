#include "../includes/geode.hpp"
#include "../includes/task_poll.hpp"
#include "../managers/api_manager.hpp"
#include "../managers/interferences_manager.hpp"
#include "../managers/replay_manager.hpp"
#include "../tasks/get_level_replay_status.hpp"
#include "../tasks/get_replay.hpp"
#include "Geode/loader/Loader.hpp"
#include <optional>

struct SBLevelInfoLayer : geode::Modify<SBLevelInfoLayer, LevelInfoLayer> {
  struct Fields {
    GetLevelReplayStatusTask m_getLevelReplayStatusTask;
    GetReplayTask m_getReplayTask;
    CCMenuItemSpriteExtra *m_clapperBtn;
    std::optional<LevelReplayStatus> m_levelReplayStatus;
    std::optional<ShowcaseBotReplay> m_replay;
  };

  bool init(GJGameLevel *level, bool challenge) {
    if (!LevelInfoLayer::init(level, challenge)) {
      return false;
    }

    NodeIDs::provideFor(this);

    auto playMenu = static_cast<CCMenu *>(this->getChildByID("play-menu"));

    if (playMenu == nullptr) {
      log::debug("play menu not found!");
      return true;
    }

    APIManager::get().tryUpload();

    auto sprite = CircleButtonSprite::createWithSprite("GJ_playBtn2_001.png");

    sprite->setScale(0.5f);

    auto btn = CCMenuItemSpriteExtra::create(
        sprite, this, menu_selector(SBLevelInfoLayer::onButton));
    m_fields->m_clapperBtn = btn;
    btn->setID("play-replay-button");
    btn->setPosition({27.f, 27.f});
    btn->setZOrder(-2);
    btn->setVisible(false);
    CCMenu *menu = CCMenu::create();
    menu->setTouchPriority(playMenu->getTouchPriority() - 1);
    menu->setPosition(playMenu->getPosition());
    menu->setContentSize(playMenu->getContentSize());
    menu->setAnchorPoint(playMenu->getAnchorPoint());
    menu->setZOrder(playMenu->getZOrder() + 1);
    menu->addChild(btn);
    this->addChild(menu);

    bool eligible = InterferencesManager::isLevelEligible(m_level);

    if (eligible) {
      m_fields->m_getLevelReplayStatusTask = getLevelReplayStatus({
          m_level->m_levelID,
          m_level->m_levelVersion,
          GEODE_GD_VERSION_STRING,
          Mod::get()->getVersion().toVString(),
          APIManager::get().getDashAuthToken(),
      });
      pollTask(m_fields->m_getLevelReplayStatusTask,
               [this](GetLevelReplayStatusTask::Value *result) {
                 if (result->isErr())
                   return;
                 m_fields->m_levelReplayStatus = result->ok();
                 updateButtonVisibility();
               });
    }

    return true;
  }

  void updateButtonVisibility() {
    if (m_fields->m_levelReplayStatus &&
        m_fields->m_levelReplayStatus->replayID) {
      m_fields->m_clapperBtn->setVisible(true);
    }
  }

  void onButton(CCObject *sender) {
    if (!m_fields->m_levelReplayStatus ||
        !m_fields->m_levelReplayStatus->replayID.has_value())
      return;

    if (m_fields->m_replay.has_value()) {
      playReplay();
    }

    m_fields->m_getReplayTask =
        getReplay(GetReplayInput{m_fields->m_levelReplayStatus->replayID.value()});
    pollTask(m_fields->m_getReplayTask, [this](GetReplayTask::Value *result) {
      if (result->isErr())
        return;
      m_fields->m_replay = result->unwrap();
      updateButtonVisibility();
      playReplay();
    });
  }

  void playReplay() {
    if (!m_fields->m_replay.has_value()) {
      return;
    }
    ReplayManager::get().startReplay(m_fields->m_replay.value());
    this->onPlay(nullptr);
  }

  void onBack(CCObject *sender) {
    ReplayManager::get().stopReplay();
    LevelInfoLayer::onBack(sender);
  }
};
