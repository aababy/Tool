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
        m_mainPart = new Part(pFileName, location, location, m_preview, m_iFrameBG, m_vFrameNameOrdered);
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
    return sMotionName;
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

void Motion::setEnabled(bool bEnabled)
{
    m_bEnabled = bEnabled;
    
    for (int i = 0; i < getPartsCount(); i++) {
        m_vParts.at(i)->setEnabled(bEnabled);
    }
}

bool Motion::isInMotion(int idx)
{
    if (idx >= iStart && idx <= iEnd) {
        return true;
    }
    else
    {
        return false;
    }
}

void Motion::setFrame(int iFrame)
{
    for (int i = 0; i < m_vParts.size(); i++) {
        //这个index时个相对于主part的index
        m_vParts.at(i)->setFrame(iFrame - iStart);
    }
    
    m_iMainIndex = m_mainPart->m_iCurIndex;
}

//获取离开这个Motion前, 最后停留在Motion内的Index
int Motion::getLastFrameIndex()
{
    return iStart + m_iMainIndex;
}

void Motion::setDragAndDropOffset(CCPoint &point)
{
    m_vParts.at(m_iCurOperationIndex)->setDragAndDropOffset(point);
}

CCPoint Motion::getCurPosition()
{
    return m_vParts.at(m_iCurOperationIndex)->getPosition();
}

void Motion::setCurPosition(CCPoint &point)
{
    m_vParts.at(m_iCurOperationIndex)->setPosition(point);
}

float Motion::getCurRotate()
{
    return m_vParts.at(m_iCurOperationIndex)->getRotate();
}

const CCPoint& Motion::getCurAnchorPoint()
{
    return m_vParts.at(m_iCurOperationIndex)->getAnchorPoint();
}

float Motion::getCurScale()
{
    return m_vParts.at(m_iCurOperationIndex)->getScale();
}

void Motion::setCurRotate(float r)
{
    m_vParts.at(m_iCurOperationIndex)->setRotate(r);
}

void Motion::setCurAnchorPoint(const CCPoint& point)
{
    m_vParts.at(m_iCurOperationIndex)->setAnchorPoint(point);
}

void Motion::setCurScale(float s)
{
    m_vParts.at(m_iCurOperationIndex)->setScale(s);
}

void Motion::setAtkDelay(float var)
{
    m_mainPart->setAtkDelay(var);
}

float Motion::getAtkDelay()
{
    if (m_mainPart == NULL) {
        return 0.f;
    }
    
    return m_mainPart->getAtkDelay();
}

void Motion::preview()
{
    m_bInPreview = true;
    
    for (int i = 0; i < m_vParts.size(); i++) {
        m_vParts.at(i)->preview();
    }
}

void Motion::clear()
{
    for (int i = 0; i < m_vParts.size(); i++) {
        delete m_vParts.at(i);
    }
    
    m_vParts.clear();
    setCurOperationIndex(-1);
}

void Motion::update(float delta)
{
    if (m_bInPreview) {
        int iCurIndex = m_mainPart->getCurFrameIndex();
        
        for (int i = 1; i < m_vParts.size(); i++) {
            m_vParts.at(i)->checkIfNeedToStart(iCurIndex);
        }
        
        //如果主体没有运行了, 就不必再启动其它的Part(但其它的Part(已经运行了的)可以继续运行下去)
        if(m_mainPart->m_bRunning == false)
        {
            m_bInPreview = false;
        }
    }
}

bool Motion::isInPreview()
{
    return m_bInPreview;
}



