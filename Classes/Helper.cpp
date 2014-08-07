//
//  Helper.cpp
//  Tool
//
//  Created by luoxp on 14-7-31.
//
//

#include "Helper.h"
#include "IncludeForCpp.h"


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
