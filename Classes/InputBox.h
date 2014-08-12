//
//  InputBox.h
//  Tool
//
//  Created by luoxp on 14-8-1.
//
//

#ifndef __Tool__InputBox__
#define __Tool__InputBox__

#include "IncludeForHeader.h"

class InputBox
{
public:
    static InputBox* create(Layout *text, CCEditBoxDelegate* delegate, CCNode *parent);
    static InputBox* create(int iTag, Layout *root, CCEditBoxDelegate* delegate, CCNode *parent);
    
    CCEditBox *m_edit;
    
    void setText(string str);
    const char * getText();
    void setVisible(bool visible);
    
    Layout *originLayout;
};



#endif /* defined(__Tool__InputBox__) */
