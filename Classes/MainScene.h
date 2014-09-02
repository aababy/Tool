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

class InputBox;

class MainScene : public CCLayer, public CCEditBoxDelegate
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init(CCScene* pScene);  
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
    // implement the "static node()" method manually
    static CCScene* scene();
    
    void touchEvent(CCObject *pSender, TouchEventType type);
	void updateList(CCObject *sender);
	void updateMotionList(CCObject *sender);
    void updateProperty(CCObject *sender);
    void dragAndDrop(CCObject *pSender, TouchEventType type);
    void importFinish(vector<string> &vec);
    
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
    void switchToMain();
    void makeAFocusOfList();
    void makeAFocusOfListForMotion();    
    void updateButtonForMotion(CCObject *sender);
    void setTotalPlist(string &str);
    
    void selectedStateEvent(CCObject *pSender, CheckBoxEventType type);
    int tag2Index(int aTag);
    void updateCheckBox();
    void selectedNormal(CCObject *pSender, CheckBoxEventType type);
    void selectedAttackFrame(CCObject *pSender, CheckBoxEventType type);
    void importOldPlist(string &str);
    
protected:
    void setFrameCount(CCObject *sender);
    ~MainScene();
    void switchToBroswer();

    
    CCScene *m_scene;
    
    Layout *m_pWidget;
    CCNode *m_rootNode;
    ImageView *m_iFrameBG;
    Label *m_lFrameCount;
    
    CCSprite * m_preview;
    CCSprite * m_pHolder;
    
    UIListView * listView;
    UIListView * motionlist;
    Layout *listroot;
    
    Button *btnPre5;
    Button *btnNext5;
    Button *btnAddMotion;
    Button *btnDelMotion;
    Button *btnImport;
    Button *btn2x;
       
    InputBox*   m_ebAnchor[2];
    InputBox*   m_ebPosition[2];
    InputBox*   m_ebRotate;
    InputBox*   m_ebScale;
    InputBox*   m_ebDelay;
    InputBox*   m_ebName;
    InputBox*   m_ebDegree;
    InputBox*   m_ebSpeed;
    InputBox*   m_ebAttackDuration;
    InputBox*   m_ebAttackInterval;
    
    CCPoint     m_oldPoint;
    bool        m_bDragAndDrop = false;
    bool        m_bInOtherLayer = false;
    
    curState    m_state = CS_NONE;
    
    CheckBox*   m_cbFlags[FLAG_COUNT];
    CheckBox*   m_cbNormal;
    CheckBox*   m_cbAttack;
};

#endif // __HELLOWORLD_SCENE_H__
