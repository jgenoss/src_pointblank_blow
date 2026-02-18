#include "pch.h"
#include "UIGachaView.h"
#include "UIPhaseGachaShop.h"
#include "UIMainFrame.h"
#include "UIGachaObject.h"
#include "UIUtil.h"
#include "UISlide.h"
#include "UITopMenu_V15.h"

#include "ShopContext.h"
#include "ItemImageMgr.h"


I3_CLASS_INSTANCE( UIGachaView);

static i3Light * s_pLight = nullptr;

namespace
{
	UI::tmPopupSlideMtd g_slide_mtd;

	bool g_check_slide;
	REAL32 elapse_time;
}

static UIPhaseGachaShop* GetGachaShop( void)
{
	if ( UIMainFrame* pFrame = g_pFramework->GetUIMainframe() ) 
		return i3::same_cast<UIPhaseGachaShop*>(pFrame->GetCurrentSubPhase());
	return nullptr;
}

void SlideCallBack(void * _Frame, void * _3DView)
{
	i3UIFrameWnd * pFrame	= static_cast<i3UIFrameWnd*>(_Frame);
	i3UI3DViewBox * pBox	= static_cast<i3UI3DViewBox*>(_3DView);

	if(pFrame && pBox)
	{
		VEC2D * pSize = pFrame->getSize();

		pBox->setSize( pSize->x-5, pSize->y-5);
		pBox->setPos( 1, 1);

		pFrame->UpdateRenderObjects();
	}
}

void SlideEndCallBack()
{
	if( GetGachaShop() )
	{
		GetGachaShop()->getGachaView()->setAnimationState(GACHA_PLAY_IDLE);
		GetGachaShop()->getGachaView()->setBuyAnimationStart();
	}
}

UIGachaView::UIGachaView() : m_p3DView(nullptr), m_pGacha(nullptr), m_pLight(nullptr), m_rCharaChangePoseTime(0.f)
	, m_LightTheta(0.f), m_LightRho(0.f), m_LightDist(0.f), m_timeLocal(0.f), m_bStartMoveFrame(false)
	, m_pParentFrm(nullptr), weaponIn_time(0.f), weaponOut_time(0.f), m_bComboStart1(false)
	, m_rComboElapsedTime1(0.f), m_bComboStart2(false), m_rComboElapsedTime2(0.f), m_pSelectedItem(nullptr)
{

}

UIGachaView::~UIGachaView()
{
	_ReleaseLight();
	Delete3DViewBox();

	I3_SAFE_RELEASE(m_p3DView);
	I3_SAFE_RELEASE(m_pGacha);
}

void UIGachaView::_AddLight( void)
{
	VEC3D d_rot;
	COLORREAL d_diffuse, d_specular;

	if( s_pLight == nullptr)
	{
		s_pLight = i3Light::new_object();
		s_pLight->SetLightType( I3G_LIGHT_DIRECTIONAL);

		i3Color::Set( &d_diffuse, 1.2f, 1.2f, 1.2f, 1.0f);
		i3Color::Set( &d_specular, 1.0f, 1.0f, 1.0f, 1.0f);
		i3Vector::Set( &d_rot, -12.0f, -34.0f, 4.0f);

		s_pLight->SetDiffuse( &d_diffuse);
		s_pLight->SetSpecular( &d_specular);

		MATRIX mtx;

		i3Matrix::SetTranslate( &mtx, 0.0f, 0.0f, getZ( &d_rot));
		i3Matrix::PostRotateX( &mtx, I3_DEG2RAD( getY( &d_rot)));
		i3Matrix::PostRotateY( &mtx, I3_DEG2RAD( getX( &d_rot)));

		s_pLight->SetWorldSpaceTransform( &mtx);

		g_pFramework->addLight( s_pLight);
	}
	else
	{
		I3_MUST_ADDREF( s_pLight);
	}
	g_pFramework->setShadowQuality( 0);
}

void UIGachaView::_ReleaseLight( void)
{
	if( g_pFramework && s_pLight)
	{
		I3_MUST_RELEASE(s_pLight);

		if( s_pLight->GetRefCount() == 1)
		{
			g_pFramework->removeLight( s_pLight);
			s_pLight = nullptr;
		}

		// 그림자 설정을 이전으로 돌린다.
		g_pFramework->setShadowQuality( g_pEnvSet->m_nShadowQualityType, false);
	}
}

