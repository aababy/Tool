//
//  Part.h
//  Tool
//
//  Created by luoxp on 14-7-30.
//
//

#ifndef __Tool__Part__
#define __Tool__Part__

#include "IncludeForHeader.h"


class Part : public CCObject{
    
public:
    //static Part * create(const char *pFileName, CCPoint &location, ImageView *parent);
    //origin 是主体所在的位置, 非主体就以这个位置来作为偏移坐标
    Part(vector<string> &vNames, CCPoint &show, CCPoint &origin, CCPoint &showForPreview, CCNode *parent, vector<string> vFrameName, string& sMotionName);
    Part(vector<string> &vNames, CCPoint &show, CCPoint &origin, CCPoint &showForPreview, CCNode *parent, CCNode *parentForPreview, int iAcc, string& sMotionName);
    void setStartFrameIndex(int iStart);
    int getStartFrameIndex();
    void setMain();
    
    vector<FramesName> m_vFrameName;
    
    const char * getCurFrameName();
    const char * getFrameCountStr();
    
    void setPosition(CCPoint &point);
    CCPoint getPosition();
    float getRotate();
    const CCPoint& getAnchorPoint();
    float getScale();
    void setRotate(float r);
    void setAnchorPoint(const CCPoint& point);
    void setScale(float s);
    void setDragAndDropOffset(CCPoint &point);
    void setAccIndex(int iAcc);
    
    void setFrame(int iIndexStartFromMain);
    void preview();
    void setEnabled(bool bEnabled);
    void setFlags(flagIndex index, bool bFlag);
    void setAllFlags(bool *bFlag);
    bool getFlags(flagIndex index);
    CCSprite* getSprite();
    CCSprite* getPreview();
    
    string getEffectName();
    
    int m_iCurIndex = 0;
    int m_iFrameCount = 0;
    
    string  sPartName;                      //显示的名字, 也影响effect
    
    int getCurFrameIndex();
    void checkIfNeedToStart(int iFrameIndex);
    bool saveEffectToDictionary(CCDictionary *effects, int iMotionStart);
    void setDelay(int idx, float delay);
    float getDelay(int idx);
    void setDegree(float degree);
    float getDegree();
    void setSpeed(float speed);
    float getSpeed();
    void setAtkFrame(int atkFrame, bool bChecked);
    bool getAtkFrame(int iAllIndex);
    void setAllAtkFrame(CCString *atkFrames, int iMotionStart);
    void saveNames(vector<string> &vNames);
    bool saveAttackFrame(CCDictionary *effect, int iMotionStart);
    void saveDelay(CCDictionary *effect);

    ~Part();
    
    bool m_bRunning = false;        //动画是否在播放中
private:
    void init(vector<string> &vNames);
    void makeAPartName();
    CCPoint getOffset();
    string makeMotionName();
    void actionDone();
    void update(float delta);
    string m_sMotionName;
    
    CCSprite * m_preview = NULL;           //用于预览
    
    CCSprite  *m_sprite = NULL;            //用于显示帧
    ImageView *m_iFrame = NULL;
    CCPoint  m_origin;
    CCPoint  m_showForPreview;
    CCNode  *m_parent;
    
    CCAnimate * m_pAction;
    
    int m_iCurFrameIndex;
    float m_fAccumulate = 0.f;
    

    bool m_bOnWait = false;         //是否在等待主体播放到开始的那一帧.
    bool m_bMain = false;           //是否是主体
    
    int     iStartFrameIndex = 0;   //这个也是相对的.
    bool    m_bEnabled;
    int     m_iAccIndex = 0;
    
    vector<float> m_vDelay;
    float   m_fDelay = 0.05f;        //帧间隔
    float   m_degree = 0.0f;
    float   m_speed = 1000.f;
    
    vector<int> m_vAtkFrame;
    CC_SYNTHESIZE(float, m_duration, Duration);
    CC_SYNTHESIZE(float, m_interval, Interval);
    
    bool    m_flag[FLAG_COUNT];
    
    vector<string> m_vNames;        //保存的名字, 多个请用逗号分开
    CCPoint posStart;               //原始位置
};


#endif /* defined(__Tool__Part__) */
