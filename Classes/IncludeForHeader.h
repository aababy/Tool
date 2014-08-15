//
//  IncludeForHeader.h
//  Tool
//
//  Created by luoxp on 14-7-30.
//
//

#ifndef Tool_IncludeForHeader_h
#define Tool_IncludeForHeader_h

#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include "cocos2d.h"
#include "cocos-ext.h"


using std::string;
using std::vector;
using std::swap;

USING_NS_CC;
using namespace cocos2d::extension;
using namespace ui;

#define FLAG_COUNT              (5)

struct FramesName
{
    string  sFrameName;
    int     iNumber;
};

enum setOperateType
{
    OT_NEW,
    OT_SELECT,
    OT_BROWSE,
};

enum curState
{
    CS_NONE,
    CS_TOTAL_PLIST,
    CS_MAIN_PLIST,
};

enum flagIndex
{
    FI_MOVE,
    FI_RANDOMPOSITION,
    FI_RANDOMCOUNT,
    FI_FOLLOW,
    FI_FIXED,
};


#endif
