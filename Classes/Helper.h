//
//  Helper.h
//  Tool
//
//  Created by luoxp on 14-7-31.
//
//

#ifndef __Tool__Helper__
#define __Tool__Helper__

#include "IncludeForHeader.h"

int cycleNum(bool bAdd, int iCount, int *idx);

Button * initButton(int iTag, Layout* root, CCObject *target, SEL_TouchEvent selector);

TextField * initTextField(int iTag, Layout* root);

int getNumber(string &str);

void bubble_sort(vector<FramesName> &vFrameName);

#endif /* defined(__Tool__Helper__) */
