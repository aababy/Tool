//
//  PartManager.h
//  Tool
//
//  Created by luoxp on 14-7-31.
//
//

#ifndef __Tool__PartManager__
#define __Tool__PartManager__

#include "IncludeForHeader.h"

#define xPM                  (PartManager::getInstance())

class Part;

class PartManager : public CCNode{
    
public:
    static PartManager* getInstance(void);

    PartManager();
    
    void importPart(const char *pFileName);
    void init(CCPoint location, CCPoint preview, CCNode *parent);
    int     getPartsCount();
    string& getPartNameByIndex(int idx);
    
    //主体Index
    int m_iMainIndex = 0;
    int m_iMainCount = 0;
        
    void nextFrame(int i);
    void preFrame(int i);
    
    void setCurOperationIndex(int idx);
    float getCurRotate();
    const CCPoint& getCurAnchorPoint();
    
    CCPoint getCurPosition();
    void setCurPosition(CCPoint &point);
    float getCurScale();
    void setCurRotate(float r);
    void setCurAnchorPoint(const CCPoint& point);
    void setCurScale(float s);
    int getCurOperationIndex();
    
    void setDragAndDropOffset(CCPoint &point);
    void preview();
    void clear();
    
    void update(float delta);
    void save();
    
    int getMainPartMotionCount();
    void setAtkDelay(float var);
    float getAtkDelay();
    
private:
    vector<Part*> m_vParts;
    ~PartManager();
    
    CCPoint     location;
    CCPoint     m_preview;
    CCNode      *m_iFrameBG;
    
    Part*       m_mainPart;
    
    int         m_iCurOperationIndex = -1;
    
    bool        m_bInPreview = false;
};


#endif /* defined(__Tool__PartManager__) */
