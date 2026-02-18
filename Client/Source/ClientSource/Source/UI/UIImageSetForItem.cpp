#include "pch.h"
#include "UIImageSetForItem.h"

#include "UIUtil.h"
#include "UIImageSetControl.h"

enum ITEM_TEMPLATE
{
	ITEM_TEMPLATE_A0 = 0,
	ITEM_TEMPLATE_A1,
	ITEM_TEMPLATE_B0,
	ITEM_TEMPLATE_B1,

	ITEM_TEMPLATE_MAX
};

I3_CLASS_INSTANCE( UIImageSetForItem);

UIImageSetForItem::UIImageSetForItem()
{
	m_pImageCtrl = nullptr;
	m_Type = UI_IMAGESET_BIND_WEAPON;
}

UIImageSetForItem::~UIImageSetForItem()
{
	I3_SAFE_RELEASE( m_pImageCtrl);
}

void UIImageSetForItem::Create( UI_IMAGESET_BIND_TYPE type /*= UI_IMAGESET_BIND_WEAPON*/)
{
	i3::rc_wstring wszTemp;
	i3::string szTemp;
	m_pImageCtrl = UIImageSetControl::new_object();
	I3ASSERT( m_pImageCtrl);

	m_Type = type;

	I3ASSERT( m_pImageCtrl != nullptr);

	switch( type)
	{
	case UI_IMAGESET_BIND_WEAPON :
		{
			for (size_t i=0; i<g_pWeaponInfoDataBase->GetUITemplateInfoCount(); i++)
			{
				const CUITemplateInfo_Weapon* info = g_pWeaponInfoDataBase->GetUITemplateInfo(i);
				I3ASSERT(info != nullptr);

				if (info)
				{
					i3::string strName;		i3::utf16_to_mb(info->GetName(), strName);
					m_pImageCtrl->AddTemplate(strName.c_str());
				}
			}
		}
		break;

	case UI_IMAGESET_BIND_CHARA :
		{
			// character 
			for (size_t i=0; i<g_pCharaInfoDataBase->GetUITemplateInfoCount_Chara(); i++)
			{
				const CUITemplateInfo_Chara* info = g_pCharaInfoDataBase->GetUITemplateInfo_Chara(i);
				I3ASSERT(info != nullptr);

				if (info)
				{
					//i3::string strName;		i3::utf16_to_mb(info->GetName(), strName);
					m_pImageCtrl->AddTemplate(info->GetName().c_str());
				}
			}

			// parts 
			for (size_t i=0; i<g_pCharaInfoDataBase->GetUITemplateInfoCount_Parts(); i++)
			{
				const CUITemplateInfo_Parts* info = 
					static_cast<const CUITemplateInfo_Parts*>(g_pCharaInfoDataBase->GetUITemplateInfo_Parts(i));
				I3ASSERT(info != nullptr);

				if (info)
				{
					i3::string strName;		i3::utf16_to_mb(info->GetName(), strName);
					m_pImageCtrl->AddTemplate(strName.c_str());
				}
			}
		}
		break;
	case UI_IMAGESET_BIND_ITEM :
		{
			// cash item (A / B)
			const INT32 EXPENDABLE = 0, MAINTENANCE = 1;
			m_CashACount = 0;

			for (size_t i=0; i<g_pShopItemInfoDataBase->GetUITemplateCount(); i++)
			{
				const CUITemplateInfo_Item* info = 
					static_cast<CUITemplateInfo_Item*>(g_pShopItemInfoDataBase->GetUITemplateInfo(i));
				I3ASSERT(info != nullptr);

				if (info && (info->GetType() == EXPENDABLE || info->GetType() == MAINTENANCE) )
				{
					i3::string strName;		i3::utf16_to_mb(info->GetName(), strName);
					m_pImageCtrl->AddTemplate(strName.c_str());

					if( info->GetType() == EXPENDABLE )
						m_CashACount += 1;
				}
			}
		}
		break;

	case UI_IMAGESET_BIND_NONWEAPON :
		{
			// character 
			for (size_t i=0; i<g_pCharaInfoDataBase->GetUITemplateInfoCount_Chara(); i++)
			{
				const CUITemplateInfo_Chara* info = g_pCharaInfoDataBase->GetUITemplateInfo_Chara(i);
				I3ASSERT(info != nullptr);

				if (info)
				{
					//i3::string strName;		i3::utf16_to_mb(info->GetName(), strName);
					m_pImageCtrl->AddTemplate(info->GetName().c_str());
				}
			}

			// parts 
			for (size_t i=0; i<g_pCharaInfoDataBase->GetUITemplateInfoCount_Parts(); i++)
			{
				const CUITemplateInfo_Parts* info = 
					static_cast<const CUITemplateInfo_Parts*>(g_pCharaInfoDataBase->GetUITemplateInfo_Parts(i));
				I3ASSERT(info != nullptr);

				if (info)
				{
					i3::string strName;		i3::utf16_to_mb(info->GetName(), strName);
					m_pImageCtrl->AddTemplate(strName.c_str());
				}
			}

			// cash item (A / B)
			const INT32 EXPENDABLE = 0, MAINTENANCE = 1;
			m_CashACount = 0;

			for (size_t i=0; i<g_pShopItemInfoDataBase->GetUITemplateCount(); i++)
			{
				const CUITemplateInfo_Item* info = 
					static_cast<CUITemplateInfo_Item*>(g_pShopItemInfoDataBase->GetUITemplateInfo(i));
				I3ASSERT(info != nullptr);

				if (info && (info->GetType() == EXPENDABLE || info->GetType() == MAINTENANCE) )
				{
					i3::string strName;		i3::utf16_to_mb(info->GetName(), strName);
					m_pImageCtrl->AddTemplate(strName.c_str());

					if( info->GetType() == EXPENDABLE )
						m_CashACount += 1;
				}
			}
		}
		break;
	default :
		break;
	}
}

