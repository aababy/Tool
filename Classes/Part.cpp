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
    
    bubble_sort();
    
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

int Part::getNumber(string &str)
{
    int iDot = str.rfind('.');
    
    int iNumber = str.find_last_not_of("0123456789", iDot - 1) + 1;
    
    string sNumber = str.substr(iNumber, iDot - iNumber);
    
    return atoi(sNumber.c_str());
}

void Part::bubble_sort()
{
    int size = m_vFrameName.size();
    int i, j;
    
    for(j = 0; j < size-1; j++)
    {
        for(i = 0; i < size-1-j; i++)
        {
            if(m_vFrameName.at(i).iNumber > m_vFrameName.at(i+1).iNumber)//数组元素大小按升序排列
            {
                swap(m_vFrameName.at(i), m_vFrameName.at(i+1));
            }
        }
    }
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
    CCArray* animFrames = CCArray::createWithCapacity(m_iFrameCount);
    
    for (int i = 0; i < m_vFrameName.size(); i++) {
        CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(m_vFrameName.at(i).sFrameName.c_str());
        animFrames->addObject(frame);
    }
    
    CCAnimation* animation = CCAnimation::createWithSpriteFrames(animFrames, 0.1f);

    return animation;
}

void Part::nextFrame()
{
    m_sprite->initWithSpriteFrameName(getNextFrameName());
}


void Part::preFrame()
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
    m_preview->stopAllActions();
    m_preview->setVisible(true);
    m_preview->setPosition(ccpAdd(m_showForPreview, getOffset()));
    m_preview->runAction(CCAnimate::create(getAnimation()));
}

CCPoint Part::getOffset()
{
    return ccpSub(m_sprite->getPosition(), m_origin);

}





