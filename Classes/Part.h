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


class Part {
    
public:
    //static Part * create(const char *pFileName, CCPoint &location, ImageView *parent);
    Part(const char *pFileName, CCPoint &show, CCPoint &origin, CCPoint &showForPreview, CCNode *parent);
    
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
    
private:
    CCPoint getOffset();
    
    int getNumber(string &str);
    void bubble_sort();
    
    CCSprite * m_preview = NULL;           //用于预览
    
    CCSprite  *m_sprite;            //用于显示帧
    ImageView *m_iFrame;
    //ImageView *parent;
    CCPoint  m_origin;
    CCPoint  m_showForPreview;
    CCNode  *m_parent;
};


#endif /* defined(__Tool__Part__) */
