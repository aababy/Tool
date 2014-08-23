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
    clear();
    CC_SAFE_RELEASE(m_dictionary);
    CCDirector::sharedDirector()->getScheduler()->unscheduleUpdateForTarget(this);
}

void Skill::importPart(vector<string> &vNames)
{
    //先判断, 是否是第一次导入Part, Part作为主体
    if (m_iFrameCount == 0) {
        //加入Cache
        
        for (int i = 0; i < vNames.size(); i++) {
            CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(vNames.at(i).c_str());
            
            m_dictionary = CCDictionary::createWithContentsOfFile(vNames.at(i).c_str());
            m_dictionary->retain();
            CCDictionary *framesDict = (CCDictionary*)m_dictionary->objectForKey("frames");
            
            CCDictElement* pElement = NULL;
            CCDICT_FOREACH(framesDict, pElement)
            {
                FramesName frame;
                frame.sFrameName = pElement->getStrKey();
                frame.iNumber = getNumber(frame.sFrameName);
                m_vFrameName.push_back(frame);
            }
        }
        
        bubble_sort(m_vFrameName);
        
        m_iFrameCount = m_vFrameName.size();
        
        sSkillName = vNames;
        
        setCurIndex(0);
        m_sprite = CCSprite::createWithSpriteFrameName(getCurFrameName());
        m_sprite->setPosition(m_origin);
        m_parent->addChild(m_sprite);
    }
    else
    {
        //判断当前帧所在Atk, 将Part作为Effect插入
        m_curMotion->importPart(vNames);
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
    if (checkCanAddMotion()) {
        vector<string> vFrameName;
        
        for (int i = m_iLastIndex; i < m_iCurIndex + 1; i++) {
            string str = m_vFrameName.at(i).sFrameName;
            vFrameName.push_back(str);
        }
        
        char buffer[20];
        sprintf(buffer, "skillpart_%d", getMotionCount() + 1);
        
        CCAssert(m_iLastIndex <= m_iCurIndex, "error");
        m_curMotion = new Motion(buffer, sSkillName, vFrameName, m_iLastIndex, m_iCurIndex, m_origin, m_showForPreview, m_parent, m_iMotionAccIndex);
        m_vMotion.push_back(m_curMotion);
        m_iMotionAccIndex++;
        
        m_iLastIndex = m_iCurIndex + 1;
        
        setCurAtkIndex(getMotionCount() - 1, OT_NEW);
        
        //增加atk后, 会回到新的atk的第一帧, (现在不这样做了)
        //setCurIndex(m_curMotion->getLastFrameIndex());
        
        xNotify->postNotification(UPDATE_ALL_INDEX);
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
                setCurIndex(m_curMotion->iStart);
            }
        }
        
        //这个post几乎是同步的, 不是异步的, 所以写最后
        xNotify->postNotification(UPDATE_MOTION_LIST);
        xNotify->postNotification(UPDATE_EFFECT_LIST);
    }
    
    if (type == OT_SELECT) {
        xNotify->postNotification(UPDATE_PROPERTY);
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
    m_bPreviewAll = true;
    
    //通知第1个Motion进行Preview, 仅仅是第1个.
    if (! m_vMotion.empty()) {
        m_bInPreview = true;
        m_iPreviewIndex = 0;
        m_vMotion.at(0)->preview();
    }
}

