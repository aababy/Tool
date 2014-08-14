//
//  InputBox.cpp
//  Tool
//
//  Created by luoxp on 14-8-1.
//
//
#include "IncludeForCpp.h"
#include "InputBox.h"


InputBox* InputBox::create(Layout *layout, CCEditBoxDelegate* delegate, CCNode *parent)
{
	layout->setVisible(false);

    InputBox *input = new InputBox();
	input->originLayout = layout;
	layout->setTouchEnabled(true);
	   
    CCPoint absolute = layout->convertToWorldSpaceAR(CCPointZero);
    absolute.x += layout->getSize().width/2;
    absolute.y += layout->getSize().height/2;
    
    input->m_edit = CCEditBox::create(layout->getSize(), CCScale9Sprite::create("R/input_bg.png"));
    input->m_edit->setPosition(absolute);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    input->m_edit->setFontName("Paint Boy");
#else
	//input->m_edit->setFontName("fonts/Paint Boy.ttf");
#endif
    
    input->m_edit->setFontSize(20);
    input->m_edit->setFontColor(ccWHITE);
    input->m_edit->setMaxLength(20);
    input->m_edit->setReturnType(kKeyboardReturnTypeDone);
    input->m_edit->setInputMode(kEditBoxInputModeSingleLine);
    input->m_edit->setDelegate(delegate);
    parent->addChild(input->m_edit);
    
    return input;
}

InputBox* InputBox::create(int iTag, Layout *root, CCEditBoxDelegate* delegate, CCNode *parent)
{
    Layout *layout = static_cast<Layout*>(UIHelper::seekWidgetByTag(root, iTag));
	InputBox *input = create(layout, delegate, parent);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	layout->addTouchEventListener(input, toucheventselector(InputBox::touchEvent));
#endif

	return input;
}

void InputBox::setText(string str)
{
    m_edit->setText(str.c_str());
}

const char * InputBox::getText()
{
    return m_edit->getText();
}

void InputBox::setVisible(bool visible)
{
    m_edit->setVisible(visible);
}

void InputBox::touchEvent(CCObject *pSender, TouchEventType type)
{
	if (type != TOUCH_EVENT_ENDED)
	{
		return;
	}

	m_edit->touchDownAction(pSender, 0);
}