void UIImageSetForItem::SetPos( REAL32 x, REAL32 y, REAL32 z )
{
	m_pImageCtrl->SetSize( x, y );
}

void UIImageSetForItem::SetSize( REAL32 cx, REAL32 cy)
{
	m_pImageCtrl->SetSize( cx, cy );
}

void UIImageSetForItem::SetSize( VEC2D * pVec)
{
	m_pImageCtrl->SetSize( pVec);
}

VEC2D * UIImageSetForItem::GetSize()
{
	return m_pImageCtrl->GetSize();
}

void UIImageSetForItem::BindScene( i3UIScene * pScene, const char * pszBindName)
{
	m_pImageCtrl->BindImageSet( pScene, pszBindName);
}

void UIImageSetForItem::BindScene( i3UIScene * pScene, const char * pszBindName, bool pResize)
{
	m_pImageCtrl->BindImageSet( pScene, pszBindName, pResize);
}

void UIImageSetForItem::SetItemImage( INT32 ItemID)
{
	if( ItemID > 0)
	{
		switch( m_Type)
		{
		case UI_IMAGESET_BIND_WEAPON :
			{
				INT32 imageIdx = GameUI::GetWeaponImageIndex( ItemID);
				imageIdx = 90 * (imageIdx/88) + (imageIdx % 88); // 기존 PB에 있던 완전 엉터리 같은 코드로 원본이미지가 88장이 된다면 이코드를 삭제할수 있다....(2011.09.01.수빈)
				if( imageIdx < 90) //100)	//m_pImageCtrl->GetTemplateShapeCount( 0))
					m_pImageCtrl->SetImage( 0, imageIdx);
				else if ( imageIdx < 180)
					m_pImageCtrl->SetImage( 1, imageIdx - 90); //- 100);//m_pImageCtrl->GetTemplateShapeCount( 0));
				else if ( imageIdx < 270)
					m_pImageCtrl->SetImage( 2, imageIdx - 180);
				else if ( imageIdx < 360)
					m_pImageCtrl->SetImage( 3, imageIdx - 270);
				else if ( imageIdx < 450)
					m_pImageCtrl->SetImage( 4, imageIdx - 360);
				else if ( imageIdx < 540)
					m_pImageCtrl->SetImage( 5, imageIdx - 450);
				else
					m_pImageCtrl->SetImage( 6, imageIdx - 540);
			}
			break;

		case UI_IMAGESET_BIND_CHARA :
			{
				INT32 id = g_pFramework->GetCharacterShape( ItemID);

				switch( CHARA::ItemID2Type(ItemID) )
				{
				case ITEM_TYPE_CHARA	:	m_pImageCtrl->SetImage( UISLOT_CHARA_ALL, id);		break;
				case ITEM_TYPE_DINO		:	m_pImageCtrl->SetImage( UISLOT_CHARA_ALL, id);		break;
				case ITEM_TYPE_HEADGEAR :	m_pImageCtrl->SetImage( UISLOT_CHARA_HEAD, id);		break;					
				case ITEM_TYPE_FACEGEAR :	m_pImageCtrl->SetImage( UISLOT_CHARA_MASK, id);		break;
				case ITEM_TYPE_BERET	:	m_pImageCtrl->SetImage( UISLOT_CHARA_BERET, id);	break;
				}
			}
			break;

		case UI_IMAGESET_BIND_ITEM :
			{
				INT32 tpl_idx = -1;
				INT32 img_idx = -1;

				ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo( ItemID);
				if( pInfo != nullptr)
				{
					switch( CashItem::GetUsageFromItemID(ItemID) )
					{
					case CASHITEM_USAGE_MAINTENANCE	:	tpl_idx = m_CashACount + pInfo->GetTexIndex();	break;	// CashB0
					case CASHITEM_USAGE_EXPENDABLES	:	tpl_idx = pInfo->GetTexIndex();					break;	// CashA0
					}

					img_idx = pInfo->GetShapeIndex();
				}

				m_pImageCtrl->SetImage( tpl_idx, img_idx);
			}
			break;

		case UI_IMAGESET_BIND_NONWEAPON :
			{
				INT32 tpl_idx = -1;
				INT32 img_idx = -1;
				
				switch( static_cast<ITEM_TYPE>(ItemID) )
				{
				case ITEM_TYPE_CHARA	: tpl_idx = UTT_NONWEAPON_CHARA;	img_idx = g_pFramework->GetCharacterShape( ItemID);	break;
				case ITEM_TYPE_DINO		: tpl_idx = UTT_NONWEAPON_CHARA;	img_idx = g_pFramework->GetCharacterShape( ItemID);	break;
				case ITEM_TYPE_HEADGEAR : tpl_idx = UTT_NONWEAPON_HEAD;		img_idx = g_pFramework->GetCharacterShape( ItemID);	break;
				case ITEM_TYPE_FACEGEAR : tpl_idx = UTT_NONWEAPON_MASK;		img_idx = g_pFramework->GetCharacterShape( ItemID);	break;
				case ITEM_TYPE_BERET	: tpl_idx = UTT_NONWEAPON_MASK;		img_idx = g_pFramework->GetCharacterShape( ItemID);	break;
				default:
					{
						ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo( ItemID);
						if( pInfo != nullptr)
						{
							switch( CashItem::GetUsageFromItemID(ItemID) )
							{
							case CASHITEM_USAGE_MAINTENANCE	:	tpl_idx = m_CashACount + pInfo->GetTexIndex();			break;	// CashB0
							case CASHITEM_USAGE_EXPENDABLES	:	tpl_idx = UTT_NONWEAPON_CASHA + pInfo->GetTexIndex();	break;	// CashA0
							}

							img_idx = pInfo->GetShapeIndex();
						}
					}
				}

				m_pImageCtrl->SetImage( tpl_idx, img_idx);
			}
			break;
		}
	}
	else
		m_pImageCtrl->SetImage( -1, -1);
}

void UIImageSetForItem::SetVisibleImage( bool bVisible)
{
	m_pImageCtrl->SetVisible( bVisible);
}
