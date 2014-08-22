//
//  Motion.cpp
//  Tool
//
//  Created by luoxp on 14-8-6.
//
//
#include "IncludeForCpp.h"
#include "Motion.h"


Motion::Motion(string sName, vector<string> &vNames, vector<string> vFrameNameOrdered, int iStart, int iEnd, CCPoint &origin, CCPoint &showForPreview, CCNode *parent, int iMotionAccIndex)
{
    m_vFrameNameOrdered = vFrameNameOrdered;
    this->iStart = iStart;
    this->iEnd = iEnd;
    
    for (int i = 0; i < vNames.size(); i++) {
        if (i != 0) {   //间隔符
            sResName += ",";
        }
        sResName += vNames.at(i);               //保存ResName
    }
    
    sSaveName = sName;
    m_iFrameBG = parent;
    location = origin;
    m_preview = showForPreview;
    m_iAccIndex = iMotionAccIndex * 100 + 1;
    
    //MotionName 加入Start和End
    char buffer[100];
    sprintf(buffer, "%d-%d", iStart + 1, iEnd + 1);
    sMotionName = buffer;
    
    //加入atk时, 便加入1个Motion
    importPart(vNames);
}

Motion::~Motion()
{
    clear();
}

void Motion::importPart(vector<string> &vNames)
{
    if (m_vParts.empty()) {
        m_mainPart = new Part(vNames, location, location, m_preview, m_iFrameBG, m_vFrameNameOrdered, sSaveName);
        m_mainPart->setMain();
        m_vParts.push_back(m_mainPart);
        
        setCurOperationIndex(0);
        
        m_show = m_mainPart->getSprite();
        m_showPreview = m_mainPart->getPreview();
    }
    else
    {
        CCPoint point = CCPointZero;
        //ccp(100, 0);
        //point = ccpAdd(location, point);
        
        Part* part = new Part(vNames, point, location, m_preview, m_show, m_showPreview, m_iAccIndex, sSaveName);
        m_iAccIndex++;
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

void Motion::preview()
{
    m_bInPreview = true;
    
    for (int i = 0; i < m_vParts.size(); i++) {
        m_vParts.at(i)->preview();
    }
}

void Motion::clear()
{
    for (int i = m_vParts.size() - 1; i >= 0; i--) {
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

int Motion::getFramesCount()
{
    return iEnd - iStart + 1;
}

void Motion::getEffectsName(CCDictionary *dic, CCDictionary *effects)
{
    //从第1帧开始
    for (int iFrameIndex = 0; iFrameIndex < getFramesCount(); iFrameIndex++)
    {
        string str;
        for (int i = 1; i < getPartsCount(); i++)
        {
            if (m_vParts.at(i)->getStartFrameIndex() == iFrameIndex) {
                if (! str.empty()) {
                    str += ",";
                }
                //如果起始帧相同, 就加上它的名字
                str += m_vParts.at(i)->getEffectName();
                
                //插入1个effect
                m_vParts.at(i)->saveEffectToDictionary(effects);        //这个函数加入effects
            }
        }
        
        //插入Dictionary
        if (!str.empty()) {
            insertString(dic, itostring(iFrameIndex), str);
        }
    }
}

void Motion::setDelay(float delay)
{
    m_vParts.at(m_iCurOperationIndex)->setDelay(delay);
}

float Motion::getDelay()
{
    return m_vParts.at(m_iCurOperationIndex)->getDelay();
}

void Motion::setDegree(float degree)
{
    m_vParts.at(m_iCurOperationIndex)->setDegree(degree);
}

float Motion::getDegree()
{
    return m_vParts.at(m_iCurOperationIndex)->getDegree();
}

void Motion::setSpeed(float speed)
{
    m_vParts.at(m_iCurOperationIndex)->setSpeed(speed);
}

float Motion::getSpeed()
{
    return m_vParts.at(m_iCurOperationIndex)->getSpeed();
}


void Motion::setFlags(flagIndex index, bool bFlag)
{
    m_vParts.at(m_iCurOperationIndex)->setFlags(index, bFlag);
}

bool Motion::getFlags(flagIndex index)
{
    if(m_iCurOperationIndex == -1)
    {
        return false;
    }
    
    return m_vParts.at(m_iCurOperationIndex)->getFlags(index);
}

void Motion::removeEffect()
{
    if (m_vParts.size() >= 2) {
        Part* part = m_vParts.at(m_vParts.size() - 1);
        delete part;
        
        m_vParts.pop_back();
        
        setCurOperationIndex(m_vParts.size() - 1);
    }
}