void UIGachaView::Create3DViewBox( i3UIControl * pParent, i3UIScene * pScene )
{
	I3ASSERT( pParent != nullptr);

	m_pParentFrm = (i3UIFrameWnd *)pParent;
	m_p3DView = i3UI3DViewBox::new_object();
	I3ASSERT( m_p3DView);

	I3ASSERT( pParent);
	pParent->AddChild( m_p3DView);

	m_p3DView->setSize( m_pParentFrm->getWidth()-9, m_pParentFrm->getHeight()-9);
	m_p3DView->setPos( 5, 5);

	UI::tmSlideAttContainer list(pScene);
	list.add("i3UIFrameWnd13", get3DViewBox(), SlideCallBack, SlideEndCallBack);

	VEC3D * pVec1 = pParent->getAccumulatedPos();
	VEC2D * pVec2 = pParent->getSize();

	REAL32 ratio = pVec2->y / pVec2->x;
	REAL32 targetWidth = i3UI::getManager()->getWidth() * 0.8f;
	REAL32 targetHeight= targetWidth * ratio;

	REAL32 xOffset = (REAL32)(i3UI::getManager()->getWidth() - targetWidth) * 0.5f;
	REAL32 yOffset = (REAL32)(i3UI::getManager()->getHeight() -targetHeight) * 0.5f;
	xOffset = xOffset - pVec1->x;
	yOffset = yOffset - pVec1->y;


	list.set_target_by_offset_and_size( (INT32)xOffset, (INT32)yOffset, targetWidth, targetHeight);

	m_Slide.add( list);

}

void UIGachaView::SetPos3DViewBox(void)
{
	VEC2D * pVec2 = m_pParentFrm->getSize();
	VEC3D * pVec3 = m_pParentFrm->getPos();
	REAL32 xOffset = (REAL32)(i3UI::getManager()->getWidth() - pVec2->x) * 0.5f;
	pVec3->x = xOffset; 
}

void UIGachaView::Delete3DViewBox( void)
{
	if( m_pGacha != nullptr)
	{
		m_pGacha->DeleteWeaponData();
		I3_SAFE_RELEASE(m_pGacha);
	}
	if( m_p3DView != nullptr)
	{
		i3GameNode * pParent = m_p3DView->getParent();
		pParent->RemoveChild( m_p3DView);
		I3_SAFE_RELEASE( m_p3DView);
	}
}

void UIGachaView::CreateViewGacha()
{
	VEC3D vPos;
	m_pGacha = GachaObject::new_object();
	
	m_pGacha->Create(g_pFramework,nullptr);
	m_pGacha->OnBindResource();

	i3AttrSet* pAttrSet = i3AttrSet::new_object_ref();

	pAttrSet->AddChild( m_pGacha->getSceneObject());

	{
		UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
		i3Texture* reflectTex = pFrame->GetTopMenu_V15()->GetUIReflectMap();
		
		if(reflectTex)
		{
			i3ReflectMapBindAttr* refMapBindAttr = i3ReflectMapBindAttr::new_object_ref();
			refMapBindAttr->SetTexture(reflectTex);
			pAttrSet->AddAttr(refMapBindAttr);
		}
	}

	I3_GAMENODE_ADDCHILD( g_pFramework->GetUIMainframe(), m_pGacha );

	m_p3DView->Create3DView();
	m_p3DView->Set3DView( pAttrSet);
	//_AddLight();

	m_p3DView->OnVisibleScene();
	m_p3DView->get3DCamera()->SetFOV(I3_DEG2RAD( 60 ));

	i3Vector::Set( &vPos, 0.f, 0.0f, -0.0f);
	m_pGacha->SetMatrix( &I3_IDENTITYMATRIX);
	m_pGacha->SetPos( &vPos);

	MATRIX * pObjectMtx = m_pGacha->GetMatrix();
	i3Matrix::PostRotateXDeg( pObjectMtx, -90.f);
	i3Matrix::PostRotateYDeg( pObjectMtx, 180.f);
	i3Matrix::PostRotateZDeg( pObjectMtx, 0.f);
	
	MATRIX * pCamMtx = m_p3DView->get3DCamera()->GetMatrix();
	i3Matrix::Identity( pCamMtx);
	i3Matrix::SetPos( pCamMtx, 0.0f, 0.05f, -0.6f, 1.0f);
}


