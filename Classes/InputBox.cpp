//
//  InputBox.cpp
//  Tool
//
//  Created by luoxp on 14-8-1.
//
//
#include "IncludeForCpp.h"
#include "InputBox.h"


InputBox* InputBox::create(TextField *text, CCEditBoxDelegate* delegate, CCNode *parent)
{
    InputBox *input = new InputBox();
    input->originText = text;
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    CCPoint absolute = text->convertToWorldSpaceAR(CCPointZero);
    input->m_edit = CCEditBox::create(text->getSize(), CCScale9Sprite::create("R/input_bg.png"));
    input->m_edit->setPosition(absolute);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    input->m_edit->setFontName("Paint Boy");
#else
	input->m_edit->setFontName("fonts/Paint Boy.ttf");
#endif
    
    input->m_edit->setFontSize(20);
    input->m_edit->setFontColor(ccWHITE);
    input->m_edit->setMaxLength(20);
    input->m_edit->setReturnType(kKeyboardReturnTypeDone);
    input->m_edit->setInputMode(kEditBoxInputModeSingleLine);
    input->m_edit->setDelegate(delegate);
    parent->addChild(input->m_edit);
#endif
    
    return input;
}

InputBox* InputBox::create(int iTag, Layout *root, CCEditBoxDelegate* delegate, CCNode *parent)
{
    TextField *text = initTextField(iTag, root);
    return create(text, delegate, parent);
}

void InputBox::setText(string str)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    m_edit->setText(str.c_str());
#else
	originText->setText(str);
#endif
}

const char * InputBox::getText()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    return m_edit->getText();
#else
	return originText->getStringValue();
#endif
}


void InputBox::setVisible(bool visible)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    m_edit->setVisible(visible);
#endif
    originText->setVisible(visible);
}