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

Skill::~Skill()
{
    CCDirector::sharedDirector()->getScheduler()->unscheduleUpdateForTarget(this);
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
        
        m_iCurIndex = 0;
        m_sprite = CCSprite::createWithSpriteFrameName(getCurFrameName());
        m_sprite->setPosition(m_origin);
        m_parent->addChild(m_sprite);
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
    
    CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget(this, 0, false);
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
        
        m_curMotion = new Motion(buffer, sPartName.c_str(), vFrameName, m_iLastIndex, m_iCurIndex, m_origin, m_showForPreview, m_parent);
        m_vMotion.push_back(m_curMotion);
        
        m_iLastIndex = m_iCurIndex + 1;
        
        setCurAtkIndex(getMotionCount() - 1, OT_NEW);
        
        //增加atk后, 会回到新的atk的第一帧, (现在不这样做了)
        //setCurIndex(m_curMotion->getLastFrameIndex());
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

//改变当前的Atk有两种情况, 1是用户选择或者新建atk, 需要改变当前全局帧, 2是用户浏览(上一帧,下一帧), 不改变当前全局帧 (已经改变了)
//bChangeAllIndex
void Skill::setCurAtkIndex(int i, setOperateType type)
{
    if (m_iCurAtk != i) {
        m_iCurAtk = i;
        
        //转换当前的Motion是个非常重要的函数, 需要将所有非当前Motion设置为不可见, 将当前Motion设置为可见.
        for (int i = 0; i < getMotionCount(); i++) {
            if (i != m_iCurAtk) {
                m_vMotion.at(i)->setEnabled(false);
            }
        }
        
        if (m_iCurAtk != -1) {
            m_curMotion = m_vMotion.at(m_iCurAtk);
            m_curMotion->setEnabled(true);
            
            //true 需要改变
            if (type == OT_SELECT) {
                setCurIndex(m_curMotion->getLastFrameIndex());
            }
        }
        
        //这个post几乎是同步的, 不是异步的, 所以写最后
        xNotify->postNotification(UPDATE_MOTION_LIST);
        xNotify->postNotification(UPDATE_EFFECT_LIST);
    }
    
    if (type == OT_BROWSE || type == OT_NEW) {
        //通知相应地Motion, 改Frame
        
        if (m_curMotion != NULL) {
            m_curMotion->setFrame(m_iCurIndex);
            
            if (type == OT_BROWSE) {
                m_sprite->setVisible(false);
            }
        }
        else
        {
            m_sprite->initWithSpriteFrameName(getCurFrameName());
            m_sprite->setVisible(true);
        }
    }
    else
    {
        m_sprite->setVisible(false);
    }
}

const char * Skill::getCurFrameName()
{
    return m_vFrameName.at(m_iCurIndex).sFrameName.c_str();
}

void Skill::setAtkDelay(float var)
{
    m_vMotion.at(m_iCurAtk)->setDelay(var);
}

float Skill::getAtkDelay()
{
    return m_vMotion.at(m_iCurAtk)->getDelay();
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
    setCurIndex(m_iCurIndex);       //仅仅方便更新UI
    checkAllIndex();
}

void Skill::preFrame(int iCount)
{
    for (int i = 0; i < iCount; i++) {
        cycleNum(false, m_iFrameCount, &m_iCurIndex);
    }
    setCurIndex(m_iCurIndex);       //仅仅方便更新UI
    checkAllIndex();
}

void Skill::setCurIndex(int idx)
{
    m_iCurIndex = idx;
    xNotify->postNotification(UPDATE_ALL_INDEX);
}

void Skill::checkAllIndex()
{
    int i;
    for (i = 0; i < getMotionCount(); i++) {
        if (m_vMotion.at(i)->isInMotion(m_iCurIndex)) {
            break;
        }
    }
    
    if (i != getMotionCount()) {
        setCurAtkIndex(i, OT_BROWSE);
    }
    else
    {
        m_curMotion = NULL;
        setCurAtkIndex(-1, OT_BROWSE);
    }
}

void Skill::setDragAndDropOffset(CCPoint &point)
{
    m_curMotion->setDragAndDropOffset(point);
}

void Skill::preview()
{
    //通知第1个Motion进行Preview, 仅仅是第1个.
    if (! m_vMotion.empty()) {
        m_bInPreview = true;
        m_iPreviewIndex = 0;
        m_vMotion.at(0)->preview();
    }
}

void Skill::update(float delta)
{
    if (m_bInPreview) {
        //update all
        for (int i = 0; i < getMotionCount(); i++) {
            m_vMotion.at(i)->update(delta);
        }
        
        //检查是否上一个Motion preview 完成, 通知下一个.
        if (m_vMotion.at(m_iPreviewIndex)->isInPreview() == false) {
            m_iPreviewIndex++;
            
            if (m_iPreviewIndex < getMotionCount()) {
                m_vMotion.at(m_iPreviewIndex)->preview();
            }
            else
            {
                //全部结束了.
                m_bInPreview = false;
            }
        }
    }
}