void UIGachaView::OnUpdate( REAL32 rDeltaSeconds)
{
	if(g_pFramework->GetUIMainframe()->IsOpenedPopup(UPW_GACHA_WEAPON_DETAIL))
		return;

	if(m_pGacha)
	{
		VEC3D vPos;
		i3Vector::Set( &vPos, 0.f, 0.0f, -0.0f);
		m_pGacha->SetPos( &vPos);
		m_pGacha->OnUpdate(rDeltaSeconds);
	}

	if(m_bComboStart1 == true)
	{
		m_rComboElapsedTime1 += rDeltaSeconds;
		if(m_rComboElapsedTime1 > weaponIn_time)
		{
			setAnimationState(GACHA_PLAY_WEAPON_IN);
			setBuyAnimationStart();

			m_bComboStart1 = false;
			m_rComboElapsedTime1 = 0.0f;
			m_bComboStart2 = true;
			m_rComboElapsedTime2 = 0.0f;
			
			// 무기가 등록되고 무기 In 사운드가 출력 되어야 한다.
			g_pSndMng->StartLotterySnd(GSND_LOTTERY_WEAPONLOAD);

			for(INT32 k=0; k<RANDOM_ITEM_COUNT_BY_ITEM; k++)
			{
				T_ItemID curWeaponItemID = 0;
				GACHA_WEAPON_POS weaponPosIdx;

				if(k < GetRandomGoodsCntFromGachaItemInfo(m_pSelectedItem) )
					curWeaponItemID = CShopContext::i()->ConvertGoodsID(m_pSelectedItem->m_arRandomGoodsIDs[k]);

				if( k > 2)
					weaponPosIdx = static_cast<GACHA_WEAPON_POS>(k+1);
				else
					weaponPosIdx = static_cast<GACHA_WEAPON_POS>(k);

				AddGachaItem( weaponPosIdx, curWeaponItemID);
			}

			// 상단
			{
				T_ItemID curWeaponItemID = CShopContext::i()->ConvertGoodsID(m_pSelectedItem->m_ui32GoodsID);
				AddGachaItem( GACHA_POS_4, curWeaponItemID);
			}
		}
	}
	else if(m_bComboStart2 == true)
	{
		m_rComboElapsedTime2 += rDeltaSeconds;
		if(m_rComboElapsedTime2 > weaponOut_time)
		{
			OnIdleStart();
			//	m_pGacha->SetGachaAnimState(GACHA_PLAY_IDLE, 0.0f, I3_ANIM_PLAY_MODE_REPEAT, I3_ANIM_PLAY_MODE_CLAMP);
			m_bComboStart2 = false;
			m_rComboElapsedTime2 = 0.0f;
		}
	}
	
//	HDIObject2(rDeltaSeconds);
	i3GameNode::OnUpdate( rDeltaSeconds);
	
}

void UIGachaView::AddGachaItem(GACHA_WEAPON_POS _posIdx, T_ItemID _itemID)
{
	INT32 curItemType = 0;
	
	curItemType = GET_ITEM_TYPE(_itemID);

	// Unknown Item & Parts Item
	//m_pGacha->ClearData(_posIdx);
	m_pGacha->AddItemToPos(_posIdx, _itemID, m_p3DView);	
}

void UIGachaView::ProcessGameEvent( GAME_EVENT evt,INT32 arg, const i3::user_data& UserData )
{
}

void UIGachaView::OnIdleStart()
{
	if (m_pGacha == nullptr)
		return;

	m_pGacha->SetGachaAnimState(GACHA_PLAY_IDLE, 0.0f, I3_ANIM_PLAY_MODE_REPEAT, I3_ANIM_PLAY_MODE_CLAMP);

	i3Animation* pani = m_pGacha->getAni(GACHA_PLAY_WEAPON_IN);
	if(pani != nullptr)
	{
		weaponIn_time = m_pGacha->getAni(GACHA_PLAY_WEAPON_IN)->GetDuration();
		weaponOut_time= m_pGacha->getAni(GACHA_PLAY_WEAPON_OUT)->GetDuration();
	}

}

void UIGachaView::setItemInfo( const GACHA_ITEM_INFO * pInfo )
{
	m_pSelectedItem = (GACHA_ITEM_INFO *)pInfo;

	m_bComboStart1 = true;
	m_rComboElapsedTime1 = 0.0f;
	setAnimationState(GACHA_PLAY_WEAPON_OUT);
	setBuyAnimationStart();
}

