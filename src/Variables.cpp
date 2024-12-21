#include "Variables.hpp"

int Variables::DailyLeft = 0;
int Variables::WeeklyLeft = 0;
int Variables::EventLeft = 0;

int Variables::GlobalRank = 0;
int Variables::OldStarsCount = 0;

cocos2d::SEL_MenuHandler Variables::BISelector = nullptr;
cocos2d::SEL_MenuHandler Variables::SupExSelector = nullptr;
cocos2d::SEL_MenuHandler Variables::GDDPSelector = nullptr;
cocos2d::SEL_MenuHandler Variables::RouletteSelector = nullptr;
cocos2d::SEL_MenuHandler Variables::G3DSelector = nullptr;

Ref<cocos2d::CCDictionary> Variables::ThumbnailsDict = CCDictionary::create();

bool Variables::didGetGJChallenge = false;
std::vector<ChallengesPage*> Variables::challengesPages = {};