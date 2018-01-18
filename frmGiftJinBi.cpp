#include "frmGiftJinBi.h"
#include "frmStencil.h"
#include "frmManager.h"

#include "DataManager.h"
#include "ZGame.h"
#include "frmDXCchapter.h"
#include "Weapon.h"
#include "WeaponSystem.h"
#include "GoodsBase.h"
#include "GoodsSystem.h"
#include "frmPromptTips.h"
#include "SMSInfo.h"
#include "GameSQL.h"
#include "ZTools.h"
#include "GameRecord.h"
NAMESPACE_ZJG_BEGIN

frmGiftJinBi::frmGiftJinBi()
{
	formType = kFORM_TYPE_D;
}

frmGiftJinBi::~frmGiftJinBi()
{

}

bool frmGiftJinBi::init()
{
	if (!frmBase::init())
	{
		return false;
	}
	auto frmNode = pStencil->getUI(kUI_Gift_JinBi);
	this->addChild(frmNode);
	INIT_FORM_ROOT(frmNode);

	pgameInfo->addSkeletonData("EQUIP_ICON", "spine/DROP_icon.json", "spine/DROP_icon.atlas", true);

	CONTROL_INIT_WITH_EVENT(BTN_BACK, Button*, PNL_ROOT, frmGiftJinBi);
	CONTROL_INIT_WITH_EVENT(BTN_FEE, Button*, PNL_ROOT, frmGiftJinBi);

	for (int i = 0; i < ARRAY_LENGTH(pnlItem); i++)
	{
		string name = "PNL_ITEM" + ConvertToString(i + 1);
		pnlItem[i] = dynamic_cast<Layout*>(Helper::seekWidgetByName(PNL_ROOT, name));
	}

	return true;
}

