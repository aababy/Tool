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
    BUTTON_CLEAN = 19,
    PARTS_LIST = 20,
    POSITION_X = 33,
    POSITION_Y = 36,
    ROTATE = 38,
    SCALE = 42,
    SPRITE_HOLDER = 43,
    ANCHOR_X = 65,
    ANCHOR_Y = 67,
    BUTTON_SAVE = 82,
    MOTION_LIST = 99,
    BUTTON_ADD_MOTION = 100,
    BUTTON_NEXT_5 = 101,
    BUTTON_PRV_5 = 102,
    BUTTON_DEL_MOTION = 103,
    DELAY = 105,
    
    
    LIST_BG = 1000,
    LIST_MOTION = 1100,
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
        
        initButton(BUTTON_IMPORT, root, this, toucheventselector(MainScene::touchEvent));
        initButton(BUTTON_PRV, root, this, toucheventselector(MainScene::touchEvent));
        initButton(BUTTON_NEXT, root, this, toucheventselector(MainScene::touchEvent));
        initButton(BUTTON_PREVIEW, root, this, toucheventselector(MainScene::touchEvent));
        initButton(BUTTON_STOP, root, this, toucheventselector(MainScene::touchEvent));
        initButton(BUTTON_CLEAN, root, this, toucheventselector(MainScene::touchEvent));
        initButton(BUTTON_SAVE, root, this, toucheventselector(MainScene::touchEvent));
        btnAddMotion = initButton(BUTTON_ADD_MOTION, root, this, toucheventselector(MainScene::touchEvent));
        btnAddMotion->setEnabled(false);
        btnDelMotion = initButton(BUTTON_DEL_MOTION, root, this, toucheventselector(MainScene::touchEvent));
        
        btnPre5 = initButton(BUTTON_PRV_5, root, this, toucheventselector(MainScene::touchEvent));
        btnNext5 = initButton(BUTTON_NEXT_5, root, this, toucheventselector(MainScene::touchEvent));
        
        //输入区
        m_ebAnchor[0] = InputBox::create(ANCHOR_X, root, this, m_rootNode);
        m_ebAnchor[1] = InputBox::create(ANCHOR_Y, root, this, m_rootNode);
        m_ebPosition[0] = InputBox::create(POSITION_X, root, this, m_rootNode);
        m_ebPosition[1] = InputBox::create(POSITION_Y, root, this, m_rootNode);
        m_ebRotate = InputBox::create(ROTATE, root, this, m_rootNode);
        m_ebScale = InputBox::create(SCALE, root, this, m_rootNode);
        m_ebDelay = InputBox::create(DELAY, root, this, m_rootNode);
        
        m_pHolder = static_cast<CCSprite*>(m_rootNode->getChildByTag(SPRITE_HOLDER));
        m_pHolder->initWithFile("R/cross.png");
        m_iFrameBG = (ImageView*)UIHelper::seekWidgetByTag(root, FRAME_BG);
        m_iFrameBG->addTouchEventListener(this, toucheventselector(MainScene::touchEvent));
        
        m_lFrameCount = (Label*)UIHelper::seekWidgetByTag(root, LABEL_FRAME_COUNT);
        m_preview = static_cast<CCSprite*>(m_rootNode->getChildByTag(PREVIEW));
        m_preview->initWithFile("R/cross.png");
        
        xSkill->init(m_pHolder->getPosition(), m_preview->getPosition(), m_rootNode);
        
        //列表
        listView = (UIListView*)UIHelper::seekWidgetByTag(root, PARTS_LIST);
        CCNode *node = (NodeReader::getInstance()->createNode("R/ListCell.ExportJson"));
        listroot = static_cast<Layout*>(node->getChildren()->objectAtIndex(0));
        Layout* defaultItem = (Layout*)UIHelper::seekWidgetByTag(listroot, 32);         //还有转一层, 一来至少加2个Panel
        listView->setItemModel(defaultItem);
        
        
        motionlist = (UIListView*)UIHelper::seekWidgetByTag(root, MOTION_LIST);
        motionlist->setItemModel(defaultItem);
        
        
        xNotify->addObserver(this, callfuncO_selector(MainScene::updateProperty), UPDATE_PROPERTY, NULL);
        xNotify->addObserver(this, callfuncO_selector(MainScene::updateButtonForMotion), UPDATE_MOTION, NULL);
        xNotify->addObserver(this, callfuncO_selector(MainScene::updateList), UPDATE_EFFECT_LIST, NULL);
        xNotify->addObserver(this, callfuncO_selector(MainScene::updateMotionList), UPDATE_MOTION_LIST, NULL);
        xNotify->addObserver(this, callfuncO_selector(MainScene::setFrameCount), UPDATE_ALL_INDEX, NULL);

        updateButtonForMotion(NULL);
        
        setTouchEnabled(true);
        
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
            FolderBrowser *browser = FolderBrowser::create(this);
            m_scene->addChild(browser);
            switchToBroswer();
        }
            break;
        case BUTTON_CLEAN:
        {
            xSkill->clear();
            updateList();
        }
            break;
        case BUTTON_NEXT:
        {
            xSkill->nextFrame(1);
        }
            break;
        case BUTTON_PRV:
        {
            xSkill->preFrame(1);
        }
            break;
        case BUTTON_PREVIEW:
        {
            xSkill->preview();
        }
            break;
        case BUTTON_STOP:
        {
            m_preview->stopAllActions();
        }
            break;
        case BUTTON_SAVE:
        {
            xSkill->save();
        }
            break;
        case BUTTON_NEXT_5:
        {
            xSkill->nextFrame(5);
        }
            break;
        case BUTTON_PRV_5:
        {
            xSkill->preFrame(5);
        }
            break;
        case BUTTON_ADD_MOTION:
        {
            xSkill->addMotion();
            updateList();
            updateMotionList();
        }
            break;
        case BUTTON_DEL_MOTION:
        {
            
        }
            break;
        default:
        {
            if (iTag >= LIST_BG && iTag < LIST_MOTION) {                //特效
                xCurAtk->setCurOperationIndex(iTag - LIST_BG);
            }
            else if(iTag >= LIST_MOTION)                                //Motion, 更改Motion会同样更新特效列表
            {
                xSkill->setCurAtkIndex(iTag - LIST_MOTION, OT_SELECT);
            }
        }
            break;
    }
}

