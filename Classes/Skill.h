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

class Skill : public CCNode{
    
public:
    static Skill* getInstance(void);
    ~Skill();
    
    void init(CCPoint location, CCPoint preview, CCNode *parent);
    void importPart(const char *pFileName);
    const char * getCurFrameName();
    int getMotionCount();
    void addMotion();
    string & getMotionName(int i);
    int getCurAtkIndex();
    void setCurAtkIndex(int i, setOperateType type);
    void setAtkDelay(float var);
    float getAtkDelay();
    int getFrameCount();
    void checkAllIndex();
    
    void nextFrame(int iCount);
    void preFrame(int iCount);
    
    void setDragAndDropOffset(CCPoint &point);
    
    Motion *m_curMotion = NULL;
    
    int         m_iCurIndex = -1;           //当前帧数, 只有在当前帧数大于最后1个Motion的帧数的时候, 才能进行AddIndex. 删除也只能删除最后一个Motion
    
    void preview();
    void update(float delta);
    void clear();
    void save();
    void saveOfActions(CCDictionary *dic);
    void saveAtksAndEffect(CCDictionary *dic);
    void saveOfEffects(CCDictionary *dic);
    
    bool checkCanAddMotion();
    void deletePart();
    
private:
    void setCurIndex(int idx);
    
    vector<Motion *>    m_vMotion;
    vector<FramesName>  m_vFrameName;
    int         m_iFrameCount = 0;          //这个来判断是否导入了主体
    int         m_iLastIndex = 0;
    int         m_iCurAtk = -1;
    
    CCPoint     m_origin;
    CCPoint     m_showForPreview;
    CCNode      *m_parent;
    
    string      sSkillName;
    
    CCSprite    *m_sprite;                  //占位sprite
    bool        m_bInPreview = false;
    int         m_iPreviewIndex = 0;
    
    CCDictionary *m_dictionary = NULL;
    
    int         m_iMotionAccIndex = 1;            //删除的时候需要减少这个值
};



#endif /* defined(__Tool__Skill__) */
