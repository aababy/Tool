//
//  Motion.h
//  Tool
//
//  Created by luoxp on 14-8-6.
//
//  每个Motion 拥有一个PartManager,

#ifndef __Tool__Motion__
#define __Tool__Motion__

#include "IncludeForHeader.h"

class Motion {
    
public:
    Motion(string sName, vector<string> &vNames, vector<string> vFrameNameOrdered, int iStart, int iEnd, CCPoint &origin, CCPoint &showForPreview, CCNode *parent, int iMotionAccIndex);
    ~Motion();
    void importPart(vector<string> &vNames);
    Part* importEffect(vector<string> &vNames, int iImportIndex);
    int getPartsCount();
    void setCurOperationIndex(int idx);
    string& getMotionName(int i);
    string& getPartNameByIndex(int idx);
    int getCurOperationIndex();
    void setEnabled(bool bEnabled);
    bool isInMotion(int idx);
    void setFrame(int iFrame);
    int getLastFrameIndex();
    void removeEffect();
    int findRealStartFrameIndex(int iStartFrameIndexOriginal);
    void repairAllEffectStartFrameIndex();
    
    string sMotionName;         //e.g. "0-5" or "6-11"
    string sSaveName;           //e.g. "atk1"
    string sResName;            //e.g. "xxx.plist"
    void setDragAndDropOffset(CCPoint &point);
    void setMotionAccIndex(int iMotionAccIndex);
    
    CCPoint getCurPosition();
    void setCurPosition(CCPoint &point);
    float getCurRotate();
    const CCPoint& getCurAnchorPoint();
    float getCurScale();
    void setCurRotate(float r);
    void setCurAnchorPoint(const CCPoint& point);
    void setCurScale(float s);
    void setMainScale(float s);
    void setEffectScale(float s);

    void preview();
    void clear();
    void update(float delta);
    bool isInPreview();
    int getFramesCount();
    void parseDelay(CCString *delay);
    void setDelay(int idx, float delay);
    void setDelay(float delay);
    float getDelay(int idx);
    void setFlags(flagIndex index, int bFlag);
    int getFlags(flagIndex index);
    bool getMainFlags(flagIndex index);
    void setDegree(float degree);
    float getDegree();
    void setSpeed(float speed);
    float getSpeed();
    
    void setAtkFrame(int atkFrame, bool bChecked);
    bool getAtkFrame(int iAllIndex);
    void setDuration(float duration);
    float getDuration();
    void setInterval(float interval);
    float getInterval();
    
    bool getEffectsName(CCDictionary *dic, CCDictionary *effects);
    bool saveMainAttackFrame(CCDictionary *dic);
    void setMainAllAtkFrame(CCString *atkFrames);
    void saveReduceFrame(ofstream &fout);
    void saveMainSoundFileName(CCDictionary *dic);

    vector<string> m_vFrameNameOrdered;
    
    bool isMainIndex();
    void saveMainDelay(CCDictionary *dic);
    void addAudio(const string& filename, int index);

    int iEnd = 0;
    int iStart = 0;

    Part* getCurPart();
    void parseSoundFile(const string& filename);
    string getAudio(int index);
    
private:
   
    vector<Part*> m_vParts;
    
    CCPoint     location;
    CCPoint     m_preview;
    CCNode      *m_iFrameBG;
    
    Part*       m_mainPart;
    
    int         m_iMainIndex = 0;           //这个是相对的?
    int         m_iCurOperationIndex = 0;
    
    bool        m_bEnabled;
    bool        m_bInPreview = false;
    
    int         m_iAccIndex = 1;
    
    CCSprite*   m_show;
    CCSprite*   m_showPreview;
};



#endif /* defined(__Tool__Motion__) */
