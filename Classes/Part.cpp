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
        
        m_vFrameUsed.push_back(frames);
    }
    
    m_iFrameCount = m_vFrameUsed.size();
    
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



Part::Part(vector<string> &vNames, CCPoint &show, CCPoint &origin, CCPoint &showForPreview, CCNode *parent, CCNode *partner, int iAcc, string& sMotionName)
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
            m_vFrameUsed.push_back(frame);
        }
    }
    
    m_iFrameCount = m_vFrameUsed.size();
    
    bubble_sort(m_vFrameUsed);

    m_partnerForPreview = partner;
    CCNode *parentForPreview = m_partnerForPreview->getParent();

    m_parent = parent;
    m_partnerForFrame = parent;
    m_origin = origin;
    m_showForPreview = showForPreview;
    m_iAccIndex = iAcc;
    m_sMotionName = sMotionName;

    //帧显示
    m_sprite = CCSprite::createWithSpriteFrameName(getCurFrameName());
    m_sprite->setPosition(origin);
    m_partnerForFrame->getParent()->addChild(m_sprite);

    //预览显示
    if (m_sMotionName.compare("p_120601") == 0 || m_sMotionName.compare("p_120602") == 0) {
        m_preview = CCSprite::create();
        m_preview->setVisible(false);
        m_partnerForPreview->addChild(m_preview);
    }
    else
    {
        m_preview = CCSprite::create();
        CCSize size = parentForPreview->getContentSize();
        m_preview->setPosition(ccp(size.width/2, size.height/2));
        m_preview->setVisible(false);
        //也改为直接加到背景上面.实际坐标再转换.
        parentForPreview->addChild(m_preview);
    }
    
    makeAPartName();
    
    //默认特效*2
    setScale(2.f);
    
    init(vNames);
}


void Part::parseSoundFile(const string &soundFile)
{
    if(soundFile.empty())
    {
        return;
    }
    
    string parse = soundFile;
    
    while (true)
    {
        //查找 逗号,
        size_t pos = parse.find(',');
        
        //如果找到, 查找 '(' 左括号, 将前面的字符串转换为 filename. 查找 ')' 右括号, 将括号间的字符串, 转换为frameIndex,
        if (pos != string::npos)
        {
            size_t posLeft = parse.find('(');
            size_t posRight = parse.find(')');
            
            AudioInfo audio;
            audio.filename = parse.substr(0, posLeft).c_str();
            audio.frameIndex = atoi(parse.substr(posLeft + 1, posRight - posLeft - 1).c_str());
            
            _sounds.push_back(audio);
            
            //将剩下的字符串组成一个新的字符串, 重新查找
            parse = parse.substr(pos + 1, parse.length() - posRight);
        }
        //如果没找到. 表示这是最后一个音效, 转换完成后, 退出循环
        else
        {
            size_t posLeft = parse.find('(');
            size_t posRight = parse.find(')');
            
            AudioInfo audio;
            audio.filename = parse.substr(0, posLeft).c_str();
            
            if(posLeft != string::npos)
            {
                audio.frameIndex = atoi(parse.substr(posLeft + 1, posRight - posLeft - 1).c_str());
                _sounds.push_back(audio);
            }
            else
            {   //0 表示攻击帧放
                audio.frameIndex = -1;
                _atkSound = audio;
            }
            
            break;
        }
    }
}



void Part::makeNewPart()
{
    m_vFrameUsed.clear();           //清除以前的.

    //加入Cache
    for (int i = 0; i < m_vFrameOriginal.size(); i++) {

        if(m_vFrameOriginal.at(i).bDeleted == false)
        {
            m_vFrameUsed.push_back(m_vFrameOriginal.at(i));
        }
    }

    m_iFrameCount = m_vFrameUsed.size();
}


