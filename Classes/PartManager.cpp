//
//  PartManager.cpp
//  Tool
//
//  Created by luoxp on 14-7-31.
//
//
#include "IncludeForCpp.h"
#include "PartManager.h"


static PartManager* pInstance = NULL;

PartManager* PartManager::getInstance(void)
{
    if(pInstance == NULL){
        pInstance = new PartManager();
    }
    
	return pInstance;
}

PartManager::PartManager()
{
}


PartManager::~PartManager()
{
    m_vParts.clear();
}

void PartManager::init(CCPoint loc, CCPoint preview, CCNode *iFrameBG)
{
    location = loc;
    m_iFrameBG = iFrameBG;
    m_preview = preview;
}

void PartManager::addPart(const char *pFileName)
{
    CCPoint point = ccp(100, 0);
    point = ccpAdd(location, point);
    
    Part* part = new Part(pFileName, point, location, m_preview, m_iFrameBG);
    m_vParts.push_back(part);
    
    setCurOperationIndex(getPartsCount() - 1);
}

void PartManager::importPart(const char *pFileName)
{
    //TODO, 清除操作
    setCurOperationIndex(0);
    
    m_mainPart = new Part(pFileName, location, location, m_preview, m_iFrameBG);
    m_vParts.push_back(m_mainPart);
}

int PartManager::getPartsCount()
{
    return m_vParts.size();
}

string &PartManager::getPartNameByIndex(int idx)
{
    return m_vParts.at(idx)->sPartName;
}

void PartManager::nextFrame()
{
    for (int i = 0; i < m_vParts.size(); i++) {
        m_vParts.at(i)->nextFrame();
    }
    
    m_iMainIndex = m_mainPart->m_iCurIndex;
    m_iMainCount = m_mainPart->m_iFrameCount;
}

void PartManager::preFrame()
{
    for (int i = 0; i < m_vParts.size(); i++) {
        m_vParts.at(i)->preFrame();
    }
    
    m_iMainIndex = m_mainPart->m_iCurIndex;
    m_iMainCount = m_mainPart->m_iFrameCount;
}

void PartManager::setCurOperationIndex(int idx)
{
    if (m_iCurOperationIndex != idx) {
        m_iCurOperationIndex = idx;
        xNotify->postNotification(UPDATE_PROPERTY);
    }
}

float PartManager::getCurRotate()
{
    return m_vParts.at(m_iCurOperationIndex)->getRotate();
}

const CCPoint& PartManager::getCurAnchorPoint()
{
    return m_vParts.at(m_iCurOperationIndex)->getAnchorPoint();
}

float PartManager::getCurScale()
{
    return m_vParts.at(m_iCurOperationIndex)->getScale();
}

void PartManager::setCurRotate(float r)
{
    m_vParts.at(m_iCurOperationIndex)->setRotate(r);
}

void PartManager::setCurAnchorPoint(const CCPoint& point)
{
    m_vParts.at(m_iCurOperationIndex)->setAnchorPoint(point);
}

void PartManager::setCurScale(float s)
{
    m_vParts.at(m_iCurOperationIndex)->setScale(s);
}

void PartManager::setDragAndDropOffset(CCPoint &point)
{
    m_vParts.at(m_iCurOperationIndex)->setDragAndDropOffset(point);
}

void PartManager::preview()
{
    for (int i = 0; i < m_vParts.size(); i++) {
        m_vParts.at(i)->preview();
    }
}
