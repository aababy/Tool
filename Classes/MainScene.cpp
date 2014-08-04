#include "MainScene.h"
#include "IncludeForCpp.h"

enum UITag
{
    FRAME_BG = 6,
    BUTTON_IMPORT = 8,
    BUTTON_PRV = 10,
    BUTTON_NEXT = 11,
    IMAGEVIEW_FRAME = 12,
    LABEL_FRAME_COUNT = 15,
    PREVIEW = 16,
    BUTTON_STOP = 17,
    BUTTON_PREVIEW = 18,
    BUTTON_NEW_PART = 19,
    PARTS_LIST = 20,
    ANCHOR_X = 33,
    ANCHOR_Y = 36,
    ROTATE = 38,
    SCALE = 42,
    SPRITE_HOLDER = 43,
    
    LIST_BG = 1000,
};


//1920x1080
//1920x864
//960x432
CCScene* MainScene::scene()
{
    CCScene* pScene = CCScene::create();
    MainScene* uiLayer = new MainScene();
    if (uiLayer && uiLayer->init(pScene))
    {
        uiLayer->autorelease();
        pScene->addChild(uiLayer);
    }
    else
    {
        CC_SAFE_DELETE(uiLayer);
    }
    return pScene;
}


bool MainScene::init(CCScene* pScene)
{
    m_scene = pScene;
    
    if (CCLayer::init())
    {      
        m_rootNode = (NodeReader::getInstance()->createNode("R/Tool_1.ExportJson"));
        addChild(m_rootNode);
        
        Layout *root = static_cast<Layout*>(m_rootNode->getChildren()->objectAtIndex(0));
        
        Button *btnImport = initButton(BUTTON_IMPORT, root, this, toucheventselector(MainScene::touchEvent));
        initButton(BUTTON_PRV, root, this, toucheventselector(MainScene::touchEvent));
        initButton(BUTTON_NEXT, root, this, toucheventselector(MainScene::touchEvent));
        initButton(BUTTON_PREVIEW, root, this, toucheventselector(MainScene::touchEvent));
        initButton(BUTTON_STOP, root, this, toucheventselector(MainScene::touchEvent));
        initButton(BUTTON_NEW_PART, root, this, toucheventselector(MainScene::touchEvent));
        
        //输入区
        m_ebAnchor[0] = InputBox::create(ANCHOR_X, root, this, m_rootNode);
        m_ebAnchor[1] = InputBox::create(ANCHOR_Y, root, this, m_rootNode);
        m_ebRotate = InputBox::create(ROTATE, root, this, m_rootNode);
        m_ebScale = InputBox::create(SCALE, root, this, m_rootNode);
        
        m_pHolder = static_cast<CCSprite*>(m_rootNode->getChildByTag(SPRITE_HOLDER));
        m_pHolder->initWithFile("R/cross.png");
        m_iFrameBG = (ImageView*)UIHelper::seekWidgetByTag(root, FRAME_BG);
        m_iFrameBG->addTouchEventListener(this, toucheventselector(MainScene::touchEvent));
        
        m_lFrameCount = (Label*)UIHelper::seekWidgetByTag(root, LABEL_FRAME_COUNT);
        m_preview = static_cast<CCSprite*>(m_rootNode->getChildByTag(PREVIEW));
        m_preview->initWithFile("R/cross.png");
        
        xPM->init(m_pHolder->getPosition(), m_preview->getPosition(), m_rootNode);
        
        //列表
        listView = (UIListView*)UIHelper::seekWidgetByTag(root, PARTS_LIST);
        CCNode *node = (NodeReader::getInstance()->createNode("R/ListCell.ExportJson"));
        listroot = static_cast<Layout*>(node->getChildren()->objectAtIndex(0));
        Layout* defaultItem = (Layout*)UIHelper::seekWidgetByTag(listroot, 32);         //还有转一层, 一来至少加2个Panel
        listView->setItemModel(defaultItem);
        
        
        xNotify->addObserver(this, callfuncO_selector(MainScene::updateProperty), UPDATE_PROPERTY, NULL);
        
        
        setTouchEnabled(true);
        
        //system("ls \"/Users/luoxp/ResearchProjects/tool/CocosStudio/Tool/Export\" > \"/Users/luoxp/ResearchProjects/temp.txt\"");
        
        return true;
    }
    
    return true;
}

MainScene::~MainScene()
{
    xNotify->removeAllObservers(this);
}


void MainScene::touchEvent(CCObject *pSender, TouchEventType type)
{
    if(type != TOUCH_EVENT_ENDED)
    {
        return;
    }
    
    Widget *widget = (Widget*)pSender;
    int iTag = widget->getTag();
    
    switch (iTag) {
        case BUTTON_IMPORT:
        {
            FolderBrowser *folder = FolderBrowser::create(this);
            m_scene->addChild(folder);
        }
            break;
        case BUTTON_NEW_PART:
        {
            xPM->addPart("monster_70_attack1.plist");
            updateList();
        }
            break;
        case BUTTON_NEXT:
        {
            xPM->nextFrame();
            setFrameCount();
        }
            break;
        case BUTTON_PRV:
        {
            xPM->preFrame();
            setFrameCount();
        }
            break;
        case BUTTON_PREVIEW:
        {
            xPM->preview();
        }
            break;
        case BUTTON_STOP:
        {
            m_preview->stopAllActions();
        }
            break;
        default:
        {
            if (iTag >= LIST_BG) {
                CCLOG("touch index %d", iTag - LIST_BG);
                xPM->setCurOperationIndex(iTag - LIST_BG);
            }
        }
            break;
    }
}

