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

struct FramesName
{
    string  sFrameName;
    int     iNumber;
};


class Part : public CCObject{
    
public:
    //static Part * create(const char *pFileName, CCPoint &location, ImageView *parent);
    Part(const char *pFileName, CCPoint &show, CCPoint &origin, CCPoint &showForPreview, CCNode *parent);
    void setStartFrameIndex(int iStart);
    void setMain();
    
    vector<FramesName> m_vFrameName;
    vector<string> m_vFrameNameOrdered;
    
    const char * getCurFrameName();
    const char * getNextFrameName();
    const char * getPreFrameName();
    const char * getFrameCountStr();
    float getRotate();
    const CCPoint& getAnchorPoint();
    float getScale();
    void setRotate(float r);
    void setAnchorPoint(const CCPoint& point);
    void setScale(float s);
    void setDragAndDropOffset(CCPoint &point);
    
    void nextFrame();
    void preFrame();
    void preview();
    
    int m_iCurIndex = 0;
    int m_iFrameCount = 0;
    
    CCAnimation* getAnimation();            //这个函数是否分离出去?
    
    string  sPartName;
    
    int getCurFrameIndex();
    void checkIfNeedToStart(int iFrameIndex);

    ~Part();
    
    bool m_bRunning = false;        //动画是否在播放中
private:
    CCPoint getOffset();
    void actionDone();    
    
    int getNumber(string &str);
    void bubble_sort();
    
    CCSprite * m_preview = NULL;           //用于预览
    
    CCSprite  *m_sprite;            //用于显示帧
    ImageView *m_iFrame;
    CCPoint  m_origin;
    CCPoint  m_showForPreview;
    CCNode  *m_parent;
    
    CCAnimate * m_pAction;
    
    int m_iOldFrameIndex;
    

    bool m_bOnWait = false;         //是否在等待主体播放到开始的那一帧.
    bool m_bMain = false;           //是否是主体
    
    int iStartFrameIndex = 0;
    
};


#endif /* defined(__Tool__Part__) */
