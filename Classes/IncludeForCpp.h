//
//  IncludeForCpp.h
//  Tool
//
//  Created by luoxp on 14-7-30.
//
//

#ifndef Tool_IncludeForCpp_h
#define Tool_IncludeForCpp_h

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include "cocos-ext.h"

#include "MainScene.h"
#include "Part.h"
#include "Helper.h"
#include "PartManager.h"
#include "InputBox.h"
#include "FolderBrowser.h"

using namespace cocostudio::timeline;

#define xNotify                         (CCNotificationCenter::sharedNotificationCenter())
#define xData                           (CCUserDefault::sharedUserDefault())

#define UPDATE_PROPERTY                 "UPDATE_PROPERTY"
#define SEARCH_PATH                     "SEARCH_PATH"
#define SAVE_PATH                       "SAVE_PATH"

#endif