void frmGiftJinBi::initForm(cocos2d::Value* frmParam)
{
	pgameInfo->addATalkingData(141);

	int shopId = pDataManager->mChargePointInfList.at(SMS_BUY_JINBI150000)->shopLink;			//300钻石的礼包
	ShopInf* shopInf = pShopInfList.at(shopId);
	curInf = shopInf;

	//奖励物品显示
	for (int i = 0; i < ARRAY_LENGTH(pnlItem); i++)
	{
		ImageView* IMG_ICON = dynamic_cast<ImageView*>(Helper::seekWidgetByName(pnlItem[i], "IMG_ICON"));
		TextAtlas* ATL_NUMBER = dynamic_cast<TextAtlas*>(Helper::seekWidgetByName(pnlItem[i], "ATL_NUMBER"));
		ImageView* IMG_PART = dynamic_cast<ImageView*>(Helper::seekWidgetByName(pnlItem[i], "IMG_PART"));
		Layout *PNL_STAR = dynamic_cast<Layout*>(Helper::seekWidgetByName(pnlItem[i], "PNL_STAR"));
		auto IMG_BLACK = dynamic_cast<ImageView*>(Helper::seekWidgetByName(pnlItem[i], "IMG_BLACK"));
		Text* TXT_STAR_NUMBER = dynamic_cast<Text*>(Helper::seekWidgetByName(PNL_STAR, "TXT_STAR_NUMBER"));
		auto BTN_ITEM = dynamic_cast<Button*>(Helper::seekWidgetByName(pnlItem[i], "BTN_ITEM"));
		BTN_ITEM->addTouchEventListener(CC_CALLBACK_2(frmGiftJinBi::BTN_ITEM_Click, this));
		PNL_STAR->setVisible(false);

		pnlItem[i]->setVisible(true);

		int type = shopInf->items[0 * 6];
		int itemId = shopInf->items[0 * 6 + 1];
		int count = shopInf->items[0 * 6 + 2];
		if (type == 0)
		{
			Weapon* wp = pWeaponSystem->getAWeaponStencil(itemId);
			paintSpineWithActionWithItem(IMG_ICON, "EQUIP_ICON", 1000, wp);
			PNL_STAR->setVisible(true);
			wp->refreshStarUP(PNL_STAR);
			BTN_ITEM->setUserObject(wp);
			ATL_NUMBER->setVisible(true);
			IMG_BLACK->setVisible(false);
			IMG_PART->setVisible(false);
			TXT_STAR_NUMBER->setVisible(false);
		}
		else if (type == 1)
		{
			auto gb = pGoodsSystem->getAGoodsStencil(itemId);
			paintSpineWithActionWithItem(IMG_ICON, "EQUIP_ICON", 1000, gb);
			ATL_NUMBER->setString(ConvertToString(count));
			ATL_NUMBER->setVisible(true);
			BTN_ITEM->setUserObject(gb);
			IMG_BLACK->setVisible(false);
			IMG_PART->setVisible(false);
			if (gb->getType() == kGOODS_TYPE_WeaponPatch || gb->getType() == kGOODS_TYPE_MaterialPatch)
			{
				IMG_PART->setVisible(true);
			}
		}
		ATL_NUMBER->setString(ConvertToString(count));
	}

	ImageView* IMG_Normal = dynamic_cast<ImageView*>(Helper::seekWidgetByName(PNL_ROOT, "Image_1"));
	ImageView* IMG_Double = dynamic_cast<ImageView*>(Helper::seekWidgetByName(PNL_ROOT, "Image_Tagline"));
	int count = curInf->buyCount;
	if (curInf->buyCount > 0)
	{
		//如果购买过了，不翻倍
		pnlItem[1]->setVisible(false);
		pnlItem[0]->setVisible(true);
		IMG_Normal->setVisible(true);
		IMG_Double->setVisible(false);
	}
	else
	{
		//没有买过，首冲翻倍
		pnlItem[1]->setVisible(true);
		pnlItem[0]->setVisible(true);
		IMG_Normal->setVisible(false);
		IMG_Double->setVisible(true);
	}

	//设置购买/领取 按钮显示
	ImageView* buttonImageBuy = dynamic_cast<ImageView*>(Helper::seekWidgetByName(BTN_FEE, "IMAGE_BUY"));
	ImageView* buttonImageGet = dynamic_cast<ImageView*>(Helper::seekWidgetByName(BTN_FEE, "IMAGE_BUY_BLACK"));
	ImageView* IMAGE_COST = dynamic_cast<ImageView*>(Helper::seekWidgetByName(PNL_ROOT, "IMAGE_COST"));
	ImageView* IMAGE_COST_BLACK = dynamic_cast<ImageView*>(Helper::seekWidgetByName(PNL_ROOT, "IMAGE_COST_BLACK"));
	if (!IsBlackPackage)
	{
		buttonImageBuy->setVisible(true);
		buttonImageGet->setVisible(false);
		IMAGE_COST->setVisible(true);
		IMAGE_COST_BLACK->setVisible(false);
	}
	else
	{
		buttonImageBuy->setVisible(false);
		buttonImageGet->setVisible(true);
		IMAGE_COST->setVisible(false);
		IMAGE_COST_BLACK->setVisible(true);
	}

	EnableControlButton(BTN_TOP);
	auto frmNode = pStencil->getTimeline(kUI_Gift_XinShou);
	this->runAction(frmNode);
	frmNode->play("EnterAnimation", false);
	frmNode->setLastFrameCallFunc([=](){
		DisabledControlButton(BTN_TOP);
	});
}

void frmGiftJinBi::refreshForm()
{

}


void frmGiftJinBi::exitForm()
{
	for (int i = 0; i < ARRAY_LENGTH(pnlItem); i++)
	{
		ImageView* IMG_ICON = dynamic_cast<ImageView*>(Helper::seekWidgetByName(pnlItem[i], "IMG_ICON"));
		IMG_ICON->removeChildByTag(1000);
	}
}

