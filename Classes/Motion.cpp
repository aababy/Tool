//
//  Motion.cpp
//  Tool
//
//  Created by luoxp on 14-8-6.
//
//
#include "IncludeForCpp.h"
#include "Motion.h"


Motion::Motion(string sName, const char *pFileName, vector<string> vFrameNameOrdered, int iStart, int iEnd, CCPoint &origin, CCPoint &showForPreview, CCNode *parent)
{
    m_vFrameNameOrdered = vFrameNameOrdered;
    this->iStart = iStart;
    this->iEnd = iEnd;
    
    sResName = pFileName;
    sMotionName = sName;
    m_iFrameBG = parent;
    location = origin;
    m_preview = showForPreview;
    
    //MotionName 加入Start和End
    char buffer[100];
    sprintf(buffer, "%d-%d", iStart + 1, iEnd + 1);
    sMotionName = buffer;
    
    //加入atk时, 便加入1个Motion
    importPart(pFileName);
}

void Motion::importPart(const char *pFileName)
{
    if (m_vParts.empty()) {
        m_mainPart = new Part(pFileName, location, location, m_preview, m_iFrameBG);
        m_mainPart->setMain();
        m_vParts.push_back(m_mainPart);
        
        setCurOperationIndex(0);
    }
    else
    {
        CCPoint point = ccp(100, 0);
        point = ccpAdd(location, point);
        
        Part* part = new Part(pFileName, point, location, m_preview, m_iFrameBG);
        part->setStartFrameIndex(m_iMainIndex);
        m_vParts.push_back(part);
        
        setCurOperationIndex(getPartsCount() - 1);
    }
}

int Motion::getPartsCount()
{
    return m_vParts.size();
}

void Motion::setCurOperationIndex(int idx)
{
    if (m_iCurOperationIndex != idx) {
        m_iCurOperationIndex = idx;
        xNotify->postNotification(UPDATE_PROPERTY);
        xNotify->postNotification(UPDATE_MOTION);
        xNotify->postNotification(UPDATE_EFFECT_LIST);
    }
}

string& Motion::getMotionName(int i)
{
    return m_mainPart->getMotionName(i);
}

int Motion::getCurAtkIndex()
{
    return m_mainPart->getCurAtkIndex();
}

void Motion::setCurAtkIndex(int i)
{
    m_mainPart->setCurAtkIndex(i);
}

string &Motion::getPartNameByIndex(int idx)
{
    return m_vParts.at(idx)->sPartName;
}

int Motion::getCurOperationIndex()
{
    return m_iCurOperationIndex;
}


