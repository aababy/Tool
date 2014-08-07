//
//  Skill.cpp
//  Tool
//
//  Created by luoxp on 14-8-6.
//
//
#include "IncludeForCpp.h"
#include "Skill.h"


static Skill* pInstance = NULL;

Skill* Skill::getInstance(void)
{
    if(pInstance == NULL){
        pInstance = new Skill();
    }
    
	return pInstance;
}

void Skill::importPart(const char *pFileName)
{
    //先判断, 是否是第一次导入Part, Part作为主体
    if (m_iFrameCount == 0) {
        //加入Cache
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(pFileName);
        
        CCDictionary *dictionary = CCDictionary::createWithContentsOfFile(pFileName);
        CCDictionary *framesDict = (CCDictionary*)dictionary->objectForKey("frames");
        
        CCDictElement* pElement = NULL;
        CCDICT_FOREACH(framesDict, pElement)
        {
            FramesName frame;
            frame.sFrameName = pElement->getStrKey();
            frame.iNumber = getNumber(frame.sFrameName);
            m_vFrameName.push_back(frame);
        }
        
        bubble_sort(m_vFrameName);
        
        m_iFrameCount = m_vFrameName.size();
        
        sPartName = pFileName;
    }
    else
    {
        //判断当前帧所在Atk, 将Part作为Effect插入
        m_curMotion->importPart(pFileName);
    }
}

void Skill::init(CCPoint location, CCPoint preview, CCNode *parent)
{
    m_origin = location;
    m_showForPreview = preview;
    m_parent = parent;    
}

int Skill::getMotionCount()
{
    return m_vMotion.size();
}

void Skill::addMotion()
{
    if (m_iCurIndex != 0) {
        vector<string> vFrameName;
        
        for (int i = m_iLastIndex; i < m_iCurIndex + 1; i++) {
            string str = m_vFrameName.at(i).sFrameName;
            vFrameName.push_back(str);
        }
        
        char buffer[10];
        sprintf(buffer, "atk%d", getMotionCount() + 1);
        
        m_curMotion = new Motion(buffer, sPartName.c_str(), vFrameName, m_iLastIndex, m_iCurIndex, m_parent);
        m_vMotion.push_back(m_curMotion);
        
        m_iLastIndex = m_iCurIndex + 1;
        
        m_iCurAtk = getMotionCount() - 1;
    }
}

string &Skill::getMotionName(int i)
{
    return m_vMotion.at(i)->sMotionName;
}

int Skill::getCurAtkIndex()
{
    return m_iCurAtk;
}

void Skill::setCurAtkIndex(int i)
{
    m_iCurAtk = i;
    m_curMotion = m_vMotion.at(m_iCurAtk);
    
    xNotify->postNotification(UPDATE_MOTION_LIST);
    xNotify->postNotification(UPDATE_EFFECT_LIST);
}

void Skill::setAtkDelay(float var)
{
    m_vMotion.at(m_iCurMotion)->setDelay(var);
}

float Skill::getAtkDelay()
{
    return m_vMotion.at(m_iCurMotion)->getDelay();
}

int Skill::getFrameCount()
{
    return m_iFrameCount;
}

void Skill::nextFrame(int iCount)
{
    for (int i = 0; i < iCount; i++) {
        cycleNum(true, m_iFrameCount, &m_iCurIndex);
    }
}

void Skill::preFrame(int iCount)
{
    for (int i = 0; i < iCount; i++) {
        cycleNum(false, m_iFrameCount, &m_iCurIndex);
    }
}
