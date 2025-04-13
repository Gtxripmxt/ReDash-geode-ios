#include <Geode/Geode.hpp>
#include <Geode/modify/GameLevelManager.hpp>
#include <Macros.hpp>

#include "../ui/timely/RDTimelyNode.hpp"
#include "../ui/timely/RDDailyNode.hpp"
#include "../ui/timely/RDWeeklyNode.hpp"
#include "../ui/timely/RDEventNode.hpp"
#include "../ui/RDButton.hpp"

using namespace geode::prelude;

class $modify(MyGLM, GameLevelManager) {
    // Add missing member variables
    int m_dailyIDUnk = 0;
    int m_weeklyIDUnk = 0;
    int m_eventIDUnk = 0;

    void updateDailyTimer() {
        Variables::DailyLeft--;
        if (Variables::DailyLeft < 0) {
            CCScheduler::get()->unscheduleSelector(schedule_selector(MyGLM::updateDailyTimer), this);
            // Replace deprecated method with manual level re-download
            this->downloadLevel(-1, false);
        }
    }

    void updateWeeklyTimer() {
        Variables::WeeklyLeft--;
        if (Variables::WeeklyLeft < 1) {
            CCScheduler::get()->unscheduleSelector(schedule_selector(MyGLM::updateWeeklyTimer), this);
            this->downloadLevel(-2, false);
        }
    }

    void updateEventTimer() {
        Variables::EventLeft--;
        if (auto scene = CCDirector::sharedDirector()->getRunningScene()) {
            if (auto layer = scene->getChildByType<MenuLayer>(0)) {
                if (Variables::EventLeft < 1 && Mod::get()->getSettingValue<bool>("events-check")) {
                    CCScheduler::get()->unscheduleSelector(schedule_selector(MyGLM::updateEventTimer), this);
                    this->downloadLevel(-3, false);
                }
            }
        }
    }

    void processOnDownloadLevelCompleted(gd::string response, gd::string tag, bool p2) {
        GameLevelManager::processOnDownloadLevelCompleted(response, tag, p2);

        if (response != "-1") {
            if (tag == "-1_0") m_dailyIDUnk = m_dailyID;
            else if (tag == "-2_0") m_weeklyIDUnk = m_weeklyID;
            else if (tag == "-3_0") m_eventIDUnk = m_eventID;
        }

        if (auto layer = CCDirector::sharedDirector()->getRunningScene()->getChildByType<MenuLayer>(0)) {
            if (tag == "-1_0") {
                RD_HANDLE_LEVEL("daily-node", m_dailyIDUnk);
            } else if (tag == "-2_0") {
                RD_HANDLE_LEVEL("weekly-node", m_weeklyIDUnk);
            } else if (tag == "-3_0") {
                RD_HANDLE_LEVEL("event-node", m_eventIDUnk);
            }
        }
    }

    void onGetGJDailyLevelStateCompleted(gd::string response, gd::string tag) {
        GameLevelManager::onGetGJDailyLevelStateCompleted(response, tag);

        if (response != "-1") {
            auto responseStd = std::string(response.c_str());
            auto pos = responseStd.find('|') + 1;
            if (pos >= responseStd.size()) return;

            int timeLeft = std::stoi(responseStd.substr(pos));
            
            if (tag == "daily_state") {
                if (m_dailyIDUnk == 0) this->downloadLevel(-1, false);
                Variables::DailyLeft = timeLeft;
                CCScheduler::get()->scheduleSelector(schedule_selector(MyGLM::updateDailyTimer), this, 1.f, false);
                RD_HANDLE_GET_DAILY(RDDailyNode, "daily-node", m_dailyID, m_dailyIDUnk);
            } else if (tag == "weekly_state") {
                if (m_weeklyIDUnk == 0) this->downloadLevel(-2, false);
                Variables::WeeklyLeft = timeLeft;
                CCScheduler::get()->scheduleSelector(schedule_selector(MyGLM::updateWeeklyTimer), this, 1.f, false);
                RD_HANDLE_GET_DAILY(RDWeeklyNode, "weekly-node", m_weeklyID, m_weeklyIDUnk);
            } else if (tag == "event_state") {
                if (m_eventIDUnk == 0) this->downloadLevel(-3, false);
                Variables::EventLeft = timeLeft;
                CCScheduler::get()->scheduleSelector(schedule_selector(MyGLM::updateEventTimer), this, 1.f, false);

                if (auto layer = CCDirector::sharedDirector()->getRunningScene()->getChildByType<MenuLayer>(0)) {
                    if (auto node = typeinfo_cast<RDEventNode*>(layer->getChildByID("redash-menu"_spr)->getChildByID("dailies-menu"_spr)->getChildByID("event-node"))) {
                        if (node->m_skipButton) {
                            if (m_eventIDUnk < m_eventID && (node->m_currentLevel->m_normalPercent < 100 || GameStatsManager::sharedState()->hasCompletedDailyLevel(m_eventIDUnk))) {
                                node->m_skipButton->setVisible(true);
                            } else {
                                node->m_skipButton->setVisible(false);
                            }
                        }
                    }
                }
            }
        }
    }

    // Removed deprecated/challenge methods entirely to avoid linker issues.
};
