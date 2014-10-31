//
//  MotionPreview.cpp
//  Tool
//
//  Created by luoxp on 14-8-4.
//
//
#include "IncludeForCpp.h"
#include "MotionPreview.h"


enum UITag
{
    T_LIST_ALTERN = 4,
    T_LIST_PLAY = 448,
    INPUT_ADDRESS = 6,
    BUTTON_START = 8,
    BUTTON_DEL = 450,
};

using namespace std;

MotionPreview* MotionPreview::create()
{
    MotionPreview* uiLayer = new MotionPreview();
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


MotionPreview::MotionPreview()
{
    state = PS_NONE;
}


bool MotionPreview::init()
{
    if (CCLayer::init())
    {
        m_rootNode = (NodeReader::getInstance()->createNode("R/MotionPreview_1.ExportJson"));
        addChild(m_rootNode);
        
        Layout *root = static_cast<Layout*>(m_rootNode->getChildren()->objectAtIndex(1));
        
        initButton(BUTTON_START, root, this, toucheventselector(MotionPreview::touchEvent));
        initButton(BUTTON_DEL, root, this, toucheventselector(MotionPreview::touchEvent));

        //输入区
        m_ebAddress = InputBox::create(INPUT_ADDRESS, root, this, m_rootNode);

        m_listAltern = (UIListView*)UIHelper::seekWidgetByTag(root, T_LIST_ALTERN);
        m_listPlay = (UIListView*)UIHelper::seekWidgetByTag(root, T_LIST_PLAY);

        setTouchEnabled(true);
        checkOldSearchPath();

        m_iPreviewBG = ImageView::create();
        m_iPreviewBG->loadTexture("R/bg_1.jpg");
        m_iPreviewBG->setPosition(ccp(m_iPreviewBG->getContentSize().width / 2, m_iPreviewBG->getContentSize().height / 2 + 250));
        m_iPreviewBG1 = ImageView::create();
        m_iPreviewBG1->loadTexture("R/bg_1.jpg");
        m_iPreviewBG1->setPosition(ccp(m_iPreviewBG->getContentSize().width * 1.5, m_iPreviewBG->getContentSize().height / 2 + 250));

        Layout *root1 = static_cast<Layout*>(m_rootNode->getChildren()->objectAtIndex(0));

        root1->addChild(m_iPreviewBG, 0);
        root1->addChild(m_iPreviewBG1, 0);

        return true;
    }
    
    return true;
}


void MotionPreview::touchEvent(CCObject *pSender, TouchEventType type)
{
    if(type != TOUCH_EVENT_ENDED)
    {
        return;
    }
    
    Widget *widget = (Widget*)pSender;
    int iTag = widget->getTag();
    
    switch (iTag) {
        case BUTTON_DEL:
        {
            //删除列表中的值
            m_listPlay->removeItem(m_listPlay->getCurSelectedIndex());
        }
            break;

        default:
            break;
    }
}


void MotionPreview::onAlternList(CCObject *pSender, TouchEventType type)
{
    if(type != TOUCH_EVENT_ENDED)
    {
        return;
    }

    if(state == PS_TOTAL_PLIST)
    {
        state = PS_CHOOSE;
        sTotalPlist = m_vFileName.at(m_listAltern->getCurSelectedIndex());
        prepareTotalPList();
    }
    else
    {
        //每点击一个, 加到 m_listPlay
        insertPlay(m_vSkillName.at(m_listAltern->getCurSelectedIndex()));
    }
}


void MotionPreview::onPlayList(CCObject *pSender, TouchEventType type)
{
    if(type != TOUCH_EVENT_ENDED)
    {
        return;
    }

    makeAFocusOfList();
}


void MotionPreview::checkOldSearchPath()
{
    string str = CCUserDefault::sharedUserDefault()->getStringForKey(SEARCH_PATH);
    if (!str.empty()) {
        m_ebAddress->setText(str);

        //检查就的关键字
        string keyword = CCUserDefault::sharedUserDefault()->getStringForKey(SEARCH_KEYWORD);
        forward(keyword);
    }
}

void MotionPreview::forward(const string &key)
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
            //if (key.empty() || key.compare("") == 0 || strLine.find(key) != string::npos)
            {
                m_vFileName.push_back(strLine);
            }
        }
    }

    fin.close();

    if (m_vFileName.size() != 0) {
        CCFileUtils::sharedFileUtils()->addSearchPath(searchPath.c_str());
    }

    //3. 将vector的内容用列表显示出来.
    updateAltern(m_vFileName);

    //保存searchPath
    CCUserDefault::sharedUserDefault()->setStringForKey(SEARCH_PATH, searchPath);

    savePath = CCUserDefault::sharedUserDefault()->getStringForKey(SAVE_PATH);
    if(savePath.empty())
    {
        sprintf(address, "%s/save", searchPath.c_str());
        CCUserDefault::sharedUserDefault()->setStringForKey(SAVE_PATH, searchPath);
    }

    state = PS_TOTAL_PLIST;
}

