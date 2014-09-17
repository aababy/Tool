//
//  Part.cpp
//  Tool
//
//  Created by luoxp on 14-7-30.
//
//

#include "Part.h"
#include "IncludeForCpp.h"

#define xScheduler      CCDirector::sharedDirector()->getScheduler()
#define xSpriteFC       CCSpriteFrameCache::sharedSpriteFrameCache()


Part::Part(vector<string> &vNames, CCPoint &show, CCPoint &origin, CCPoint &showForPreview, CCNode *parent, vector<string> vFrameName, string& sMotionName)
{
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
    m_sMotionName = sMotionName;
    
    m_preview = CCSprite::create();
    m_preview->setVisible(false);
    
    m_parent->addChild(m_preview);
    
    init(vNames);
}



Part::Part(vector<string> &vNames, CCPoint &show, CCPoint &origin, CCPoint &showForPreview, CCNode *parent, CCNode *parentForPreview, int iAcc, string& sMotionName)
{
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
    CCSize size = parent->getContentSize();
    
    m_sprite->setPosition(ccp(size.width/2, size.height/2));
    parent->addChild(m_sprite);
    
    m_parent = parent;
    m_origin = origin;
    m_showForPreview = showForPreview;
    m_iAccIndex = iAcc;
    m_sMotionName = sMotionName;
    
    m_preview = CCSprite::create();
    m_preview->setPosition(ccp(size.width/2, size.height/2));
    m_preview->setVisible(false);
    
    parentForPreview->addChild(m_preview);
    
    makeAPartName();
    
    //默认特效*2
    setScale(2.f);
    
    init(vNames);
}


void Part::init(vector<string> &vNames)
{
    sPartName = vNames.at(0);               //PartName 显示只会显示一个
    
    for(int i = 0; i < FLAG_COUNT; i++)
    {
        m_flag[i] = false;
    }
    
    m_duration = 0.f;
    m_interval = 0.f;
    
    saveNames(vNames);
    
    //间隔帧
    for (int i = 0; i < m_iFrameCount; i++)
    {
        float delay = 0.05f;
        m_vDelay.push_back(delay);
    }
}


#define NAME_LEN                      (20)
void Part::makeAPartName()
{
    int dot = sPartName.find('.');
    
    sPartName = sPartName.substr(0, dot);

    if (sPartName.length() > NAME_LEN) {
        sPartName = ".." + sPartName.substr(sPartName.length() - NAME_LEN, NAME_LEN);
    }
    
    //加上帧数
    char buffer[30];
    
    sprintf(buffer, "%s (%lu)", sPartName.c_str(), m_vFrameName.size());
    
    sPartName = buffer;
}


Part::~Part()
{
    xScheduler->unscheduleUpdateForTarget(this);
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
    m_preview->setPosition(ccpAdd(m_preview->getPosition(), point));
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
        m_iCurFrameIndex = 0;
        m_fAccumulate = 0;

        if (m_bMain) {
            m_preview->setPosition(ccpAdd(m_showForPreview, getOffset()));
        }
        else
        {
            m_preview->setPosition(m_sprite->getPosition());
        }
        
        CCSequence * sequence;
        
        //开始设置Frame
        CCSpriteFrame* frame = xSpriteFC->spriteFrameByName(m_vFrameName.at(0).sFrameName.c_str());
        m_preview->setDisplayFrame(frame);
        
        xScheduler->unscheduleUpdateForTarget(this);
        xScheduler->scheduleUpdateForTarget(this, 0, false);
        
        //位移
        if(m_flag[FI_MOVE])
        {
            //根据帧数确定时间
            CCPoint dest = ccp(1000, 0);       //水平
            dest = pointRotateWithAngle(dest, m_degree);
            float duration = 1000 / m_speed;
            
            sequence = CCSequence::create(CCMoveBy::create(duration, dest), CCCallFunc::create(this, callfunc_selector(Part::actionDone)), NULL);
            m_preview->runAction(sequence);
        }
        
        m_preview->setVisible(true);
        m_bRunning = true;
    }
}

void Part::update(float delta)
{
    if (m_bRunning) {
        //累积时间
        m_fAccumulate += delta;
        
        //如果累积时间大于帧间隔, 清空, 然后播放下一帧
        if (m_fAccumulate > m_vDelay.at(m_iCurFrameIndex)) {
            m_fAccumulate = 0.f;
            m_iCurFrameIndex++;
            
            if (m_iCurFrameIndex < m_vFrameName.size()) {
                CCSpriteFrame* frame = xSpriteFC->spriteFrameByName(m_vFrameName.at(m_iCurFrameIndex).sFrameName.c_str());
                m_preview->setDisplayFrame(frame);
            }
            else
            {
                //所有帧数都播放完.
                m_bRunning = false;
                m_bOnWait = false;
                m_fAccumulate = 0;
                //如果是特效, 设置不可见
                m_preview->setVisible(false);
                
                xScheduler->unscheduleUpdateForTarget(this);
            }
        }
    }
}

CCPoint Part::getOffset()
{
    return ccpSub(m_sprite->getPosition(), m_origin);

}

