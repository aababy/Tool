//
//  Part.cpp
//  Tool
//
//  Created by luoxp on 14-7-30.
//
//

#include "Part.h"
#include "IncludeForCpp.h"


//Part * Part::create(const char *pFileName, CCPoint &location, ImageView *parent)
//{
//	Part * pRet = new Part(pFileName, location, parent);
//    if (pRet)
//    {
//        pRet->autorelease();
//    }
//    else
//    {
//        CC_SAFE_DELETE(pRet);
//    }
//	return pRet;
//}


Part::Part(const char *pFileName, CCPoint &show, CCPoint &origin, CCPoint &showForPreview, CCNode *parent)
{
    sPartName = pFileName;
    
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
    
    m_iFrameCount = m_vFrameName.size();
    
    bubble_sort(m_vFrameName);
    
    //显示帧
    m_sprite = CCSprite::createWithSpriteFrameName(getCurFrameName());
    m_sprite->setPosition(show);
    
    parent->addChild(m_sprite);
    
    m_parent = parent;
    m_origin = origin;
    m_showForPreview = showForPreview;
    
    m_preview = CCSprite::create();
    m_preview->setVisible(false);
    m_parent->addChild(m_preview);
}

Part::~Part()
{
    m_preview->stopAllActions();
    CCNode *parent = m_preview->getParent();
    parent->removeChild(m_preview);
    
    parent = m_sprite->getParent();
    parent->removeChild(m_sprite);
    
    CC_SAFE_RELEASE(m_dictionary);
}

const char * Part::getCurFrameName()
{
    return m_vFrameName.at(m_iCurIndex).sFrameName.c_str();
}

const char * Part::getNextFrameName()
{
    cycleNum(true, m_iFrameCount, &m_iCurIndex);
    return getCurFrameName();
}

const char * Part::getPreFrameName()
{
    cycleNum(false, m_iFrameCount, &m_iCurIndex);
    return getCurFrameName();
}

CCAnimation* Part::getAnimation()
{
    CCArray *animFrames = CCArray::createWithCapacity(m_iFrameCount);
    
    for (int i = 0; i < m_vFrameName.size(); i++) {
        CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(m_vFrameName.at(i).sFrameName.c_str());
        animFrames->addObject(frame);
    }
    
    CCAnimation* animation = CCAnimation::createWithSpriteFrames(animFrames, 0.1f);

    return animation;
}

void Part::nextFrame(int iCount)
{
    m_sprite->initWithSpriteFrameName(getNextFrameName());
}


void Part::preFrame(int iCount)
{
    m_sprite->initWithSpriteFrameName(getPreFrameName());
}

float Part::getRotate()
{
    return m_sprite->getRotation();
}

const CCPoint& Part::getAnchorPoint()
{
    return m_sprite->getAnchorPoint();
}

float Part::getScale()
{
    return m_sprite->getScale();
}

void Part::setAnchorPoint(const CCPoint& point)
{
    m_sprite->setAnchorPoint(point);
    m_preview->setAnchorPoint(point);
}

void Part::setScale(float s)
{
    m_sprite->setScale(s);
    m_preview->setScale(s);
}

void Part::setRotate(float r)
{
    m_sprite->setRotation(r);
    m_preview->setRotation(r);
}

void Part::setDragAndDropOffset(CCPoint &point)
{
    m_sprite->setPosition(ccpAdd(m_sprite->getPosition(), point));
}

void Part::preview()
{
    m_bOnWait = true;
    checkIfNeedToStart(0);
}

void Part::checkIfNeedToStart(int iFrameIndex)
{
    if (m_bOnWait && iFrameIndex >= iStartFrameIndex) {
        m_bOnWait = false;
        m_iOldFrameIndex = -1;
        m_preview->stopAllActions();
        m_preview->setVisible(true);
        m_preview->setPosition(ccpAdd(m_showForPreview, getOffset()));
        
        m_pAction = CCAnimate::create(getAnimation());
        m_preview->runAction(CCSequence::create(m_pAction, CCCallFunc::create(this, callfunc_selector(Part::actionDone)), NULL));
        m_bRunning = true;
    }
}

CCPoint Part::getOffset()
{
    return ccpSub(m_sprite->getPosition(), m_origin);

}

int Part::getCurFrameIndex()
{
    if (m_bRunning) {
        
        if (m_iOldFrameIndex == m_iFrameCount - 1) {
            return m_iOldFrameIndex;
        }
        
        CCArray* spriteArray = m_pAction->getAnimation()->getFrames();     //其中的m_pLocateSkillAction是一个CCAnimate的对象指针
        
        for (int i = 0; i < m_iFrameCount; i++) {
            CCAnimationFrame * tempFrame = (CCAnimationFrame*)spriteArray->objectAtIndex(i) ;   //这里要注意了，返回的是CCAnimationFrame的指针，不是CCSpriteFrame*
            
            if(m_preview->isFrameDisplayed(tempFrame->getSpriteFrame()))     //m_Sprite就是正在播放动画的那个精灵
            {
                m_iOldFrameIndex = i;
                return i;
            }
        }
    }
    
    return -1;
}

void Part::actionDone()
{
    m_bRunning = false;
    m_bOnWait = false;
    
    //非主体在运行完后自动消失
    if (!m_bMain) {
        m_preview->setVisible(false);
    }
}

void Part::setStartFrameIndex(int iStart)
{
    iStartFrameIndex = iStart;
}

void Part::setMain()
{
    m_bMain = true;
    
    //如果是主体, 需要保留CCDictionary
    m_dictionary = CCDictionary::createWithContentsOfFile(sPartName.c_str());
    m_dictionary->retain();
}

CCPoint Part::getPosition()
{
    return ccpSub(m_sprite->getPosition(), m_origin);
}

void Part::setPosition(CCPoint &point)
{
    m_sprite->setPosition(ccpAdd(m_origin, point));
}


CCDictionary * Part::getDictionary()
{
    return m_dictionary;
}

int Part::getMotionCount()
{
    return m_vMotion.size();
}


string &Part::getMotionName(int i)
{
    return m_vMotion.at(i)->sMotionName;
}

int Part::getCurAtkIndex()
{
    return m_iCurAtk;
}

void Part::setCurAtkIndex(int i)
{
    m_iCurAtk = i;
}

void Part::setAtkDelay(float var)
{
    m_vMotion.at(m_iCurIndex)->setDelay(var);
}

float Part::getAtkDelay()
{
    return m_vMotion.at(m_iCurIndex)->getDelay();
}

