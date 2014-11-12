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
    void onAlternList(CCObject *pSender, TouchEventType type);
    void onPlayList(CCObject *pSender, TouchEventType type);
    void prepareTotalPList();
    void parserAction(CCDictionary * plist, const string &name);
    void insertPlay(const string &sSkillName);
    void makeAFocusOfList();
    void preview();

private:
    void updateAltern(vector<string> &vFileName);
    void forward(const string &key);
    void checkOldSearchPath();
    bool checkIfPlist(const string &str);


    CCNode*     m_rootNode;
    UIListView * m_listAltern;
    UIListView * m_listPlay;
    
    char    address[200];
    char    fullPathName[200];
    
    vector<string> m_vFileName;
    vector<string> m_vSkillName;
    vector<string> m_vSkillNameForPreview;
    vector<FramesName> m_vMotionName;
    string  searchPath;
    string  savePath;
    string  sTotalPlist;

    InputBox *m_ebAddress;
    InputBox *m_ebMain;
    InputBox *m_ebEffect;
    ImageView * m_iPreviewBG;
    ImageView * m_iPreviewBG1;

    previewState state;

    MainScene * m_mainLayer;
    bool bAllReady = false;
};


#endif /* defined(__Tool__FolderBrowser__) */