bool MotionPreview::checkIfPlist(const string &str)
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
void MotionPreview::editBoxEditingDidBegin(CCEditBox* editBox)
{
}

void MotionPreview::editBoxEditingDidEnd(CCEditBox* editBox)
{
}

void MotionPreview::editBoxTextChanged(CCEditBox* editBox, const std::string& text)
{
}

void MotionPreview::editBoxReturn(CCEditBox* editBox)
{
}


void MotionPreview::updateAltern(vector<string> &vFileName)
{
    m_listAltern->removeAllItems();

    CCNode *node = (NodeReader::getInstance()->createNode("R/FolderBrowser_2.ExportJson"));
    Layout *listroot = static_cast<Layout*>(node->getChildren()->objectAtIndex(0));
    Layout* defaultItem = (Layout*)UIHelper::seekWidgetByTag(listroot, 32);

    for (int i = 0; i < vFileName.size(); i++) {
        Layout *panel = (Layout *)defaultItem->clone();
        panel->addTouchEventListener(this, toucheventselector(MotionPreview::onAlternList));

        Label *label = (Label*)UIHelper::seekWidgetByTag(panel, 22);
        label->setText(vFileName.at(i));

        m_listAltern->pushBackCustomItem(panel);
    }
}


void MotionPreview::prepareTotalPList()
{
    CCDictionary *plist = CCDictionary::createWithContentsOfFile(sTotalPlist.c_str());

    parserAction(plist, "atks");
    parserAction(plist, "normals");
    parserAction(plist, "join");


    //先排下序
    bubble_sort(m_vMotionName);
    for(int i = 0; i < m_vMotionName.size(); i++)
    {
        m_vSkillName.push_back(m_vMotionName.at(i).sFrameName);
    }

    updateAltern(m_vSkillName);
}


void MotionPreview::parserAction(CCDictionary * plist, const string &name)
{
    //创建motion, 先是atks
    CCDictionary *atks = (CCDictionary *)plist->objectForKey(name);
    if (atks != NULL)
    {
        CCArray *array = atks->allKeys();

        if (array != NULL)
        {
            for (int i = 0; i < array->count(); i ++)
            {
                //获取key的方法
                CCString *key =  (CCString*)array->objectAtIndex(i);
                FramesName frame;
                frame.sFrameName = key->getCString();
                frame.iNumber = getNumber(frame.sFrameName);
                m_vMotionName.push_back(frame);
            }
        }
    }
}

void MotionPreview::insertPlay(const string &sSkillName)
{
    CCNode *node = (NodeReader::getInstance()->createNode("R/FolderBrowser_2.ExportJson"));
    Layout *listroot = static_cast<Layout*>(node->getChildren()->objectAtIndex(0));
    Layout* defaultItem = (Layout*)UIHelper::seekWidgetByTag(listroot, 32);

    Layout *panel = (Layout *)defaultItem->clone();
    panel->addTouchEventListener(this, toucheventselector(MotionPreview::onPlayList));

    Label *label = (Label*)UIHelper::seekWidgetByTag(panel, 22);
    label->setText(sSkillName);

    m_listPlay->pushBackCustomItem(panel);
}


void MotionPreview::makeAFocusOfList()
{
    CCArray* items = m_listPlay->getItems();
    for (int i = 0; i < items->count(); i++) {
        Layout * bg = (Layout*)items->objectAtIndex(i);

        if (i == m_listPlay->getCurSelectedIndex()) {
            bg->setBackGroundColorType(LAYOUT_COLOR_SOLID);
            bg->setBackGroundColor(ccBLACK);
        }
        else
        {
            bg->setBackGroundColorType(LAYOUT_COLOR_NONE);
        }
    }
}


void MotionPreview::startPreview()
{
    //准备好所有浏览的Part
    m_vSkillNameForPreview.clear();
    CCArray * array = m_listPlay->getItems();

    CCObject *obj = NULL;
    CCARRAY_FOREACH(array, obj)
    {
        Layout *widget = (Layout*)obj;
        Label *label = (Label*)UIHelper::seekWidgetByTag(widget, 22);
        string str = label->getStringValue();

        m_vSkillNameForPreview.push_back(str);
    }

    xSkill->setMotionPreviewName(m_vSkillNameForPreview);

    //清除Skill,
    xSkill->clear();

    //重新导入所有Part
    xSkill->importOldPlist("", true);

    //将Part的指针按顺序放到 m_vMotionPreview 中
    xSkill->prepareMotionPreview();

    //开始preview

}