int Part::getCurFrameIndex()
{
    if (m_bRunning) {        
        return m_iCurFrameIndex;
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
    if (m_bMain) {
        return ccpSub(m_sprite->getPosition(), m_origin);
    }
    else
    {
        return m_sprite->getPosition();
    }
}

void Part::setPosition(CCPoint &point)
{
    if (m_bMain)
    {
        m_sprite->setPosition(ccpAdd(m_origin, point));
        m_preview->setPosition(ccpAdd(m_origin, point));
    }
    else
    {
        m_sprite->setPosition(point);
        m_preview->setPosition(point);
    }
}

void Part::setEnabled(bool bEnabled)
{
    m_bEnabled = bEnabled;
    
    m_sprite->setVisible(m_bEnabled);
    m_preview->setVisible(false);
}

string Part::getEffectName()
{
    bool bNormal = CCUserDefault::sharedUserDefault()->getBoolForKey(NORMAL);
    
    char temp[40];
    
    if (bNormal) {
        sprintf(temp, "n%s%d", makeMotionName().c_str(), m_iAccIndex);
    }
    else
    {
        sprintf(temp, "e%s%d", makeMotionName().c_str(), m_iAccIndex);
    }
    
    return string(temp);
}

bool Part::saveEffectToDictionary(CCDictionary *effects, int iMotionStart)
{
    CCDictionary *effect = new CCDictionary();
   
    string str;
    for (int i = 0; i < m_vNames.size(); i++)
    {
        if (!str.empty()) {
            str += ",";
        }
        str += m_vNames.at(i);
    }
    
    insertString(effect, "fileName", str);
    
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
    
    //flags
    insertString(effect, "flags", flag2string(m_flag));
    
    //speed 只有子弹有速度(FI_MOVE, FI_ISOLATE 为ture)
    if (m_flag[FI_MOVE] && m_flag[FI_ISOLATE])
    {
        insertFloat(effect, "speed", m_speed);
        insertFloat(effect, "degree", m_degree);
    }
    
    bool bRet = true;
    
    if (m_flag[FI_ISOLATE] == false) {
        bRet = saveAttackFrame(effect, iMotionStart);
        
        if (!m_bMain) {
            insertFloat(effect, "attackDuration", m_duration);
            insertFloat(effect, "attackInterval", m_interval);
        }
    }
    
    effects->setObject(effect, getEffectName());
    
    return bRet;
}

bool Part::saveAttackFrame(CCDictionary *effect, int iMotionStart)
{
    string str;
    for (int i = 0; i < m_vAtkFrame.size(); i++)
    {
        if (!str.empty()) {
            str += ",";
        }
        str += any2string(m_vAtkFrame.at(i) + 1 - iMotionStart - iStartFrameIndex);
    }
    
    if (!str.empty()) {
        insertString(effect, "attackFrame", str);
        
        return true;
    }
    else
    {
        return false;
    }
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

void Part::setAllFlags(bool *bFlag)
{
    memcpy(m_flag, bFlag, FI_MAX * sizeof(bool));
}

bool Part::getFlags(flagIndex index)
{
    return m_flag[index];
}

CCSprite* Part::getSprite()
{
    return m_sprite;
}

CCSprite* Part::getPreview()
{
    return m_preview;
}

void Part::setDegree(float degree)
{
    m_degree = degree;
    setRotate(360 - m_degree);
}

float Part::getDegree()
{
    return m_degree;
}

void Part::setSpeed(float speed)
{
    m_speed = speed;
}

float Part::getSpeed()
{
    return m_speed;
}

void Part::saveNames(vector<string> &vNames)
{
    for (int i = 0; i < vNames.size(); i++) {
        m_vNames.push_back(vNames.at(i));
    }
    
}

string Part::makeMotionName()
{
    string str;
    
    int iPos = m_sMotionName.find('_');
    
    if (iPos != string::npos) {
        str = m_sMotionName.substr(iPos + 1);
        
        return str;
    }
    else
    {
        return m_sMotionName;
    }
}

void Part::setAtkFrame(int atkFrame, bool bChecked)
{
    //查找是否已经存在
    vector<int>::iterator it;
    for (it = m_vAtkFrame.begin(); it != m_vAtkFrame.end(); it++)
    {
        int value = (*it);
        if (value == atkFrame)
        {
            if (bChecked == false)
            {
                m_vAtkFrame.erase(it);
            }
            break;
        }
    }
    
	if (bChecked && it == m_vAtkFrame.end()) {
        m_vAtkFrame.push_back(atkFrame);
    }
}


void Part::setAllAtkFrame(CCString *atkFrames, int iMotionStart)
{
    string str = atkFrames->getCString();
    vector<string> vAtkFrames;
    
    string2Vector(str, vAtkFrames);
    
    for (int i = 0; i < vAtkFrames.size(); i++)
    {
        int atkFrame = atoi(vAtkFrames.at(i).c_str());
        atkFrame = atkFrame + iMotionStart + iStartFrameIndex - 1;
        m_vAtkFrame.push_back(atkFrame);
    }
}


bool Part::getAtkFrame(int iAllIndex)
{
    vector<int>::iterator it;
    for (it = m_vAtkFrame.begin(); it != m_vAtkFrame.end(); it++)
    {
        int value = (*it);
        if (value == iAllIndex)
        {
            return true;
        }
    }
    
    return false;
}


void Part::setAccIndex(int iAcc)
{
    m_iAccIndex = iAcc;
}

