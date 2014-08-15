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
    Part(vector<string> &vNames, CCPoint &show, CCPoint &origin, CCPoint &showForPreview, CCNode *parent, vector<string> vFrameName);
    Part(vector<string> &vNames, CCPoint &show, CCPoint &origin, CCPoint &showForPreview, CCNode *parent, int iAcc);
    void setStartFrameIndex(int iStart);
    int getStartFrameIndex();
    void setMain();
    
    vector<FramesName> m_vFrameName;
    
    const char * getCurFrameName();
    const char * getNextFrameName();
    const char * getPreFrameName();
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
    
    void setFrame(int iIndexStartFromMain);
    void nextFrame(int iCount);
    void preFrame(int iCount);
    void preview();
    void setEnabled(bool bEnabled);
    
    string getEffectName();
    
    int m_iCurIndex = 0;
    int m_iFrameCount = 0;
    
    CCAnimation* getAnimation();            //这个函数是否分离出去?
    
    string  sPartName;
    
    int getCurFrameIndex();
    void checkIfNeedToStart(int iFrameIndex);
    void saveEffectToDictionary(CCDictionary *effects);
    void setDelay(float delay);
    float getDelay();

    ~Part();
    
    bool m_bRunning = false;        //动画是否在播放中
private:
    CCPoint getOffset();
    void actionDone();    
    
    CCSprite * m_preview = NULL;           //用于预览
    
    CCSprite  *m_sprite = NULL;            //用于显示帧
    ImageView *m_iFrame = NULL;
    CCPoint  m_origin;
    CCPoint  m_showForPreview;
    CCNode  *m_parent;
    
    CCAnimate * m_pAction;
    
    int m_iOldFrameIndex;
    

    bool m_bOnWait = false;         //是否在等待主体播放到开始的那一帧.
    bool m_bMain = false;           //是否是主体
    
    int     iStartFrameIndex = 0;   //这个也是相对的.
    bool        m_bEnabled;
    int     m_iAccIndex = 0;
    
    float   m_fDelay = 0.1f;        //帧间隔
};


#endif /* defined(__Tool__Part__) */