void MainScene::setFrameCount(CCObject *sender)
{
    if (xSkill->getFrameCount() == 0) {
        return;
    }
    
    char szFrameCount[20];
    sprintf(szFrameCount, "%d/%d", xSkill->m_iCurIndex + 1, xSkill->getFrameCount());
    string str = szFrameCount;
    
    m_lFrameCount->setText(str);
    
    if (xSkill->checkCanAddMotion()) {
        btnAddMotion->setEnabled(true);
    }
    else
    {
        btnAddMotion->setEnabled(false);
    }
}

void MainScene::updateList()
{
    if (xCurAtk == NULL) {
        return;
    }
    
    listView->removeAllItems();
    
    for (int i = 0; i < xCurAtk->getPartsCount(); ++i)
    {
        listView->pushBackDefaultItem();
    }
    
    CCArray* items = listView->getItems();
    for (int i = 0; i < items->count(); i++) {
        Layout * bg = (Layout*)items->objectAtIndex(i);
        bg->addTouchEventListener(this, toucheventselector(MainScene::touchEvent));
        bg->setTag(LIST_BG + i);
        Label *label = (Label*)UIHelper::seekWidgetByTag(bg, 22);
        label->setText(xCurAtk->getPartNameByIndex(i));
    }
    
    makeAFocusOfList();
}

void MainScene::updateMotionList()
{
    motionlist->removeAllItems();
    
    for (int i = 0; i < xSkill->getMotionCount(); ++i)
    {
        motionlist->pushBackDefaultItem();
    }
    
    CCArray* items = motionlist->getItems();
    for (int i = 0; i < items->count(); i++) {
        Layout * bg = (Layout*)items->objectAtIndex(i);
        bg->addTouchEventListener(this, toucheventselector(MainScene::touchEvent));
        bg->setTag(LIST_MOTION + i);
        Label *label = (Label*)UIHelper::seekWidgetByTag(bg, 22);
        label->setText(xSkill->getMotionName(i));
    }
    
    makeAFocusOfListForMotion();
}

void MainScene::updateButtonForMotion(CCObject *sender)
{
//    if (xSkill->getFrameCount() != 0) {
//        btnPre5->setEnabled(true);
//        btnNext5->setEnabled(true);
//        btnDelMotion->setEnabled(true);
//    }
//    else
//    {
//        btnPre5->setEnabled(false);
//        btnNext5->setEnabled(false);
//        btnAddMotion->setEnabled(false);
//        btnDelMotion->setEnabled(false);
//    }
}

void MainScene::updateProperty(CCObject *sender)
{
    if (xCurAtk == NULL || xCurAtk->getPartsCount() == 0) {
        return;
    }
    
    char buffer[20];
    const CCPoint po = xCurAtk->getCurPosition();
    sprintf(buffer, "%.1f", po.x);
    m_ebPosition[0]->setText(string(buffer));
    sprintf(buffer, "%.1f", po.y);
    m_ebPosition[1]->setText(string(buffer));
    
    const CCPoint point = xCurAtk->getCurAnchorPoint();
    sprintf(buffer, "%.1f", point.x);
    m_ebAnchor[0]->setText(string(buffer));
    sprintf(buffer, "%.1f", point.y);
    m_ebAnchor[1]->setText(string(buffer));
    
    sprintf(buffer, "%.1f", xCurAtk->getCurRotate());
    m_ebRotate->setText(string(buffer));
    
    sprintf(buffer, "%.1f", xCurAtk->getCurScale());
    m_ebScale->setText(string(buffer));
    
    sprintf(buffer, "%.2f", xCurAtk->getDelay());
    m_ebDelay->setText(string(buffer));
}