void MainScene::setFrameCount()
{
    if (xPM->m_iMainCount == 0) {
        return;
    }
    
    char szFrameCount[20];
    sprintf(szFrameCount, "%d/%d", xPM->m_iMainIndex + 1, xPM->m_iMainCount);
    string str = szFrameCount;
    
    m_lFrameCount->setText(str);
}

void MainScene::updateList()
{
    listView->removeAllItems();
    
    for (int i = 0; i < xPM->getPartsCount(); ++i)
    {
        listView->pushBackDefaultItem();
    }
    
    CCArray* items = listView->getItems();
    for (int i = 0; i < items->count(); i++) {
        Layout * bg = (Layout*)items->objectAtIndex(i);
        bg->addTouchEventListener(this, toucheventselector(MainScene::touchEvent));
        bg->setTag(LIST_BG + i);
        Label *label = (Label*)UIHelper::seekWidgetByTag(bg, 22);
        label->setText(xPM->getPartNameByIndex(i));
    }
}

void MainScene::updateProperty(CCObject *sender)
{
    if (xPM->getPartsCount() == 0) {
        return;
    }
    
    char buffer[20];
    
    const CCPoint point = xPM->getCurAnchorPoint();    
    sprintf(buffer, "%.1f", point.x);
    m_ebAnchor[0]->setText(string(buffer));
    sprintf(buffer, "%.1f", point.y);
    m_ebAnchor[1]->setText(string(buffer));
    
    sprintf(buffer, "%.1f", xPM->getCurRotate());
    m_ebRotate->setText(string(buffer));
    
    sprintf(buffer, "%.1f", xPM->getCurScale());
    m_ebScale->setText(string(buffer));
}

void MainScene::editBoxEditingDidBegin(CCEditBox* editBox)
{
}

void MainScene::editBoxEditingDidEnd(CCEditBox* editBox)
{
}

void MainScene::editBoxTextChanged(CCEditBox* editBox, const std::string& text)
{
    //只有changed有用
    if (text.length() > 2) {
        double temp = 0;
        
        CCPoint point;
        point.x = atof(m_ebAnchor[0]->getText());
        point.y = atof(m_ebAnchor[1]->getText());
        
        if (editBox == m_ebAnchor[0]->m_edit) {
            xPM->setCurAnchorPoint(point);
        } else if(editBox == m_ebAnchor[1]->m_edit) {
            xPM->setCurAnchorPoint(point);
        } else if(editBox == m_ebRotate->m_edit) {
            temp = atof(text.c_str());
            xPM->setCurRotate(temp);
        } else if(editBox == m_ebScale->m_edit) {
            temp = atof(text.c_str());
            xPM->setCurScale(temp);
        }
    }
}

void MainScene::editBoxReturn(CCEditBox* editBox)
{

}

void MainScene::registerWithTouchDispatcher()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority + 1, false);
}


bool MainScene::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    if (checkIfStartToDrag(pTouch->getLocation())) {
        m_oldPoint = pTouch->getLocation();
        m_bDragAndDrop = true;
    }
    else
    {
        m_bDragAndDrop = false;
    }
    
    return true;
}

void MainScene::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    if (m_bDragAndDrop) {
        CCPoint point = ccpSub(pTouch->getLocation(), m_oldPoint);
        xPM->setDragAndDropOffset(point);
        m_oldPoint = pTouch->getLocation();
    }
}

void MainScene::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    if (m_bDragAndDrop) {
        CCPoint point = ccpSub(m_oldPoint, pTouch->getLocation());
        xPM->setDragAndDropOffset(point);
    }
    
    m_bDragAndDrop = false;
}


bool MainScene::checkIfStartToDrag(CCPoint point)
{
    if (xPM->getPartsCount() == 0) {
        return false;
    }
    
    CCPoint origin = m_iFrameBG->convertToWorldSpace(CCPointZero);
    CCRect area = CCRect(origin.x - m_iFrameBG->getSize().width/2, origin.y - m_iFrameBG->getSize().height/2, m_iFrameBG->getSize().width, m_iFrameBG->getSize().height);
    if (point.x >= area.origin.x && point.x <= area.origin.x + area.size.width && point.y >= area.origin.y && point.y <= area.origin.y + area.size.height)
    {
        return true;
    }
    else
    {
        return false;
    }
}


void MainScene::importFinish(string &str)
{
    xPM->importPart(str.c_str());
    setFrameCount();
    updateList();
}








