//
//  FolderBrowser.h
//  Tool
//
//  Created by luoxp on 14-8-4.
//
//

#ifndef __Tool__FolderBrowser__
#define __Tool__FolderBrowser__

#include "IncludeForHeader.h"

class MainScene;

class FolderBrowser : public CCLayer, public CCEditBoxDelegate
{
public:
    FolderBrowser(MainScene *mainlayer);
    static FolderBrowser* create(MainScene *mainlayer);
    bool init();
    void touchEvent(CCObject *pSender, TouchEventType type);
    void updateList();
    void forward();
    
    //editbox
    virtual void editBoxEditingDidBegin(CCEditBox* editBox);
    virtual void editBoxEditingDidEnd(CCEditBox* editBox);
    virtual void editBoxTextChanged(CCEditBox* editBox, const std::string& text);
    virtual void editBoxReturn(CCEditBox* editBox);
    
//    void registerWithTouchDispatcher();
//    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    void checkOldSearchPath();
    
private:
    bool checkIfPlist(string &str);
    
    CCNode *m_rootNode;
    InputBox*   m_ebAddress;
    UIListView * listView;
    
    char    address[200];
    char    fullPathName[200];
    
    vector<string> m_vFileName;
    MainScene *m_mainlayer;
    string  searchPath;
};


#endif /* defined(__Tool__FolderBrowser__) */
