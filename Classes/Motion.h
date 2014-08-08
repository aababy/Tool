//
//  Motion.h
//  Tool
//
//  Created by luoxp on 14-8-6.
//
//  每个Motion 拥有一个PartManager,

#ifndef __Tool__Motion__
#define __Tool__Motion__

#include "IncludeForHeader.h"

class Motion {
    
public:
    Motion(string sName, const char *pFileName, vector<string> vFrameNameOrdered, int iStart, int iEnd, CCPoint &origin, CCPoint &showForPreview, CCNode *parent, int iMotionAccIndex);
    void importPart(const char *pFileName);
    int getPartsCount();
    void setCurOperationIndex(int idx);
    string& getMotionName(int i);
    string& getPartNameByIndex(int idx);
    int getCurOperationIndex();
    void setEnabled(bool bEnabled);
    bool isInMotion(int idx);
    void setFrame(int iFrame);
    int getLastFrameIndex();
    
    string sMotionName;         //e.g. "0-5" or "6-11"
    string sSaveName;           //e.g. "atk1"
    string sResName;            //e.g. "xxx.plist"
    void setDragAndDropOffset(CCPoint &point);
    
    CCPoint getCurPosition();
    void setCurPosition(CCPoint &point);
    float getCurRotate();
    const CCPoint& getCurAnchorPoint();
    float getCurScale();
    void setCurRotate(float r);
    void setCurAnchorPoint(const CCPoint& point);
    void setCurScale(float s);
    
    void preview();
    void clear();
    void update(float delta);
    bool isInPreview();
    int getFramesCount();
    
    void getEffectsName(CCDictionary *dic, CCDictionary *effects);

    vector<string> m_vFrameNameOrdered;
    
private:
    int iStart = 0;
    int iEnd = 0;
    
    
    
    CCSprite * m_spPreview = NULL;            //用于预览
    CCSprite  *m_sprite;                    //用于显示帧
    

    CC_SYNTHESIZE(float, m_fDelay, Delay);
    
    vector<Part*> m_vParts;
    
    CCPoint     location;
    CCPoint     m_preview;
    CCNode      *m_iFrameBG;
    
    Part*       m_mainPart;
    
    int         m_iMainIndex = 0;           //这个是相对的?
    int         m_iCurOperationIndex = 0;
    
    bool        m_bEnabled;
    bool        m_bInPreview = false;
    
    int         m_iAccIndex = 1;
};



#endif /* defined(__Tool__Motion__) */
