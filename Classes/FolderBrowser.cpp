//
//  FolderBrowser.cpp
//  Tool
//
//  Created by luoxp on 14-8-4.
//
//
#include "IncludeForCpp.h"
#include "FolderBrowser.h"


enum UITag
{
    ADDRESS = 59,
    BUTTON_FORWARD = 60,
    FILE_LIST = 61,
    BUTTON_BACK = 62,
    SAVE_ADDRESS = 83,
    BUTTON_MODIFY_SAVE = 84,
    SEARCH_EDIT = 126,
    BUTTON_SEARCH = 127,
    CHECK_MULTI = 172,
    BUTTON_OK = 188,
    TIPS = 190,
    SKILL_LIST = 311,
    
    LIST_FILE = 1000,
    
    LIST_SKILL = 2000,
};

using namespace std;

FolderBrowser* FolderBrowser::create(MainScene *mainlayer, curState state)
{
    FolderBrowser* uiLayer = new FolderBrowser(mainlayer);
    if (uiLayer && uiLayer->init(state))
    {
        uiLayer->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(uiLayer);
    }
    return uiLayer;
}


FolderBrowser::FolderBrowser(MainScene *mainlayer)
{
    m_mainlayer = mainlayer;
}


bool FolderBrowser::init(curState state)
{
    if (CCLayer::init())
    {
        m_rootNode = (NodeReader::getInstance()->createNode("R/FolderBrowser_1.ExportJson"));
        addChild(m_rootNode);
        
        Layout *root = static_cast<Layout*>(m_rootNode->getChildren()->objectAtIndex(0));
        
        initButton(BUTTON_FORWARD, root, this, toucheventselector(FolderBrowser::touchEvent));
        initButton(BUTTON_BACK, root, this, toucheventselector(FolderBrowser::touchEvent));
        initButton(BUTTON_OK, root, this, toucheventselector(FolderBrowser::touchEvent));
        m_btnModify = initButton(BUTTON_MODIFY_SAVE, root, this, toucheventselector(FolderBrowser::touchEvent));
        
        m_tips = (Label*)UIHelper::seekWidgetByTag(root, TIPS);
        
        m_state = state;
        
        switch (state) {
            case CS_NONE:
            {
                m_tips->setText(xStr("first_step"));
            }
                break;
            case CS_TOTAL_PLIST:
            {
                m_tips->setText(xStr("second_step"));
            }
                break;
            case CS_MAIN_PLIST:
            {
                m_tips->setVisible(false);
            }
                break;
            default:
                break;
        }
        
        
        //输入区
        m_ebAddress = InputBox::create(ADDRESS, root, this, m_rootNode);
        m_ebModifySave = InputBox::create(SAVE_ADDRESS, root, this, m_rootNode);
        m_ebModifySave->setVisible(false);
        
        m_ebSearch = InputBox::create(SEARCH_EDIT, root, this, m_rootNode);
        initButton(BUTTON_SEARCH, root, this, toucheventselector(FolderBrowser::touchEvent));
        
        //列表
        listView = (UIListView*)UIHelper::seekWidgetByTag(root, FILE_LIST);
        CCNode *node = (NodeReader::getInstance()->createNode("R/FolderBrowser_2.ExportJson"));
        Layout *listroot = static_cast<Layout*>(node->getChildren()->objectAtIndex(0));
        Layout* defaultItem = (Layout*)UIHelper::seekWidgetByTag(listroot, 32);         //还有转一层, 一来至少加2个Panel
        listView->setItemModel(defaultItem);
        
        //编辑列表
        listSkill = (UIListView*)UIHelper::seekWidgetByTag(root, SKILL_LIST);
        listSkill->setItemModel(defaultItem);
        
        //checkbox
        m_cbMulti = (CheckBox*)UIHelper::seekWidgetByTag(root, CHECK_MULTI);
        
        setTouchEnabled(true);
        
        checkOldSearchPath();
        
        SkillName skillname;
        skillname.s = xStr("new");
        skillname.flag = 1;
        
        m_vSkillName.push_back(skillname);
       
        return true;
    }
    
    return true;
}


