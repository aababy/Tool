//
//  Helper.cpp
//  Tool
//
//  Created by luoxp on 14-7-31.
//
//

#include "Helper.h"
#include "IncludeForCpp.h"
#include <math.h>


int cycleNum(bool bAdd, int iCount, int *idx)
{
    if (bAdd == true)
    {
        if (*idx >= iCount - 1)
        {
            *idx = 0;
        }
        else
        {
            (*idx)++;
        }
    }
    else if (bAdd == false)
    {
        if ((*idx) <= 0)
        {
            (*idx) = iCount - 1;
        }
        else
        {
            (*idx)--;
        }
    }
    
    return (*idx);
}

Button * initButton(int iTag, Layout* root, CCObject *target, SEL_TouchEvent selector)
{
    Button *btn = (Button*)UIHelper::seekWidgetByTag(root, iTag);
    btn->addTouchEventListener(target, selector);
    
    return btn;
}

TextField * initTextField(int iTag, Layout* root)
{
    TextField* text = static_cast<TextField*>(UIHelper::seekWidgetByTag(root, iTag));
//    text->addEventListenerTextField(target, selector);
    
    return text;
}

CheckBox * initCheckBox(int iTag, Layout* root, CCObject *target, SEL_SelectedStateEvent selector)
{
    CheckBox *checkBox = (CheckBox*)(UIHelper::seekWidgetByTag(root, iTag));
    checkBox->addEventListenerCheckBox(target, selector);
    
    return checkBox;
}


int getNumber(string &str)
{
    int iDot = str.rfind('.');
    
    int iNumber = str.find_last_not_of("0123456789", iDot - 1) + 1;
    
    string sNumber = str.substr(iNumber, iDot - iNumber);
    
    return atoi(sNumber.c_str());
}

void bubble_sort(vector<FramesName> &vFrameName)
{
    int size = vFrameName.size();
    int i, j;
    
    for(j = 0; j < size-1; j++)
    {
        for(i = 0; i < size-1-j; i++)
        {
            if(vFrameName.at(i).iNumber > vFrameName.at(i+1).iNumber)//数组元素大小按升序排列
            {
                swap(vFrameName.at(i), vFrameName.at(i+1));
            }
        }
    }
}

void bubble_sort(vector<Motion* > &vMotion)
{
    int size = vMotion.size();
    int i, j;
    
    for(j = 0; j < size-1; j++)
    {
        for(i = 0; i < size-1-j; i++)
        {
            if(vMotion.at(i)->iStart > vMotion.at(i+1)->iStart)//数组元素大小按升序排列
            {
                swap(vMotion.at(i), vMotion.at(i+1));
            }
        }
    }
}

//写文件时只能保存成string, 不然不能写
void insertInteger(CCDictionary *dic, const string& key, int value)
{
    insertString(dic, key, any2string(value));
}

void insertFloat(CCDictionary *dic, const string& key, float value)
{
    insertString(dic, key, any2string(value));
}

void insertString(CCDictionary *dic, const string& key, const string& value)
{
    CCString *str = CCString::create(value);
    dic->setObject(str, key);
}

void insertCCPoint(CCDictionary *dic, const string& key, CCPoint& value)
{
    char buf[20];
    sprintf(buf, "{%.2f,%.2f}", value.x, value.y);
    string str(buf);
    
    insertString(dic, key, str);
}

void insertArray(CCDictionary *dic, const string& key, CCArray* value)
{
    dic->setObject(value, key);
}

string itostring(int i)
{
    char buffer[10];
    sprintf(buffer, "%d", i);
    return string(buffer);
}


CCPoint pointRotateWithAngle(CCPoint& point, float fDegree)
{
    //角度转弧度
    float radian = M_PI * fDegree /180;
    
    return ccpRotateByAngle(point, CCPointZero, radian);
}

//只对特效使用, 这个函数
string flag2string(int m_flag[FLAG_COUNT])
{
    string str;
    
    for (int i = 0; i < FLAG_COUNT; i++)
    {
        str += any2string(m_flag[i]);
    }
    
    return str;
}

void string2Vector(const string &name, vector<string>& vec)
{
    int start = 0;
    string str;
    while (true) {
        int pos = name.find(',', start);
        if (pos != string::npos) {
            str = name.substr(start, pos - start);
        }
        else
        {
            str = name.substr(start, name.length() - start);
        }
        
        //寻找对应的effect
        vec.push_back(str);
        start += str.length() + 1;
        
        if (start >= name.length()) {
            break;
        }
    }
}

CCPoint str2Point(const char * sz)
{
    string str = sz;
    
    int iStartX = str.find_first_of("0123456789-");
    int iEndX = str.find_first_of(",");
    
    int iStartY = str.find_first_of("0123456789-", iEndX);
    int iEndY = str.find_last_of("0123456789.");
    
    
    string x = str.substr(iStartX, iEndX - iStartX);
    string y = str.substr(iStartY, iEndY - iStartY + 1);
    
    return ccp(atof(x.c_str()), atof(y.c_str()));
}

void string2Flags(CCString *sFlags, int *flags)
{
    //兼容以前的格式
    if (sFlags == NULL)
    {
        for (int i = 0; i < FI_MAX; i++) {
            flags[i] = false;
        }
        return;
    }
    
    string str = sFlags->getCString();
    
    for (int i = 0; i < FI_MAX; i++) {
        if (i >= str.length() || str.at(i) == '0') {
            flags[i] = 0;
        }
        else
        {
            flags[i] = atoi(str.substr(i, 1).c_str());
        }
    }
}

bool isEqualFloat(float f1, float f2)
{
    float fRet = f1 - f2;
    if (fRet > 0.0001f || fRet < -0.0001f) {
        return false;
    }
    else
    {
        return true;
    }
}

