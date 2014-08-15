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
    
    LIST_FILE = 1000,
};

using namespace std;

FolderBrowser* FolderBrowser::create(MainScene *mainlayer)
{
    FolderBrowser* uiLayer = new FolderBrowser(mainlayer);
    if (uiLayer && uiLayer->init())
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


bool FolderBrowser::init()
{
    if (CCLayer::init())
    {
        m_rootNode = (NodeReader::getInstance()->createNode("R/FolderBrower.ExportJson"));
        addChild(m_rootNode);
        
        Layout *root = static_cast<Layout*>(m_rootNode->getChildren()->objectAtIndex(0));
        
        initButton(BUTTON_FORWARD, root, this, toucheventselector(FolderBrowser::touchEvent));
        initButton(BUTTON_BACK, root, this, toucheventselector(FolderBrowser::touchEvent));
        initButton(BUTTON_OK, root, this, toucheventselector(FolderBrowser::touchEvent));
        m_btnModify = initButton(BUTTON_MODIFY_SAVE, root, this, toucheventselector(FolderBrowser::touchEvent));
        
        //输入区
        m_ebAddress = InputBox::create(ADDRESS, root, this, m_rootNode);
        m_ebModifySave = InputBox::create(SAVE_ADDRESS, root, this, m_rootNode);
        m_ebModifySave->setVisible(false);
        
        m_ebSearch = InputBox::create(SEARCH_EDIT, root, this, m_rootNode);
        initButton(BUTTON_SEARCH, root, this, toucheventselector(FolderBrowser::touchEvent));
        
        //列表
        listView = (UIListView*)UIHelper::seekWidgetByTag(root, FILE_LIST);
        CCNode *node = (NodeReader::getInstance()->createNode("R/ListCell.ExportJson"));
        Layout *listroot = static_cast<Layout*>(node->getChildren()->objectAtIndex(0));
        Layout* defaultItem = (Layout*)UIHelper::seekWidgetByTag(listroot, 32);         //还有转一层, 一来至少加2个Panel
        listView->setItemModel(defaultItem);
        
        //checkbox
        m_cbMulti = (CheckBox*)UIHelper::seekWidgetByTag(root, CHECK_MULTI);
        
        setTouchEnabled(true);
        
        checkOldSearchPath();
       
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
            m_mainlayer->importFinish(m_vMultiName);
            CCNode* parent = this->getParent();
            parent->removeChild(this);
            m_mainlayer->switchToMain();
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
            if (iTag >= LIST_FILE) {
                
                string fileName = m_vFileName.at(iTag - LIST_FILE);
                CCLOG("touch index %d", iTag - LIST_FILE);
                
                if (m_cbMulti->getSelectedState() == false)
                {
                    m_vMultiName.clear();
                    m_vMultiName.push_back(fileName);
                    
                    m_mainlayer->importFinish(m_vMultiName);
                    CCNode* parent = this->getParent();
                    parent->removeChild(this);
                    m_mainlayer->switchToMain();
                }
                else
                {
                    handleString(fileName);
                    updateList();
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



