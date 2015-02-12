//
//  AudioManager.cpp
//  DK2.0
//
//  Created by luoxp on 14-9-28.
//
//

#include "AudioManager.h"
#include "SimpleAudioEngine.h"
#include "IncludeForCpp.h"


// android effect only support ogg
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#define EFFECT_SUF        ".ogg"
#elif( CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#define EFFECT_SUF        ".mp3"
#else
#define EFFECT_SUF        ".mp3"
#endif


using namespace CocosDenshion;
static AudioManager *_sharedAudioManager = NULL;

AudioManager* AudioManager::getInstance()
{
    if (! _sharedAudioManager)
    {
        _sharedAudioManager = new AudioManager();
    }
    
    return _sharedAudioManager;
}

void AudioManager::destroyInstance()
{
    CC_SAFE_DELETE(_sharedAudioManager);
}

AudioManager::AudioManager()
{
    _engine = SimpleAudioEngine::sharedEngine();
    _path = CCUserDefault::sharedUserDefault()->getStringForKey(SEARCH_PATH);
}


AudioManager::~AudioManager()
{
    _engine->end();
}


void AudioManager::playEffect(const string& filename)
{
#if TARGET_IPHONE_SIMULATOR == 0
    string name = _path + "/" +filename + EFFECT_SUF;
    _engine->playEffect(name.c_str());
#endif
}