void FolderBrowser::touchEvent(CCObject *pSender, TouchEventType type)
{
    if(type != TOUCH_EVENT_ENDED)
    {
        return;
    }
    
    Widget *widget = (Widget*)pSender;
    int iTag = widget->getTag();
    
    switch (iTag) {
        case BUTTON_FORWARD:
        {
            forward();
        }
            break;
        case BUTTON_BACK:
        {
            CCNode* parent = this->getParent();
            parent->removeChild(this);
            m_mainlayer->switchToMain();
        }
            break;
        case BUTTON_SEARCH:
        {
            string search = m_ebSearch->getText();
            forward(search);
        }
            break;
        case BUTTON_OK:
        {
            finish();
        }
            break;
        case BUTTON_MODIFY_SAVE:
        {
            if (m_bShowSaveAddress) {
                savePath = m_ebModifySave->getText();
                CCUserDefault::sharedUserDefault()->setStringForKey(SAVE_PATH, savePath);
                
                m_bShowSaveAddress = false;
                m_ebModifySave->setVisible(m_bShowSaveAddress);
                
                string str = "修改保存路径";
                m_btnModify->setTitleText(str);
            }
            else
            {
                savePath = CCUserDefault::sharedUserDefault()->getStringForKey(SAVE_PATH);
                                
                CCUserDefault::sharedUserDefault()->setStringForKey(SAVE_PATH, savePath);
                
                m_bShowSaveAddress = true;
                m_ebModifySave->setVisible(m_bShowSaveAddress);
                m_ebModifySave->setText(savePath);
                
                string str = "确认";
                m_btnModify->setTitleText(str);
            }
        }
            break;
        default:
        {
            if (iTag >= LIST_FILE && iTag < LIST_SKILL) {
                
                string fileName = m_vFileName.at(iTag - LIST_FILE);
                CCLOG("touch index %d", iTag - LIST_FILE);
                
                if (m_state == CS_NONE) {
                    m_state = CS_MAIN_PLIST;
                    m_mainlayer->setTotalPlist(fileName);
                    sTotalPlist = fileName;
                    
                    m_tips->setText(xStr("second_step"));
                    
                    prepareTotalPList();
                    updateSkillList();
                }
                else
                {
                    if (m_cbMulti->getSelectedState() == false)
                    {
                        m_vMultiName.clear();
                        m_vMultiName.push_back(fileName);
                        
                        finish();
                    }
                    else
                    {
                        handleString(fileName);
                        updateList();
                    }
                
                }
            }
            else
            {
                string fileName = m_vSkillName.at(iTag - LIST_SKILL).s;
                
                if (fileName.compare(xStr("new")) != 0)
                {
                    m_vMultiName.clear();
                    string2Vector(fileName, m_vMultiName);
                    
                    finish();
                    m_mainlayer->importOldPlist(fileName, m_vSkillName.at(iTag - LIST_SKILL).flag);
                }
                else
                {
                    //选导入主体
                    m_tips->setText(xStr("third_step"));
                }
            }
        }
            break;
    }
}

void FolderBrowser::forward()
{
    string str;
    forward(str);
}

void FolderBrowser::forward(string &key)
{
    m_vFileName.clear();
    
    //1. 将指定地址下的文件列出来, 存放到临时文件里面.
    const char * szAddress = m_ebAddress->getText();
    searchPath = szAddress;
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    sprintf(address, "dir \"%s\" /b > \"%s/temp.txt\"", szAddress, szAddress);
#else
    sprintf(address, "ls \"%s\" > \"%s/temp.txt\"", szAddress, szAddress);
#endif
    
    //使用shell/dos命令来操作
    system(address);
    
    //2. 将文件中的内容读出来, 存放到vector里面.
    sprintf(fullPathName, "%s/temp.txt", szAddress);
    //int lines = countLines(fullPathName);
    
    ifstream fin(fullPathName);
    string strLine;
    while(getline(fin,strLine))
    {
        if (checkIfPlist(strLine)) {
            
            //关键字搜索
            if (key.empty() || key.compare("") == 0 || strLine.find(key) != string::npos) {
                m_vFileName.push_back(strLine);
            }
        }
    }
   
    fin.close();
    
    if (m_vFileName.size() != 0) {
        CCFileUtils::sharedFileUtils()->addSearchPath(searchPath.c_str());
    }
    
    //3. 将vector的内容用列表显示出来.
    updateList();
    
    //保存searchPath
    CCUserDefault::sharedUserDefault()->setStringForKey(SEARCH_PATH, searchPath);
    
    
    savePath = CCUserDefault::sharedUserDefault()->getStringForKey(SAVE_PATH);
    if(savePath.empty())
    {
        sprintf(address, "%s/save", searchPath.c_str());
        CCUserDefault::sharedUserDefault()->setStringForKey(SAVE_PATH, searchPath);
    }    
}


void FolderBrowser::updateList()
{
    listView->removeAllItems();
    
    for (int i = 0; i < m_vFileName.size(); ++i)
    {
        listView->pushBackDefaultItem();
    }
    
    CCArray* items = listView->getItems();
    for (int i = 0; i < items->count(); i++) {
        Layout * bg = (Layout*)items->objectAtIndex(i);
        bg->addTouchEventListener(this, toucheventselector(FolderBrowser::touchEvent));
        bg->setTag(LIST_FILE + i);
        Label *label = (Label*)UIHelper::seekWidgetByTag(bg, 22);
        label->setText(m_vFileName.at(i));
        
        
        if (m_vMultiName.size() != 0) {
            if (isStringInMulti(m_vFileName.at(i)))
            {
                bg->setBackGroundColorType(LAYOUT_COLOR_SOLID);
                bg->setBackGroundColor(ccBLACK);
            }
        }
    }
}