void frmGiftJinBi::paint(float delta)
{

}

void frmGiftJinBi::BTN_BACK_Click(Ref* pSender, ui::Widget::TouchEventType type)
{
	Button *btn = (Button*)pSender;
	if (type == Widget::TouchEventType::BEGAN)
	{
		playEffectMusic(btn);
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		auto frmNode = pStencil->getTimeline(kUI_Gift_XinShou);
		this->runAction(frmNode);
		frmNode->play("ExitAnimation", false);
		frmNode->setLastFrameCallFunc([=](){
			pFRMManager->ClosedDialog(this);
			if (pFRMManager->getForm(kFORM_SKILL_NEW))
			{
				pFRMManager->getForm(kFORM_SKILL_NEW)->refreshForm();
			}
		});
	}
}

void frmGiftJinBi::BTN_FEE_Click(Ref* pSender, ui::Widget::TouchEventType type)
{
	Button *btn = (Button*)pSender;
	if (type == Widget::TouchEventType::BEGAN)
	{
		playEffectMusic(btn);
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		//登陆奖励,礼包Gaoang
		pSMSInfo->sendMessage(curInf->chargePointId, CC_CALLBACK_2(frmGiftJinBi::buyJinBi150000SMS, this));		//SMS_BUY_XINSHOU
	}
}

void frmGiftJinBi::buyJinBi150000SMS(int payId, bool flag)
{
	//购买失败，直接返回
	if (!flag)
	{
		return;
	}
	// 提示购买成功("恭喜购买成功")
	auto frmNode = pStencil->getTimeline(kUI_Gift_XinShou);
	this->runAction(frmNode);
	frmNode->play("ExitAnimation", false);
	frmNode->setLastFrameCallFunc([=](){
		pFRMManager->ClosedDialog(this);
	});
	//发放物品奖励
	pgameInfo->buyItems(curInf);
	pgameInfo->saveRecord();
}

void frmGiftJinBi::BTN_ITEM_Click(Ref* pSender, ui::Widget::TouchEventType type)
{
	Button *btn = (Button*)pSender;
	if (type == Widget::TouchEventType::BEGAN)
	{
		playEffectMusic(btn);
		pFRMManager->ClosedDialog(kFORM_GOODSUSE);
		pFRMManager->ClosedDialog(kFORM_EQUIPPROPERTY_COMPARE);

		int showType = 0;
		int itemId = 0;
		auto wp = dynamic_cast<Weapon*>(btn->getUserObject());
		if (wp)
		{
			showType = 0;
			itemId = wp->getClassID();
		}

		auto gb = dynamic_cast<GoodsBase*>(btn->getUserObject());
		if (gb)
		{
			showType = 1;
			itemId = gb->getClassID();

			auto goods = pGoodsSystem->getAGoodsStencil(itemId);
			if (goods->getType() == kGOODS_TYPE_WeaponPatch)
			{
				showType = 0;
				itemId = goods->mLinkItemList[1];
			}
			else if (goods->getType() == kGOODS_TYPE_PetPatch)
			{
				showType = 2;
				itemId = goods->mLinkItemList[1];
			}
		}
		if (gb)
		{
			showType = 1;
			itemId = gb->getClassID();
		}


		Vec2 pos = Vec2(90, 0);
		if (showType == 1)
		{
			auto goods = pGoodsSystem->getAGoodsStencil(itemId);

			if (goods->getType() == kGOODS_TYPE_WeaponPatch)
			{
			}
			else if (goods->getType() == kGOODS_TYPE_PetPatch)
			{
				pos = Vec2(0, 0);
			}
			else
			{
				pos = btn->convertToWorldSpace(Vec2::ZERO);
				pos += Vec2(-150, -60);
			}
		}

		Node * frm = pgameInfo->showItemInf(showType, itemId);

		if (frm)
		{
			frm->setPosition(pos);
		}
	}
}

NAMESPACE_ZJG_END