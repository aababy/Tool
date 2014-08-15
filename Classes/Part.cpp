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
Part::Part(vector<string> &vNames, CCPoint &show, CCPoint &origin, CCPoint &showForPreview, CCNode *parent, vector<string> vFrameName)
{
    sPartName = vNames.at(0);               //PartName 显示只会显示一个
    
    for (int i = 0; i < vFrameName.size(); i++) {
        FramesName frames;
        frames.iNumber = i;
        frames.sFrameName = vFrameName.at(i);
        
        m_vFrameName.push_back(frames);
    }
    
    m_iFrameCount = m_vFrameName.size();
    
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



Part::Part(vector<string> &vNames, CCPoint &show, CCPoint &origin, CCPoint &showForPreview, CCNode *parent, int iAcc)
{
    sPartName = vNames.at(0);
    m_iAccIndex = iAcc;
    
    //加入Cache
    for (int i = 0; i < vNames.size(); i++) {
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(vNames.at(i).c_str());
        
        CCDictionary *dictionary = CCDictionary::createWithContentsOfFile(vNames.at(i).c_str());
        CCDictionary *framesDict = (CCDictionary*)dictionary->objectForKey("frames");
        
        CCDictElement* pElement = NULL;
        CCDICT_FOREACH(framesDict, pElement)
        {
            FramesName frame;
            frame.sFrameName = pElement->getStrKey();
            frame.iNumber = getNumber(frame.sFrameName);
            m_vFrameName.push_back(frame);
        }
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
    
    CCAnimation* animation = CCAnimation::createWithSpriteFrames(animFrames, m_fDelay);

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

void Part::setFrame(int iIndexStartFromMain)
{
    if (iStartFrameIndex <= iIndexStartFromMain) {
        m_iCurIndex = iIndexStartFromMain - iStartFrameIndex;
        
        if (m_iCurIndex < m_vFrameName.size()) {
            m_sprite->initWithSpriteFrameName(getCurFrameName());
            m_sprite->setVisible(true);
        }
        else
        {
            m_sprite->setVisible(false);
        }
    }
    else
    {
        m_sprite->setVisible(false);
    }
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
        m_preview->initWithSpriteFrameName(m_vFrameName.at(0).sFrameName.c_str());
        m_preview->setPosition(ccpAdd(m_showForPreview, getOffset()));
        
        m_pAction = CCAnimate::create(getAnimation());
        CCSequence * sequence = CCSequence::create(m_pAction, CCCallFunc::create(this, callfunc_selector(Part::actionDone)), NULL);
        
        //位移
        if(m_flag[FI_MOVE])
        {
            //根据帧数确定时间
            CCPoint dest = ccp(m_iFrameCount * 50, 0);       //水平
            float duration = m_iFrameCount * m_fDelay;
            CCSpawn* spawn = CCSpawn::create(sequence, CCMoveBy::create(duration, dest), NULL);
            
            m_preview->runAction(spawn);
        }
        else
        {
            m_preview->runAction(sequence);
        }
        
        m_preview->setVisible(true);
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
//    if (!m_bMain) {
//        m_preview->setVisible(false);
//    }
    
    //主体也消失, 仅仅在对于是单独预览时.
    m_preview->setVisible(false);
}

void Part::setStartFrameIndex(int iStart)
{
    iStartFrameIndex = iStart;
}

int Part::getStartFrameIndex()
{
    return iStartFrameIndex;
}


void Part::setMain()
{
    m_bMain = true;    
}

CCPoint Part::getPosition()
{
    return ccpSub(m_sprite->getPosition(), m_origin);
}

void Part::setPosition(CCPoint &point)
{
    m_sprite->setPosition(ccpAdd(m_origin, point));
}

void Part::setEnabled(bool bEnabled)
{
    m_bEnabled = bEnabled;
    
    m_sprite->setVisible(m_bEnabled);
    m_preview->setVisible(false);
}

string Part::getEffectName()
{
    char temp[10];
    sprintf(temp, "effect%d", m_iAccIndex);
    return string(temp);
}

void Part::saveEffectToDictionary(CCDictionary *effects)
{
    CCDictionary *effect = new CCDictionary();
    
    insertString(effect, "fileName", sPartName);
    
    //anchorPoint
    CCPoint point = m_sprite->getAnchorPoint();
    insertCCPoint(effect, "anchorPoint", point);
    
    //delay
    insertFloat(effect, "delay", m_fDelay);
    
    //position
    point = getPosition();
    insertCCPoint(effect, "position", point);
    
    //rotation
    insertFloat(effect, "rotation", m_sprite->getRotation());
    
    effects->setObject(effect, getEffectName());
}

void Part::setDelay(float delay)
{
    m_fDelay = delay;
}

float Part::getDelay()
{
    return m_fDelay;
}

void Part::setFlags(flagIndex index, bool bFlag)
{
    m_flag[index] = bFlag;
}

bool Part::getFlags(flagIndex index)
{
    return m_flag[index];
}




