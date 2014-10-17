//
//  FrameBrowser.h
//  Tool
//
//  Created by luoxp on 14-10-16.
//
//

#ifndef __Tool__FrameBrowser__
#define __Tool__FrameBrowser__

#include "IncludeForHeader.h"

class MainScene;

class FrameBrowser : public CCLayer
{
public:
    FrameBrowser();
    static FrameBrowser* create();

private:
    bool init();

    CCNode * m_rootNode;
    UIListView * _list;

    void touchEvent(CCObject *pSender, TouchEventType type);
    void removeVectorElementByIndex(vector<FramesName> vec, int index);

    void modifyFrame();
};



#endif /* defined(__Tool__FrameBrowser__) */
