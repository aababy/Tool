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
    Motion(string sName, const char *pFileName, vector<string> vFrameNameOrdered, int iStart, int iEnd, CCPoint &origin, CCPoint &showForPreview, CCNode *parent);
    void importPart(const char *pFileName);
    int getPartsCount();
    void setCurOperationIndex(int idx);
    string& getMotionName(int i);
    int getCurAtkIndex();
    void setCurAtkIndex(int i);
    string& getPartNameByIndex(int idx);
    int getCurOperationIndex();
    void setEnabled(bool bEnabled);
    bool isInMotion(int idx);
    
    string sMotionName;
    
private:
    int iStart;
    int iEnd;
    
    string sResName;
    
    CCSprite * m_spPreview = NULL;            //用于预览
    CCSprite  *m_sprite;                    //用于显示帧
    
    vector<string> m_vFrameNameOrdered;
    CC_SYNTHESIZE(float, m_fDelay, Delay);
    
    vector<Part*> m_vParts;
    
    CCPoint     location;
    CCPoint     m_preview;
    CCNode      *m_iFrameBG;
    
    Part*       m_mainPart;
    
    int         m_iMainIndex;
    int         m_iCurOperationIndex = 0;
    
    bool        m_bEnabled;
};



#endif /* defined(__Tool__Motion__) */
