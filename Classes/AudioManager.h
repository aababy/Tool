//
//  AudioManager.h
//  DK2.0
//
//  Created by luoxp on 14-9-28.
//
//  后面改成PlayerManager?

#ifndef __DK2_0__AudioManager__
#define __DK2_0__AudioManager__

#include "IncludeForHeader.h"

#define xAudio                          (AudioManager::getInstance())

namespace CocosDenshion {
    class SimpleAudioEngine;
}

class AudioManager {

public:
    static AudioManager* getInstance();
    static void destroyInstance();
    ~AudioManager();
    
    void playEffect(const string& filename);
    
    
private:
    AudioManager();
    
private:
    CocosDenshion::SimpleAudioEngine * _engine;
    string _path;
};


#endif /* defined(__DK2_0__AudioManager__) */
