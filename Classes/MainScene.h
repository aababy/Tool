#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "IncludeForHeader.h"

#define UI_SCENE_CREATE_FUNC(UIScene) \
public: \
static CCScene* scene() \
{ \
    CCScene* pScene = CCScene::create(); \
    UIScene* uiLayer = new UIScene(); \
    if (uiLayer && uiLayer->init()) \
    { \
    uiLayer->autorelease(); \
    pScene->addChild(uiLayer); \
    } \
    else \
    { \
    CC_SAFE_DELETE(uiLayer); \
    } \
    return pScene; \
}

class Part;
class InputBox;

class MainScene : public CCLayer, public CCEditBoxDelegate
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
    // implement the "static node()" method manually
    UI_SCENE_CREATE_FUNC(MainScene);
    
    void touchEvent(CCObject *pSender, TouchEventType type);
    void updateList();
    void updateProperty(CCObject *sender);
    void dragAndDrop(CCObject *pSender, TouchEventType type);
    
    //editbox
    virtual void editBoxEditingDidBegin(CCEditBox* editBox);
    virtual void editBoxEditingDidEnd(CCEditBox* editBox);
    virtual void editBoxTextChanged(CCEditBox* editBox, const std::string& text);
    virtual void editBoxReturn(CCEditBox* editBox);
    
    //drag and drop
    bool checkIfStartToDrag(CCPoint point);
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    void registerWithTouchDispatcher();
    
protected:
    void setFrameCount();
    ~MainScene();
    
    Layout *m_pWidget;
    CCNode *m_rootNode;
    ImageView *m_iFrameBG;
    Label *m_lFrameCount;
    
    CCSprite * m_preview;
    CCSprite * m_pHolder;
    
    UIListView * listView;
    Layout *listroot;
       
    InputBox*   m_ebAnchor[2];
    InputBox*   m_ebRotate;
    InputBox*   m_ebScale;
    
    //暂时写这里
    Part  *m_curPart = NULL;
    
    CCPoint     m_oldPoint;
    bool        m_bDragAndDrop;
};

#endif // __HELLOWORLD_SCENE_H__