void MainScene::editBoxEditingDidBegin(CCEditBox* editBox)
{
}

void MainScene::editBoxEditingDidEnd(CCEditBox* editBox)
{
}

void MainScene::editBoxTextChanged(CCEditBox* editBox, const std::string& text)
{
    if (xCurAtk == NULL) {
        return;
    }
    
    //只有changed有用
    if (text.length() > 2) {
        double temp = 0;
        
        CCPoint point;
        point.x = atof(m_ebAnchor[0]->getText());
        point.y = atof(m_ebAnchor[1]->getText());
        
        CCPoint po;
        po.x = atof(m_ebPosition[0]->getText());
        po.y = atof(m_ebPosition[1]->getText());
        
        if (editBox == m_ebAnchor[0]->m_edit) {
            xCurAtk->setCurAnchorPoint(point);
        } else if(editBox == m_ebAnchor[1]->m_edit) {
            xCurAtk->setCurAnchorPoint(point);
        } else if(editBox == m_ebPosition[0]->m_edit) {
            xCurAtk->setCurPosition(po);
        } else if(editBox == m_ebPosition[1]->m_edit) {
            xCurAtk->setCurPosition(po);
        } else if(editBox == m_ebRotate->m_edit) {
            temp = atof(text.c_str());
            xCurAtk->setCurRotate(temp);
        } else if(editBox == m_ebScale->m_edit) {
            temp = atof(text.c_str());
            xCurAtk->setCurScale(temp);
        } else if(editBox == m_ebDelay->m_edit) {
            temp = atof(text.c_str());
            xCurAtk->setDelay(temp);
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
        xSkill->setDragAndDropOffset(point);
        m_oldPoint = pTouch->getLocation();
    }
}

void MainScene::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    if (m_bDragAndDrop) {
        CCPoint point = ccpSub(m_oldPoint, pTouch->getLocation());
        xSkill->setDragAndDropOffset(point);
        updateProperty(NULL);
    }
    
    m_bDragAndDrop = false;
}


bool MainScene::checkIfStartToDrag(CCPoint point)
{
    if (xCurAtk == NULL || xCurAtk->getPartsCount() == 0 || m_bInOtherLayer) {
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
    xSkill->importPart(str.c_str());
    setFrameCount(NULL);
    updateList();
    updateButtonForMotion(NULL);
}

void MainScene::switchToBroswer()
{
    m_bInOtherLayer = true;
    m_ebAnchor[0]->setVisible(false);
    m_ebPosition[0]->setVisible(false);
    m_ebAnchor[1]->setVisible(false);
    m_ebPosition[1]->setVisible(false);
    m_ebRotate->setVisible(false);
    m_ebScale->setVisible(false);
    m_ebDelay->setVisible(false);
}

void MainScene::switchToMain()
{
    m_bInOtherLayer = false;
    m_ebAnchor[0]->setVisible(true);
    m_ebPosition[0]->setVisible(true);
    m_ebAnchor[1]->setVisible(true);
    m_ebPosition[1]->setVisible(true);
    m_ebRotate->setVisible(true);
    m_ebScale->setVisible(true);
    m_ebDelay->setVisible(true);
}

void MainScene::makeAFocusOfList()
{
    CCArray* items = listView->getItems();
    for (int i = 0; i < items->count(); i++) {
        Layout * bg = (Layout*)items->objectAtIndex(i);
        
        if (i == xCurAtk->getCurOperationIndex()) {
            bg->setBackGroundColorType(LAYOUT_COLOR_SOLID);
            bg->setBackGroundColor(ccBLACK);
        }
        else
        {
            bg->setBackGroundColorType(LAYOUT_COLOR_NONE);
        }
    }
}

void MainScene::makeAFocusOfListForMotion()
{
    CCArray* items = motionlist->getItems();
    for (int i = 0; i < items->count(); i++) {
        Layout * bg = (Layout*)items->objectAtIndex(i);
        
        if (i == xSkill->getCurAtkIndex()) {
            bg->setBackGroundColorType(LAYOUT_COLOR_SOLID);
            bg->setBackGroundColor(ccBLACK);
        }
        else
        {
            bg->setBackGroundColorType(LAYOUT_COLOR_NONE);
        }
    }
}