void FolderBrowser::updateSkillList()
{
    listSkill->removeAllItems();
    
    for (int i = 0; i < m_vSkillName.size(); ++i)
    {
        listSkill->pushBackDefaultItem();
    }
    
    CCArray* items = listSkill->getItems();
    for (int i = 0; i < items->count(); i++) {
        Layout * bg = (Layout*)items->objectAtIndex(i);
        bg->addTouchEventListener(this, toucheventselector(FolderBrowser::touchEvent));
        bg->setTag(LIST_SKILL + i);
        Label *label = (Label*)UIHelper::seekWidgetByTag(bg, 22);
        label->setText(m_vSkillName.at(i).s);
    }
}


bool FolderBrowser::checkIfPlist(string &str)
{
    int iDot = str.rfind('.');
    if (iDot != -1) {
        string suffix = str.substr(iDot + 1, str.size() - iDot);
        
        if (suffix.compare("plist") == 0) {
            return true;
        }
    }
    
    return false;
}

//editbox
void FolderBrowser::editBoxEditingDidBegin(CCEditBox* editBox)
{
}

void FolderBrowser::editBoxEditingDidEnd(CCEditBox* editBox)
{
}

void FolderBrowser::editBoxTextChanged(CCEditBox* editBox, const std::string& text)
{
}

void FolderBrowser::editBoxReturn(CCEditBox* editBox)
{
}


void FolderBrowser::checkOldSearchPath()
{
    string str = CCUserDefault::sharedUserDefault()->getStringForKey(SEARCH_PATH);            
    if (!str.empty()) {
        m_ebAddress->setText(str);
        forward();
    }
}

void FolderBrowser::handleString(string &str)
{
    for (int i = 0; i < m_vMultiName.size(); i++) {
        
        if (m_vMultiName.at(i).compare(str) == 0) {
            
            //如果已经存在就删除
            vector<string>::iterator it = m_vMultiName.begin() + i;
            m_vMultiName.erase(it);
            return;
        }
    }
    
    //不存在, 就加入
    m_vMultiName.push_back(str);
}

bool FolderBrowser::isStringInMulti(string &str)
{
    for (int i = 0; i < m_vMultiName.size(); i++)
    {
        if (m_vMultiName.at(i).compare(str) == 0) {
            return true;
        }
    }
    
    return false;
}

void FolderBrowser::prepareTotalPList()
{
    CCDictionary *plist = CCDictionary::createWithContentsOfFile(sTotalPlist.c_str());
    
    //创建motion, 先是atks
    CCDictionary *atks = (CCDictionary *)plist->objectForKey("atks");
    if (atks != NULL)
    {
        CCArray *array = atks->allKeys();
        
        if (array != NULL)
        {
            for (int i = 0; i < array->count(); i ++)
            {
                //获取key的方法
                CCString *key =  (CCString*)array->objectAtIndex(i);
                CCDictionary* motionDic = (CCDictionary *)atks->objectForKey(key->getCString());
                CCString *fileName =  (CCString*)motionDic->objectForKey("fileName");
                string str = fileName->getCString();
                
                if (checkIfInSkill(str) == false) {
                    SkillName skill;
                    skill.s = str;
                    skill.flag = 1;
                    m_vSkillName.push_back(skill);
                }
            }
        }
    }
    
    CCDictionary *normals = (CCDictionary *)plist->objectForKey("normals");
    if (normals != NULL)
    {
        CCArray *array = normals->allKeys();
        if (array != NULL) {
            for (int i = 0; i < array->count(); i ++)
            {
                //获取key的方法
                CCString *key =  (CCString*)array->objectAtIndex(i);
                CCDictionary* motionDic = (CCDictionary *)normals->objectForKey(key->getCString());
                CCString *fileName =  (CCString*)motionDic->objectForKey("fileName");
                string str = fileName->getCString();
                
                if (checkIfInSkill(str) == false) {
                    SkillName skill;
                    skill.s = str;
                    skill.flag = 0;
                    m_vSkillName.push_back(skill);
                }
            }
        }
    }
    
    if (m_vSkillName.empty()) {
        m_tips->setText(xStr("second_step_new"));
    }
}

bool FolderBrowser::checkIfInSkill(string &str)
{
    for (int i = 0; i < m_vSkillName.size(); i++)
    {
        if (str.compare(m_vSkillName.at(i).s) == 0) {
            return true;
        }
    }
    
    return false;
}


void FolderBrowser::finish()
{
    m_mainlayer->importFinish(m_vMultiName);
    CCNode* parent = this->getParent();
    parent->removeChild(this);
    m_mainlayer->switchToMain();
}
