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
    static FolderBrowser* create(MainScene *mainlayer, curState state);
    bool init(curState state);
    void touchEvent(CCObject *pSender, TouchEventType type);
    void updateList();
    void updateSkillList();
    void forward();
    void forward(string &key);
    
    //editbox
    virtual void editBoxEditingDidBegin(CCEditBox* editBox);
    virtual void editBoxEditingDidEnd(CCEditBox* editBox);
    virtual void editBoxTextChanged(CCEditBox* editBox, const std::string& text);
    virtual void editBoxReturn(CCEditBox* editBox);
    
    void checkOldSearchPath();
    void handleString(string &str);
    bool isStringInMulti(string &str);
    bool checkIfInSkill(string &str);
    void finish();
    
private:
    bool m_bShowSaveAddress = false;
    
    bool checkIfPlist(string &str);
    void prepareTotalPList();
    
    CCNode *m_rootNode;
    InputBox*   m_ebAddress;
    InputBox*   m_ebSearch;
    InputBox*   m_ebModifySave;         //修改保存路径
    UIListView * listView;
    UIListView * listSkill;
    Button*     m_btnModify;
    Label*      m_tips;
    curState    m_state;
    
    char    address[200];
    char    fullPathName[200];
    
    vector<string> m_vFileName;
    vector<string> m_vMultiName;
    vector<SkillName> m_vSkillName;
    MainScene *m_mainlayer;
    string  searchPath;
    string  savePath;
    string  sTotalPlist;

    
    CheckBox* m_cbMulti;
};


#endif /* defined(__Tool__FolderBrowser__) */
