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
    clear();
    CCDirector::sharedDirector()->getScheduler()->unscheduleUpdateForTarget(this);
}

//void PartManager::init(CCPoint loc, CCPoint preview, CCNode *iFrameBG)
//{
//    location = loc;
//    m_iFrameBG = iFrameBG;
//    m_preview = preview;
//    
//    CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget(this, 0, false);
//}

//void PartManager::importPart(const char *pFileName)
//{
//    if (m_vParts.empty()) {
//        m_mainPart = new Part(pFileName, location, location, m_preview, m_iFrameBG);
//        m_mainPart->setMain();
//        m_vParts.push_back(m_mainPart);
//        
//        setCurOperationIndex(0);
//        m_iMainIndex = m_mainPart->m_iCurIndex;
//        m_iMainCount = m_mainPart->m_iFrameCount;
//    }
//    else
//    {
//        CCPoint point = ccp(100, 0);
//        point = ccpAdd(location, point);
//        
//        Part* part = new Part(pFileName, point, location, m_preview, m_iFrameBG);
//        part->setStartFrameIndex(m_iMainIndex);
//        m_vParts.push_back(part);
//        
//        setCurOperationIndex(getPartsCount() - 1);
//    }
//}

//int PartManager::getPartsCount()
//{
//    return m_vParts.size();
//}

string &PartManager::getPartNameByIndex(int idx)
{
    return m_vParts.at(idx)->sPartName;
}

//void PartManager::nextFrame(int iCount)
//{
//    for (int i = 0; i < m_vParts.size(); i++) {
//        m_vParts.at(i)->nextFrame(iCount);
//    }
//    
//    m_iMainIndex = m_mainPart->m_iCurIndex;
//    m_iMainCount = m_mainPart->m_iFrameCount;
//}
//
//void PartManager::preFrame(int iCount)
//{
//    for (int i = 0; i < m_vParts.size(); i++) {
//        m_vParts.at(i)->preFrame(iCount);
//    }
//    
//    m_iMainIndex = m_mainPart->m_iCurIndex;
//    m_iMainCount = m_mainPart->m_iFrameCount;
//}

int PartManager::getCurOperationIndex()
{
    return m_iCurOperationIndex;
}

void PartManager::setCurOperationIndex(int idx)
{
    if (m_iCurOperationIndex != idx) {
        m_iCurOperationIndex = idx;
        xNotify->postNotification(UPDATE_PROPERTY);
        xNotify->postNotification(UPDATE_MOTION);
    }
}
//
//CCPoint PartManager::getCurPosition()
//{
//    return m_vParts.at(m_iCurOperationIndex)->getPosition();
//}
//
//void PartManager::setCurPosition(CCPoint &point)
//{
//    m_vParts.at(m_iCurOperationIndex)->setPosition(point);
//}
//
//float PartManager::getCurRotate()
//{
//    return m_vParts.at(m_iCurOperationIndex)->getRotate();
//}
//
//const CCPoint& PartManager::getCurAnchorPoint()
//{
//    return m_vParts.at(m_iCurOperationIndex)->getAnchorPoint();
//}
//
//float PartManager::getCurScale()
//{
//    return m_vParts.at(m_iCurOperationIndex)->getScale();
//}
//
//void PartManager::setCurRotate(float r)
//{
//    m_vParts.at(m_iCurOperationIndex)->setRotate(r);
//}
//
//void PartManager::setCurAnchorPoint(const CCPoint& point)
//{
//    m_vParts.at(m_iCurOperationIndex)->setAnchorPoint(point);
//}
//
//void PartManager::setCurScale(float s)
//{
//    m_vParts.at(m_iCurOperationIndex)->setScale(s);
//}
//
//void PartManager::setDragAndDropOffset(CCPoint &point)
//{
//    m_vParts.at(m_iCurOperationIndex)->setDragAndDropOffset(point);
//}

void PartManager::preview()
{
    m_bInPreview = true;
    
    for (int i = 0; i < m_vParts.size(); i++) {
        m_vParts.at(i)->preview();
    }
}

void PartManager::clear()
{
    for (int i = 0; i < m_vParts.size(); i++) {
        delete m_vParts.at(i);
    }

    m_vParts.clear();
    setCurOperationIndex(-1);
}

void PartManager::update(float delta)
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


void PartManager::save()
{
    //if(getPartsCount() == 0)
    {
        return;
    }
    
    CCDictionary *dictionary = m_mainPart->getDictionary();
    
    string str = xData->getStringForKey(SAVE_PATH);
    
    if (!str.empty() && str.compare("") != 0) {
        if (str.rfind('/') == str.length() - 1) {
            str = str.substr(0, str.length() - 1);
        }
        
        char stringBuffer[250];
        sprintf(stringBuffer, "%s/%s.plist", str.c_str(), "abc");
        dictionary->writeToFile(stringBuffer);
    }
}


//int PartManager::getMainPartMotionCount()
//{
//    if (m_mainPart != NULL) {
//        return m_mainPart->getMotionCount();
//    }
//    
//    return 0;
//}

//void PartManager::setAtkDelay(float var)
//{
//    m_mainPart->setAtkDelay(var);
//}

//float PartManager::getAtkDelay()
//{
//    if (m_mainPart == NULL) {
//        return 0.f;
//    }
//
//    return m_mainPart->getAtkDelay();
//}



