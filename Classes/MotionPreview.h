//  MotionPreview.h
//  Tool
//
//  Created by luoxp on 14-8-4.
//
//

#ifndef __Tool__MotionPreview__
#define __Tool__MotionPreview__

#include "IncludeForHeader.h"

class MainScene;

class MotionPreview : public CCLayer, public CCEditBoxDelegate
{
public:
    MotionPreview();
    static MotionPreview* create();
    bool init();
    void touchEvent(CCObject *pSender, TouchEventType type);

    //editbox
    virtual void editBoxEditingDidBegin(CCEditBox* editBox);
    virtual void editBoxEditingDidEnd(CCEditBox* editBox);
    virtual void editBoxTextChanged(CCEditBox* editBox, const std::string& text);
    virtual void editBoxReturn(CCEditBox* editBox);
    void onList(CCObject *pSender, TouchEventType type);
    void onPlayList(CCObject *pSender, TouchEventType type);
    void prepareTotalPList();
    void parserAction(CCDictionary * plist, const string &name);
    void insertPlay(const string &sSkillName);
    void makeAFocusOfList();

private:
    void updateAltern(vector<string> &vFileName);
    void forward(const string &key);
    void checkOldSearchPath();
    bool checkIfPlist(const string &str);

    CCNode*     m_rootNode;
    UIListView * m_listAltern;
    UIListView * m_listPlay;
    Button*     m_btnModify;
    Label*      m_tips;
    
    char    address[200];
    char    fullPathName[200];
    
    vector<string> m_vFileName;
    vector<string> m_vMultiName;
    vector<string> m_vSkillName;
    vector<FramesName> m_vMotionName;
    MainScene *m_mainlayer;
    string  searchPath;
    string  savePath;
    string  sTotalPlist;

    InputBox *m_ebAddress;

    previewState state;
};


#endif /* defined(__Tool__FolderBrowser__) */