void Part::init(vector<string> &vNames)
{
    sPartName = vNames.at(0);               //PartName 显示只会显示一个
    
    for(int i = 0; i < FI_MAX; i++)
    {
        m_flag[i] = 0;
    }
    
    m_duration = 0.f;
    m_interval = 0.f;
    
    saveNames(vNames);
    
    //间隔帧
    for (int i = 0; i < m_iFrameCount; i++)
    {
        float delay = 0.05f;
        m_vDelay.push_back(delay);

        //复制到original里面
        m_vFrameUsed.at(i).bDeleted = false;
        m_vFrameOriginal.push_back(m_vFrameUsed.at(i));
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
    
    sprintf(buffer, "%s (%lu)", sPartName.c_str(), m_vFrameUsed.size());
    
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
    return m_vFrameUsed.at(m_iCurIndex).sFrameName.c_str();
}

void Part::setFrame(int iIndexStartFromMain)
{
    if (iStartFrameIndex <= iIndexStartFromMain) {
        m_iCurIndex = iIndexStartFromMain - iStartFrameIndex;
        
        if (m_iCurIndex < m_vFrameUsed.size()) {
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
        m_iCurFrameIndex = 0;
        m_fAccumulate = 0;
        
        _atkSound.frameIndex = -1;
        for (int i = 0; i < _sounds.size(); i++) {
            _sounds.at(i).played = false;
        }

        if (m_bMain) {
            //跳回来的时候不需要重新设置坐标
            if (m_sMotionName.compare("p_120602") != 0) {
                
                m_preview->setPosition(ccpAdd(m_showForPreview, getOffset()));
            }
            else
            {
                CCPoint pos = ccpAdd(m_showForPreview, getOffset());
                pos = ccpAdd(pos, ccp(800, 0));
                m_preview->setPosition(pos);
            }
        }
        else
        {
            CCPoint posDiff, pos;
            
            if (m_sMotionName.compare("p_120601") == 0 || m_sMotionName.compare("p_120602") == 0)
            {
                //1. 获取坐标差值.
                posDiff = ccpSub(m_sprite->getPosition(), m_partnerForFrame->getPosition());
                
                //2. 获取主体的坐标
                pos = m_partnerForPreview->getPosition();
                
                //3. 坐标加上差值
                pos = ccpAdd(pos, posDiff);
                
                //这个坐标转换为世界坐标
                pos = m_partnerForPreview->getParent()->convertToWorldSpace(pos);
                pos = m_partnerForPreview->convertToNodeSpace(pos);
                
                m_preview->setPosition(pos);
            }
            else
            {
                //特效, 重新算坐标
                //1. 获取坐标差值.
                posDiff = ccpSub(m_sprite->getPosition(), m_partnerForFrame->getPosition());
                
                //2. 获取主体的坐标
                pos = m_partnerForPreview->getPosition();
                
                //3. 坐标加上差值
                pos = ccpAdd(pos, posDiff);
                
                m_preview->setPosition(pos);
            }
        }
        posStart = m_preview->getPosition();
        m_iMoveIndex = 0;
        m_iMoveCount = 1000 / (m_speed/60);
        
        //开始设置Frame
        CCSpriteFrame* frame = xSpriteFC->spriteFrameByName(m_vFrameUsed.at(0).sFrameName.c_str());
        m_preview->setDisplayFrame(frame);
        if(!m_bMain)
        {
            if(m_flag[FI_HIGH_LIGHT] == 0)
            {
                ccBlendFunc tmp_oBlendFunc = {GL_SRC_ALPHA, GL_ONE};
                m_preview->setBlendFunc(tmp_oBlendFunc);
            }
            else
            {
                ccBlendFunc tmp_oBlendFunc = {GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA};
                m_preview->setBlendFunc(tmp_oBlendFunc);
            }
        }
        
        xScheduler->unscheduleUpdateForTarget(this);
        xScheduler->scheduleUpdateForTarget(this, 0, false);
        
        m_preview->setVisible(true);
        m_bRunning = true;
    }
}

void Part::update(float delta)
{
    if (m_bRunning) {
        //播放音效
        for (int i = 0; i < _sounds.size(); i++) {
            if (_sounds.at(i).frameIndex == m_iCurFrameIndex && !_sounds.at(i).played) {
                _sounds.at(i).played = true;
                xAudio->playEffect(_sounds.at(i).filename);
            }
        }
        
        //攻击帧音效
        if (getAtkFrame(_iMotionStart + m_iCurFrameIndex)) {
            CCLOG("%s", _atkSound.filename.c_str());
            if (_atkSound.frameIndex != m_iCurFrameIndex && !_atkSound.filename.empty()) {
                _atkSound.frameIndex = m_iCurFrameIndex;
                xAudio->playEffect(_atkSound.filename);
            }
        }
        
        //累积时间
        m_fAccumulate += delta;
        
        //如果累积时间大于帧间隔, 清空, 然后播放下一帧
        if (m_fAccumulate > m_vDelay.at(m_iCurFrameIndex)) {
            m_fAccumulate = 0.f;
            m_iCurFrameIndex++;
            m_iMoveIndex++;
            
            if (m_iCurFrameIndex < m_vFrameUsed.size()) {
                CCSpriteFrame* frame = xSpriteFC->spriteFrameByName(m_vFrameUsed.at(m_iCurFrameIndex).sFrameName.c_str());
                m_preview->setDisplayFrame(frame);
                if(!m_bMain && m_flag[FI_HIGH_LIGHT] == 0)
                {
                    ccBlendFunc tmp_oBlendFunc = {GL_SRC_ALPHA, GL_ONE};
                    m_preview->setBlendFunc(tmp_oBlendFunc);
                }
                
                //位移操作
                if(m_flag[FI_MOVE])
                {
                    //根据帧数确定时间
                    CCPoint dest = ccp(m_speed / 60, 0);       //水平
                    dest = pointRotateWithAngle(dest, m_degree);
                    
                    //第一帧就要开始移动
                    CCPoint posCur = m_preview->getPosition();
                    
                    posCur.x += dest.x;
                    posCur.y += dest.y;
                    
                    m_preview->setPosition(posCur);
                }
                
                if (m_bMain) {
                    if (m_sMotionName.compare("p_120601") == 0)
                    {
                        CCPoint dest = ccp(160, 0);       //水平
                        
                        //第一帧就要开始移动
                        CCPoint posCur = m_preview->getPosition();
                        
                        posCur.x += dest.x;
                        posCur.y += dest.y;
                        
                        m_preview->setPosition(posCur);
                    }
                    else if (m_sMotionName.compare("p_120602") == 0)
                    {
                        CCPoint dest = ccp(-133, 0);       //水平
                        
                        //第一帧就要开始移动
                        CCPoint posCur = m_preview->getPosition();
                        
                        posCur.x += dest.x;
                        posCur.y += dest.y;
                        
                        m_preview->setPosition(posCur);
                    }
                }
            }
            else
            {
                //所有帧数都播放完.
                if(!m_flag[FI_MOVE])
                {
                    m_bRunning = false;
                }
                else
                {
                    if(m_iMoveIndex > m_iMoveCount)
                    {
                        m_bRunning = false;
                    }
                    else
                    {
                        //否则重新开始
                        m_iCurFrameIndex = 0;
                        CCSpriteFrame* frame = xSpriteFC->spriteFrameByName(m_vFrameUsed.at(0).sFrameName.c_str());
                        m_preview->setDisplayFrame(frame);
                        if(!m_bMain && m_flag[FI_HIGH_LIGHT] == 0)
                        {
                            ccBlendFunc tmp_oBlendFunc = {GL_SRC_ALPHA, GL_ONE};
                            m_preview->setBlendFunc(tmp_oBlendFunc);
                        }
                    }
                }

                if(!m_bRunning)
                {
                    m_bOnWait = false;
                    m_fAccumulate = 0;
                    //如果是特效, 设置不可见
                    m_preview->setVisible(false);

                    xScheduler->unscheduleUpdateForTarget(this);
                }
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


void Part::setStartFrameIndex(int iStart)
{
    iStartFrameIndex = iStart;
    iStartFrameIndexOriginal = iStartFrameIndex;
}

int Part::getStartFrameIndex()
{
    return iStartFrameIndex;
}


void Part::setMain()
{
    m_bMain = true;    
}

bool Part::getMain()
{
    return m_bMain;
}

CCPoint Part::getPosition()
{
    if (m_bMain) {
        return ccpSub(m_sprite->getPosition(), m_origin);
    }
    else
    {
        CCPoint pos = m_sprite->getParent()->convertToWorldSpace(m_sprite->getPosition());
        return m_partnerForFrame->convertToNodeSpace(pos);
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
        CCPoint pos = m_partnerForFrame->convertToWorldSpace(point);
        pos = m_sprite->getParent()->convertToNodeSpace(pos);
        m_sprite->setPosition(pos);
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
    saveDelay(effect);
    
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

void Part::saveDelay(CCDictionary *effect)
{
    float fOld = -1.f;
    string str;
    char buffer[40];
    int i;
    
    for (i = 0; i < m_vDelay.size(); i++)
    {
        if (isEqualFloat(fOld, m_vDelay.at(i)) == false)
        {
            if (i != 0)
            {
                sprintf(buffer, "(%d),", i);
                str += buffer;
            }
            
            fOld = m_vDelay.at(i);
            str += any2string(fOld);
        }
    }
    
    sprintf(buffer, "(%d)", i);
    str += buffer;
    
    insertString(effect, "delay", str);
}


void Part::saveReduceFrame(ofstream &fout)
{
    //写入资源名 xxx.plist
    fout << sPartName << endl;

    //写入缺少的帧名称
    for(int i = 0; i < m_vFrameOriginal.size(); i++)
    {
        if(m_vFrameOriginal.at(i).bDeleted)
        {
            fout << m_vFrameOriginal.at(i).sFrameName << endl;
        }
    }

    fout << endl;
}


void Part::parseDelay(CCString *delay)
{
    string str = delay->getCString();
    
    vector<string> vLines;
    string2Vector(str, vLines);
 
    m_vDelay.clear();
    
    int lastIndex = 0;
    
    for (int i = 0; i < vLines.size(); i++)
    {
        string line = vLines.at(i);
        
        int left = line.find('(');
        int right = line.find(')');
        
        string sDelay = line.substr(0, left);
        string sCount = line.substr(left + 1, right - left - 1);
        
        float delay = atof(sDelay.c_str());
        int count = atoi(sCount.c_str());
        int temp = count;
        count -= lastIndex;
        lastIndex = temp;
        
        for (int j = 0; j < count; j++)
        {
            m_vDelay.push_back(delay);
        }
    }
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

void Part::saveSoundFileName(CCDictionary *dic)
{
    if (!_atkSound.filename.empty()) {
        insertString(dic, "soundFileName", _atkSound.filename);
    }
    else
    {
        string str;
        for (int i = 0; i < _sounds.size(); i++)
        {
            if (!str.empty()) {
                str += ",";
            }
            
            str += _sounds.at(i).filename;
            str += "(" + any2string(_sounds.at(i).frameIndex) + ")";
        }
        
        insertString(dic, "soundFileName", str);
    }
}

void Part::setDelay(int idx, float delay)
{
    if (idx >= iStartFrameIndex && idx - iStartFrameIndex < m_iFrameCount) {
        m_vDelay.at(m_iCurIndex) = delay;
    }
}

void Part::setDelay(float delay)
{
    for(int i = 0; i < m_vDelay.size(); i++)
    {
        m_vDelay.at(i) = delay;
    }
}


float Part::getDelay(int idx)
{
    if (idx >= iStartFrameIndex && idx - iStartFrameIndex < m_iFrameCount) {
        return m_vDelay.at(m_iCurIndex);
    }
    else
    {
        return -1;
    }
}

void Part::setFlags(flagIndex index, int bFlag)
{
    m_flag[index] = bFlag;
}

void Part::setAllFlags(int *bFlag)
{
    memcpy(m_flag, bFlag, FI_MAX * sizeof(int));
}

int Part::getFlags(flagIndex index)
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


void Part::addAudio(const string& filename, int globalIndex)
{
    _atkSound.filename.clear();         //一旦添加音效后, 攻击帧音效就失效
    deleteCurIndexAudio(globalIndex - _iMotionStart);
    
    AudioInfo info;
    int iDot = filename.rfind('.');
    if (iDot != -1) {
        info.filename = filename.substr(0, iDot);
        
    }
    info.frameIndex = globalIndex - _iMotionStart;
    
    _sounds.push_back(info);
}


string Part::getAudio(int index)
{
    string audio;
    for (int i = 0; i < _sounds.size(); i ++) {
        if (_sounds.at(i).frameIndex == index) {
            return _sounds.at(i).filename;
        }
    }
    return audio;
}


void Part::deleteCurIndexAudio(int localIndex)
{
    for (vector<AudioInfo>::iterator it = _sounds.begin(); it != _sounds.end(); it++) {
        if (it->frameIndex == localIndex) {
            _sounds.erase(it);
            break;
        }
    }
}

void Part::setMotionStart(int iMotionStart)
{
    _iMotionStart = iMotionStart;
}
