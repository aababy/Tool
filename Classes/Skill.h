//
//  Skill.h
//  Tool
//
//  Created by luoxp on 14-8-6.
//
//  这只是一个管理Motion的类, 不负责显示, 显示交给Motion处理

#ifndef __Tool__Skill__
#define __Tool__Skill__

#include "IncludeForHeader.h"

#define xSkill                  (Skill::getInstance())
#define xCurAtk                 ((Skill::getInstance())->m_curMotion)

class Motion;

class Skill {
    
public:
    static Skill* getInstance(void);
    
    void init(CCPoint location, CCPoint preview, CCNode *parent);
    void importPart(const char *pFileName);
    const char * getCurFrameName();
    int getMotionCount();
    void addMotion();
    string & getMotionName(int i);
    int getCurAtkIndex();
    void setCurAtkIndex(int i);
    void setAtkDelay(float var);
    float getAtkDelay();
    int getFrameCount();
    
    void nextFrame(int iCount);
    void preFrame(int iCount);
    
    Motion *m_curMotion = NULL;
    
    int         m_iCurIndex = 0;           //当前帧数, 只有在当前帧数大于最后1个Motion的帧数的时候, 才能进行AddIndex. 删除也只能删除最后一个Motion
    
private:
    vector<Motion *>    m_vMotion;          //只有主体有这个字段.
    vector<FramesName>  m_vFrameName;
    int         m_iFrameCount = 0;          //这个来判断是否导入了主体
    
    int         m_iLastIndex = 0;
    int         m_iCurAtk = 0;
    int         m_iCurMotion;

    
    CCPoint     m_origin;
    CCPoint     m_showForPreview;
    CCNode      *m_parent;
    
    string      sPartName;
};



#endif /* defined(__Tool__Skill__) */