void UIGachaView::setBuyAnimationStart()
{
	if (m_pGacha)
		m_pGacha->SetGachaAnimState(m_AniState, 0.0f, I3_ANIM_PLAY_MODE_CLAMP, I3_ANIM_PLAY_MODE_CLAMP);
}

void UIGachaView::BeginGachaPlayingAni()
{
	//g_pFramework->GetUIMainframe()->DisableFloatWin(UFW_CHAT);
	//g_pFramework->GetUIMainframe()->DisableFloatWin(UFW_USERLIST);

	g_slide_mtd.invert = true; 
	m_Slide.move_target_and_size();

	i3GameNode * pParent = m_p3DView->getParent();
	m_pParentFrm2->EnableCtrl(true);
	pParent->RemoveChild( m_p3DView);
	m_pParentFrm2->AddChild(m_p3DView);

	//UpdateParentSize();
	VEC2D* sizP2 = m_pParentFrm2->getSize();
	m_p3DView->setPos( (sizP2->x/2.0f) - ((m_pParentFrm->getWidth()-2)/2.0f)    , 0);
	m_p3DView->setSize(m_pParentFrm->getWidth()-2, m_pParentFrm->getHeight()-2);


	UIPhaseGachaShop::EGACHA_CLICK iGachaAmount = UIPhaseGachaShop::EGACHA_NON_CLICK;
		
	if( GetGachaShop() ) 
		iGachaAmount = GetGachaShop()->getGachaAmount();

	if(iGachaAmount == UIPhaseGachaShop::EGACHA_FIRST_CLICK)
		setAnimationState(GACHA_PLAY_SELECT_A);
	else if(iGachaAmount == UIPhaseGachaShop::EGACHA_SECOND_CLICK)
		setAnimationState(GACHA_PLAY_SELECT_B);
	else if(iGachaAmount == UIPhaseGachaShop::EGACHA_THIRD_CLICK)
		setAnimationState(GACHA_PLAY_SELECT_C);
	else
		setAnimationState(GACHA_PLAY_SELECT_A);						//일단 이렇게 처리합니다. 이게 걸리면 안되는 상황이고, 없을거라 생각되지만 걸린다면 다시 생각해보겠습니다. 2015-02-04 김대영

	setBuyAnimationStart();

	m_bObjectAniStart = true;

	i3Animation * pAni = getRouletteObject()->getCurrentAni();

	if( pAni != nullptr)
		m_rObjectDurationTime = pAni->GetDuration();

	m_rObjectElapseTime = 0.0f;

	//2014-08-07 캡슐 뱉는 애니메이션이 끊기는 현상으로 인해 캡슐 뽑기전 슬라이드 애니메이션을 초기화 합니다.
	//ENG_김대영

	m_bComboStart1 = false;
	m_rComboElapsedTime1 = 0.0f;
	m_bComboStart2 = false;
	m_rComboElapsedTime2 = 0.0f;
	i3Animation* pani = m_pGacha->getAni(GACHA_PLAY_WEAPON_IN);
	if(pani != nullptr)
	{
		weaponIn_time = m_pGacha->getAni(GACHA_PLAY_WEAPON_IN)->GetDuration();
		weaponOut_time= m_pGacha->getAni(GACHA_PLAY_WEAPON_OUT)->GetDuration();
	}
	SetPos3DViewBox();
}

bool UIGachaView::UpdateGachaPlayingAni(REAL32 rDeltaSeconds)
{
	if(m_bObjectAniStart == true)
	{
		m_rObjectElapseTime += rDeltaSeconds;

		if(m_rObjectElapseTime > m_rObjectDurationTime + 0.5f)
		{
			i3GameNode * pParent = m_p3DView->getParent();
			pParent->RemoveChild( m_p3DView);
			m_pParentFrm->AddChild(m_p3DView);
			m_pParentFrm2->EnableCtrl(false);

			g_slide_mtd.invert = false;
			m_Slide.move_origin_and_size();

			m_bObjectAniStart = false;
			m_rObjectElapseTime = 0.0f;

			// 로비가차애니메이션뷰로 바뀜
			m_p3DView->setPos(5,5);
			m_p3DView->setSize(m_pParentFrm->getWidth()-9, m_pParentFrm->getHeight()-9);

			OnIdleStart();

			return false;
		}

		return true;
	}

	return false;
}




