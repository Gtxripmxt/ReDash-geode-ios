#include <Geode/Geode.hpp>
#include <Geode/modify/GameLevelManager.hpp>

#include "../ui/timely/RDTimelyNode.hpp"
#include "../ui/timely/RDDailyNode.hpp"
#include "../ui/timely/RDWeeklyNode.hpp"
#include "../ui/timely/RDEventNode.hpp"
#include "../ui/RDButton.hpp"

using namespace geode::prelude;

class $modify(MyGLM, GameLevelManager) {
    struct Fields {
        int m_dailyIDUnk = 0;
        int m_weeklyIDUnk = 0;
        int m_eventIDUnk = 0;
    };

    void updateDailyTimer() {
        Variables::DailyLeft--;
        if (Variables::DailyLeft < 0) {
            CCScheduler::get()->unscheduleSelector(schedule_selector(MyGLM::updateDailyTimer), this);
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
            if (tag == "-1_0") m_fields->m_dailyIDUnk = m_dailyID;
            else if (tag == "-2_0") m_fields->m_weeklyIDUnk = m_weeklyID;
            else if (tag == "-3_0") m_fields->m_eventIDUnk = m_eventID;
        }

        if (auto layer = CCDirector::sharedDirector()->getRunningScene()->getChildByType<MenuLayer>(0)) {
            if (tag == "-1_0") {
                RD_HANDLE_LEVEL("daily-node", m_fields->m_dailyIDUnk);
            } else if (tag == "-2_0") {
                RD_HANDLE_LEVEL("weekly-node", m_fields->m_weeklyIDUnk);
            } else if (tag == "-3_0") {
                RD_HANDLE_LEVEL("event-node", m_fields->m_eventIDUnk);
            }
        }
    }
};

    // Removed deprecated/challenge methods entirely to avoid linker issues.
};