void Skill::previewSingle()
{
    m_bPreviewAll = false;
    
    //通知当前Motion进行Preview, 仅仅是当前.
    if (m_curMotion != NULL) {
        m_bInPreview = true;
        m_iPreviewIndex = m_iCurAtk;
        m_curMotion->preview();
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
            
            if (m_iPreviewIndex < getMotionCount() && m_bPreviewAll == true) {
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

void Skill::clear()
{
    for (int i = 0; i < getMotionCount(); i++) {
        m_vMotion.at(i)->clear();
    }
    
    m_vMotion.clear();
    checkAllIndex();            //任何时候删除Motion都要调用这个函数.
    m_curMotion = NULL;
    m_iCurAtk = -1;
    m_iLastIndex = 0;
    
    xNotify->postNotification(UPDATE_MOTION);
    xNotify->postNotification(UPDATE_MOTION_LIST);
    xNotify->postNotification(UPDATE_ALL_INDEX);
}

void Skill::save(bool bSkill)
{
    if(getMotionCount() != 0)
    {
        m_bSkill = bSkill;
        
        CCDictionary *dic = CCDictionary::createWithContentsOfFile(sTotalPlist.c_str());
        prepareTotalPlist(dic);
        
        ////////////////////////////////////////////////////////////////////////////////////////////////    修改内容
        
        saveAtksAndEffect(dic);
        
        ////////////////////////////////////////////////////////////////////////////////////////////////    写文件
        
        string str = xData->getStringForKey(SAVE_PATH);
        
        if (!str.empty() && str.compare("") != 0) {
            if (str.rfind('/') == str.length() - 1) {
                str = str.substr(0, str.length() - 1);
            }
            
            char stringBuffer[250];
            sprintf(stringBuffer, "%s/%s", str.c_str(), sTotalPlist.c_str());
            dic->writeToFile(stringBuffer);
        }
    }
}

//保存 act1
void Skill::saveOfActions(CCDictionary *dic)
{
    //获取内容
    string str;
    for (int i = 0; i < getMotionCount(); i++) {
        if (i != 0) {   //间隔符
            str += ",";
        }
        str += m_vMotion.at(i)->sSaveName;
    }
    
    CCDictionary *dictionary = new CCDictionary();
    insertString(dictionary, "act1", str);
    
    dic->setObject(dictionary, "acts");
}

//保存 atks 和 effects, 最好同时保存, 节省效率
void Skill::saveAtksAndEffect(CCDictionary *dic)
{
    //判断是否已经有这些字段
    CCDictionary *atks = NULL;
    
    if (m_bSkill)
    {
        atks = (CCDictionary *)dic->objectForKey("atks");
    }
    else
    {
        atks = (CCDictionary *)dic->objectForKey("normals");
    }
    
    if (atks == NULL) {
        atks = new CCDictionary();
    }
    
    CCDictionary *effects = (CCDictionary *)dic->objectForKey("effects");
    if (effects == NULL) {
        effects = new CCDictionary();
    }

    
    for (int i = 0; i < getMotionCount(); i++) {
        Motion *motion = m_vMotion.at(i);
        
        CCDictionary *dictionary = new CCDictionary();
        
        insertFloat(dictionary, "delay", motion->getDelay());
        
        CCDictionary *eff = new CCDictionary();
        motion->getEffectsName(eff, effects);               //effect 是这个函数加入的.
        dictionary->setObject(eff, "effects");

        insertString(dictionary, "fileName", motion->sResName);
        
        //插入frames
        char buff[100];
        sprintf(buff, "%s,%d", motion->m_vFrameNameOrdered.at(0).c_str(), motion->getFramesCount());
        insertString(dictionary, "frames", buff);
        
        insertString(dictionary, "soundFileName", "");
        
        insertString(dictionary, "flags", getMotionFlags());
        
        //插入整个atk
        atks->setObject(dictionary, motion->sSaveName);
    }
    
    if (m_bSkill)
    {
        dic->setObject(atks, "atks");
    }
    else
    {
        dic->setObject(atks, "normals");
    }
    
    dic->setObject(effects, "effects");
}


string Skill::getMotionFlags()
{
    string str;
    if (m_curMotion->getFlags(FI_MOVE))
    {
        str = "1";
    }
    else
    {
        str = "0";
    }
    
    return str;
}


bool Skill::checkCanAddMotion()
{
    if (getMotionCount() != 0) {
        if (m_iCurIndex > m_vMotion.at(getMotionCount() - 1)->iEnd) {
            return true;
        }
        
        return false;
    }
    else
    {
        return true;
    }
}

void Skill::deletePart()
{
    //不改变当前帧数, (如果当前帧在Part内, 就不动, 如果不在, 就显示默认sprite)
    if(getMotionCount() != 0)
    {
        //删除最后一个Part, 判断是否需要改变atk
        Motion *motion = m_vMotion.at(getMotionCount() - 1);
        delete motion;
        m_vMotion.pop_back();
        
        checkAllIndex();
        
        m_iMotionAccIndex--;
        
        
        if (getMotionCount() == 0) {
            //m_iCurIndex = 0;
            m_iLastIndex = 0;
        }
        else
        {
            m_iLastIndex = m_vMotion.at(getMotionCount() - 1)->iEnd + 1;
        }
        
        xNotify->postNotification(UPDATE_MOTION);
        xNotify->postNotification(UPDATE_MOTION_LIST);
        xNotify->postNotification(UPDATE_ALL_INDEX);
    }
}

void Skill::setTotalPlist(string &str)
{
    sTotalPlist = str;
}

void Skill::prepareTotalPlist(CCDictionary *dic)
{
    //去掉无关的项目
    dic->removeObjectForKey("seqActions");
    dic->removeObjectForKey("special_effects");
    
    //去掉atk开头的actions
    CCDictionary *actions = (CCDictionary *)dic->objectForKey("actions");
    
    CCArray *array = actions->allKeys();
    for (int i = 0; i < array->count(); i ++)
    {
        //获取key的方法
        CCString *key =  (CCString*)array->objectAtIndex(i);
        string strKey = key->getCString();

        if (strKey.find("atk") != string::npos) {
            actions->removeObjectForKey(key->getCString());
        }
    }
}


