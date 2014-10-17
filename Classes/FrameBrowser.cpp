//
//  FrameBrowser.cpp
//  Tool
//
//  Created by luoxp on 14-10-16.
//
//

#include "IncludeForCpp.h"
#include "FrameBrowser.h"
#include "MainScene.h"

enum UITag
{
    LAYOUT = 100,
    BTN_OK = 393,
    CHECK_BOX = 394,
};

FrameBrowser::FrameBrowser()
{

}

FrameBrowser* FrameBrowser::create()
{
    FrameBrowser* uiLayer = new FrameBrowser();
    if (uiLayer && uiLayer->init())
    {
        uiLayer->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(uiLayer);
    }
    return uiLayer;
}

#define SIDE_LEN                (200)

bool FrameBrowser::init()
{
    if (CCLayer::init())
    {
        m_rootNode = (NodeReader::getInstance()->createNode("R/Frame_1.ExportJson"));
        addChild(m_rootNode);

        Layout *root = static_cast<Layout*>(m_rootNode->getChildren()->objectAtIndex(0));

        initButton(BTN_OK, root, this, toucheventselector(FrameBrowser::touchEvent));
        _list = (UIListView*)UIHelper::seekWidgetByTag(root, 392);

        UICheckBox * check = (UICheckBox*)UIHelper::seekWidgetByTag(root, CHECK_BOX);

        if(xCurAtk == NULL)
        {
            return true;
        }

        Part *part = xCurAtk->getCurPart();
        vector<FramesName> &vFrameName = part->m_vFrameOriginal;

        for(int i = 0; i < vFrameName.size(); i++)
        {
            Layout * layout = Layout::create();
            layout->setContentSize(CCSizeMake(SIDE_LEN, SIDE_LEN));
            layout->setBackGroundColorType(LAYOUT_COLOR_SOLID);
            layout->setBackGroundColor(ccBLACK);
            layout->setSize(CCSizeMake(SIDE_LEN, SIDE_LEN));
            layout->setTouchEnabled(true);
            layout->setTag(LAYOUT);
            layout->addTouchEventListener(this, toucheventselector(FrameBrowser::touchEvent));


            UIImageView * image = UIImageView::create();
            image->loadTexture(vFrameName.at(i).sFrameName.c_str(), UI_TEX_TYPE_PLIST);
            image->setPosition(ccp(SIDE_LEN/2, SIDE_LEN/2));
            image->setScale(0.5);

            UICheckBox * checkOne = (UICheckBox*)check->clone();
            checkOne->setScale(0.5);
            checkOne->setPosition(ccp(SIDE_LEN - 20, SIDE_LEN - 20));
            checkOne->setSelectedState(!part->m_vFrameOriginal.at(i).bDeleted);

            layout->addChild(image);
            layout->addChild(checkOne);
            _list->pushBackCustomItem(layout);
        }

        check->setVisible(false);
        return true;
    }

    return true;
}


void FrameBrowser::touchEvent(CCObject *pSender, TouchEventType type)
{
    if(type != TOUCH_EVENT_ENDED)
    {
        return;
    }

    Widget *widget = (Widget*)pSender;
    int iTag = widget->getTag();

    switch (iTag) {
        case BTN_OK:           //确定
        {
            modifyFrame();
        }
            break;
        case LAYOUT:
        {
            CCArray *array = _list->getItems();

            CCObject *object = NULL;
            CCARRAY_FOREACH(array, object)
                {
                    Layout * layout = (Layout*)object;

                    if (layout == pSender)
                    {
                        UICheckBox * check = (UICheckBox*)UIHelper::seekWidgetByTag(layout, CHECK_BOX);

                        bool bCheck = !(check->getSelectedState());
                        check->setSelectedState(bCheck);
                    }
                }
        }
        default:
            break;
    }
}

void FrameBrowser::modifyFrame()
{
    Part *part = xCurAtk->getCurPart();
    vector<FramesName> &vFrameName = part->m_vFrameOriginal;

    for(int i = 0; i < vFrameName.size(); i++)
    {
        Layout * layout = (Layout*)_list->getItem(i);
        UICheckBox * check = (UICheckBox*)UIHelper::seekWidgetByTag(layout, CHECK_BOX);

        //判断是否勾选
        if(check->getSelectedState() == true)   //需要的
        {

        }
        else        //不需要的.
        {
            //加入Delete Vector
            vFrameName.at(i).bDeleted = true;
        }
    }

    //重做m_vFrameUsed
    part->makeNewPart();

    if(part->getMain())   //如果是主体, 需要修正所有关联的特效.
    {
        xCurAtk->repairAllEffectStartFrameIndex();
    }

    //重新选择
    xSkill->setCurAtkIndex(xSkill->getCurAtkIndex(), OT_SELECT);

    removeFromParent();
}


void FrameBrowser::removeVectorElementByIndex(vector<FramesName> vec, int index)
{
    vector<FramesName>::iterator it;

    for(it = vec.begin();it != vec.end(); ++it)
    {
        FramesName &frames = *it;
        if(frames.iNumber == vec.at(index).iNumber)
        {
            it = vec.erase(it);
            break;
        }
    }
}