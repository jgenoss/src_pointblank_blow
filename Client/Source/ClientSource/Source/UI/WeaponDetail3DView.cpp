#include "pch.h"

#include "WeaponDetail3DView.h"

#include "UIMainFrame.h"
#include "UIShopFunction.h"
#include "UIClanFunction.h"
#include "ItemImageMgr.h"
#include "BuyUtil.h"
#include "UIUtil.h"
#include "GameGUI.h"
#include "PurchaseMgr.h"
#include "UIFloatCharacterView.h"
#include "InvenUtil.h"
#include "UICharaInfoMgr.h"
#include "UserInfoContext.h"
#include "UISideMenu.h"
#include "UISMenu_Ability.h"
#include "UISMenu_Equip.h"
#include "MainWeapon_Dual.h"
#include "MainWeapon_Knuckle.h"
#include "MainWeapon_DualGrenade.h"
#include "MainWeapon_C5.h"
#include "MainWeapon_DualCIC.h"
#include "MainWeapon_DualUzi.h"
#include "i3Base/string/ext/sprintf.h"
#include "i3Base/string/ext/erase_string.h"
#include "i3Base/string/ext/insert_string.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "../../../../Common/CommonSource/CommonDef.h"

#include "WeaponFunction.h"
#include "i3Math/structure/aabb.h"
#include "i3Math/structure/vec3.h"

#pragma push_macro("min")
#undef min

#pragma push_macro("max")
#undef max

namespace
{
	bool aabb_aabbCollisontest(const i3::aabb* left, const i3::aabb* right)
	{
		if (left->max().x < right->min().x || left->min().x > right->max().x)	return false;
		if (left->max().y < right->min().y || left->min().y > right->max().y) 	return false;
		if (left->max().z < right->min().z || left->min().z > right->max().z)	return false;

		return true;
	}
}

/****************************************************************************/
/*								Detail 3D View								*/
/****************************************************************************/


WeaponDetail3DView::WeaponDetail3DView() : view_frame_wnd(nullptr), m_p3DViewNode(nullptr), m_p3DView(nullptr), m_pCamera(nullptr), m_BaseCamDist(0), m_ZoomMinDist(0), m_ZoomMaxDist(0), m_fDummyDist(0.0f), m_Dist(0.0f), weapon_obj(0), left_weapon_obj(0),
m_pDummyNode(nullptr), m_pSpriteBG(nullptr), m_pRefAttr(nullptr), LeftMBTNDown(false), m_change_dist(0.1f)
{
	m_rcDummyName = "";
	ZeroMemory(&m_vecDummyPos, sizeof(m_vecDummyPos));
	ZeroMemory(&m_ObjCenterPos, sizeof(m_ObjCenterPos));
	ZeroMemory(&m_BasePos, sizeof(m_BasePos));
	ZeroMemory(&m_DragStartMatrix, sizeof(m_DragStartMatrix));
	ZeroMemory(&m_DragStartPt, sizeof(m_DragStartPt));
};
WeaponDetail3DView::~WeaponDetail3DView()
{
	Weapon3DViewExit();
};

void WeaponDetail3DView::Weapon3DViewCreate(T_ItemID item_id)
{
	weapon_obj = g_pWeaponManager->_CreateNewWeapon(item_id, 0, nullptr, false);

	if (weapon_obj->GetAIContext())
	{
		weapon_obj->GetAIContext()->RemoveAIStateAll();
	}

	MainWeapon* mainWeapon = weapon_obj->GetMainWeapon();

	m_rcDummyName = "";
	ZeroMemory(&m_vecDummyPos, sizeof(m_vecDummyPos));
	m_fDummyDist = 0.0f;

	{
		if (i3::kind_of<WeaponDualHandGun*>(weapon_obj))
		{
			left_weapon_obj = static_cast<MainWeapon_Dual*>(mainWeapon)->getLeftWeapon();
			I3ASSERT(left_weapon_obj != nullptr);
			i3::sprintf(m_rcDummyName, "Weapon/Shop3DView/Dual_Handgun_Display.i3s");
			if (IsWeaponKeyWord("Handgun", weapon_obj->getWeaponInfo()))
			{
				m_fDummyDist = -0.3f;
			}
		}
		else if (i3::kind_of<WeaponDual*>(weapon_obj))
		{
			left_weapon_obj = static_cast<MainWeapon_Dual*>(mainWeapon)->getLeftWeapon();
			I3ASSERT(left_weapon_obj != nullptr);
			i3::sprintf(m_rcDummyName, "Weapon/Shop3DView/Dual_Gun_Display.i3s");
		}
		else if (i3::kind_of<WeaponKnuckle*>(weapon_obj))
		{
			left_weapon_obj = static_cast<MainWeapon_Knuckle*>(mainWeapon)->getLeftWeapon();
			I3ASSERT(left_weapon_obj != nullptr);
			i3::sprintf(m_rcDummyName, "Weapon/Shop3DView/Dual_Knuckle_Display.i3s");
			m_fDummyDist = 0.5f;
			if (IsWeaponKeyWord("PumpkinKnuckle") || IsWeaponKeyWord("Garenaknuckle"))
			{
				i3::sprintf(m_rcDummyName, "Weapon/Shop3DView/Dual_Knuckle_Event_Display.i3s");
			}
			else if (IsWeaponKeyWord("Kunai_Dual"))
			{
				i3::sprintf(m_rcDummyName, "Weapon/Shop3DView/Dual_Melee_Kunai_Display.i3s");
			}
		}
		else if (i3::kind_of<WeaponDualKnife*>(weapon_obj))
		{
			left_weapon_obj = static_cast<MainWeapon_Dual*>(mainWeapon)->getLeftWeapon();
			I3ASSERT(left_weapon_obj != nullptr);
			i3::sprintf(m_rcDummyName, "Weapon/Shop3DView/Dual_Melee_Display.i3s");
		}
		else if (i3::kind_of<WeaponDualGrenade*>(weapon_obj))
		{
			left_weapon_obj = static_cast<MainWeapon_DualGrenade*>(mainWeapon)->GetLeftWeapon();
			I3ASSERT(left_weapon_obj != nullptr);
			i3::sprintf(m_rcDummyName, "Weapon/Shop3DView/Dual_Throw_Display.i3s");
			m_fDummyDist = 0.5f;
			if (IsWeaponKeyWord("Chocolate"))
			{
				i3::sprintf(m_rcDummyName, "Weapon/Shop3DView/Dual_Throw_Event_Display.i3s");
			}
			else if (IsWeaponKeyWord("Shuttlecock"))
			{
				i3::sprintf(m_rcDummyName, "Weapon/Shop3DView/Dual_Throw2_Display.i3s");
			}
		}
		else if (i3::kind_of<WeaponDualCIC*>(weapon_obj))
		{
			left_weapon_obj = static_cast<MainWeapon_DualCIC*>(mainWeapon)->getLeftWeapon();
			I3ASSERT(left_weapon_obj != nullptr);
			i3::sprintf(m_rcDummyName, "Weapon/Shop3DView/Dual_Gun&Knife_Display.i3s");
			if (IsWeaponKeyWord("ColtPython_Black-Pearl"))
			{
				i3::sprintf(m_rcDummyName, "Weapon/Shop3DView/Dual_Gun&Knife_Display_2.i3s");
			}
		}
		else if (i3::kind_of<WeaponDualUzi*>(weapon_obj))
		{
			left_weapon_obj = static_cast<MainWeapon_DualUzi*>(mainWeapon)->getLeftWeapon();
			I3ASSERT(left_weapon_obj != nullptr);
			i3::sprintf(m_rcDummyName, "Weapon/Shop3DView/Dual_Gun_Display.i3s");
			if (IsWeaponKeyWord("Silencer", left_weapon_obj->getWeaponInfo()))
			{
				m_vecDummyPos.x = 0.09f;
			}
		}
		else if (i3::kind_of<WeaponDualSMG*>(weapon_obj))
		{
			left_weapon_obj = static_cast<MainWeapon_DualSMG*>(mainWeapon)->getLeftWeapon();

			if (IsWeaponKeyWord("Scorpion_Vz61"))
			{
				i3::sprintf(m_rcDummyName, "Weapon/Shop3DView/Dual_Gun_Display.i3s");
			}
		}
		else if (i3::kind_of<WeaponC5*>(weapon_obj) && !IsWeaponKeyWord("M14_Mine"))
		{
			//이전객체 지우고
			weapon_obj->setPairWeapon(nullptr);
			g_pWeaponManager->ReleaseCharaWeapon(weapon_obj);
			weapon_obj = nullptr;

			T_ItemID Itemid = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_THROWING1, WEAPON_CLASS_BOMBTRIGGER, WEAPON::getItemIndex(WEAPON::BOMBTRIGGER_DEFAULT)); // 412001
			weapon_obj = g_pWeaponManager->_CreateNewWeapon(Itemid, item_id, nullptr, false);
			if (weapon_obj->GetAIContext())
				weapon_obj->GetAIContext()->RemoveAIStateAll();

			left_weapon_obj = static_cast<WeaponBombTrigger*>(weapon_obj)->getBomb();
			if (left_weapon_obj->GetAIContext())
				left_weapon_obj->GetAIContext()->RemoveAIStateAll();
			left_weapon_obj->SetVisible(true);

			i3::sprintf(m_rcDummyName, "Weapon/Shop3DView/Dual_C5_Display.i3s");
			m_fDummyDist = 0.5f;
		}
	}

	//해당 사항을 pef에서 핸들링 할수있게 해놓았으나 GD팀에서 많은 무기류를 수정하는걸 꺼려한다
	//그래서 걍 하드코딩
	if (IsWeaponKeyWord("Spectre") || IsWeaponKeyWord("ChineseCleaver") || IsWeaponKeyWord("Spetnatz_Field_Shovel"))
	{
		m_fDummyDist = 0.4f;
	}
	if (IsWeaponKeyWord("MiniAxe"))
	{
		m_fDummyDist = 0.6f;
	}
	if (IsWeaponKeyWord("L115A1") || IsWeaponKeyWord("RangeMaster"))
	{
		m_vecDummyPos.x = -0.05f;
	}
	if (IsWeaponKeyWord("GH5007"))
	{
		m_vecDummyPos.x = 0.02f;
	}
	if (IsWeaponKeyWord("Snow-ball_Blaster"))
	{
		m_fDummyDist = 0.5f;
	}
	if (IsWeaponKeyWord("MP5K") || IsWeaponKeyWord("SSG69"))
	{
		m_fDummyDist = 0.1f;
	}
	if (IsWeaponKeyWord("P90") || IsWeaponKeyWord("Taurus_454SS") || IsWeaponKeyWord("SuperShorty") ||
		IsWeaponKeyWord("M-7") || IsWeaponKeyWord("Colt45") || IsWeaponKeyWord("DesertEagle"))
	{
		m_fDummyDist = 0.2f;
	}

	if (IsWeaponKeyWord("Taurus_454SS_2M"))
	{
		m_fDummyDist = 0.4f;
	}

	i3Vector::Set(&m_ObjCenterPos, 0.f, 0.0f, 0.0f);

	Weapon3DViewEntrance();
}
void WeaponDetail3DView::Weapon3DViewEntrance()
{
	i3Vector::Set(&m_BasePos, 0.0f, 0.0f, 0.0f);
	m_DragStartPt.x = m_DragStartPt.y = 0;

	// 일반 무기와 Dual 무기는 따로 처리해야함
	if (left_weapon_obj == 0)
		InitSingleWeaponPos();
	else
		InitDualWeaponPos();

	if (m_p3DView == nullptr) return;

	m_pCamera = m_p3DView->get3DCamera();
	m_pCamera->SetNearFar(0.01f, m_pCamera->GetFar());
	m_pCamera->SetFOV(I3_DEG2RAD(41.5f));

	ResetObjectPosition();
	ResetCameraPosition();
}

void WeaponDetail3DView::ReleaseWeaponObj()
{
	if (!i3::kind_of<WeaponBombTrigger*>(left_weapon_obj))
	{
		left_weapon_obj = nullptr;
		if (weapon_obj)
		{
			weapon_obj->setPairWeapon(nullptr);
			g_pWeaponManager->ReleaseCharaWeapon(weapon_obj);
			weapon_obj = nullptr;
		}
	}
	else
	{
		if (left_weapon_obj)
		{
			left_weapon_obj->setPairWeapon(nullptr);
			g_pWeaponManager->ReleaseCharaWeapon(left_weapon_obj);
			left_weapon_obj = nullptr;
		}
	}
}
void WeaponDetail3DView::Weapon3DViewExit()
{
	if (m_p3DView)
	{
		m_p3DView->getParent()->RemoveChild(m_p3DView);

		I3_SAFE_NODE_RELEASE(m_pDummyNode);
		I3_SAFE_RELEASE(m_p3DView);
		I3_SAFE_RELEASE(m_pRefAttr);
		I3_SAFE_RELEASE(m_p3DViewNode);
		I3_SAFE_RELEASE(m_pSpriteBG);
	}
}
void WeaponDetail3DView::Weapon3DViewUpdate(bool _LeftMBTNDown, bool _MouseIn3DView)
{
	if (_LeftMBTNDown == false && _MouseIn3DView == false) return;

	POINT mouse_pt = GameUI::GetScreenMousePosition();

	if (GameUI::IsMouseStroked(I3I_MOUSE_WHEELUP) == true)
		mouse_wheel(I3I_MOUSE_WHEELUP, mouse_pt);
	if (GameUI::IsMouseStroked(I3I_MOUSE_WHEELDOWN) == true)
		mouse_wheel(I3I_MOUSE_WHEELDOWN, mouse_pt);

	if (LeftMBTNDown == false)
	{
		if (GameUI::IsMousePressed(I3I_MOUSE_LBUTTON) == true)
		{
			mouse_btn_down(I3I_MOUSE_LBUTTON, mouse_pt); LeftMBTNDown = true;
		}
		else if (GameUI::IsMousePressed(I3I_MOUSE_RBUTTON) == true)
		{
			mouse_btn_down(I3I_MOUSE_RBUTTON, mouse_pt);
		}
	}
	else
	{
		if (GameUI::IsMousePressedUp(I3I_MOUSE_LBUTTON) == true)
		{
			mouse_btn_up(I3I_MOUSE_LBUTTON, mouse_pt); LeftMBTNDown = false;
		}
		else if (GameUI::IsMousePressedUp(I3I_MOUSE_RBUTTON) == true)
		{
			mouse_btn_up(I3I_MOUSE_RBUTTON, mouse_pt);
		}
	}

	mouse_move(mouse_pt, LeftMBTNDown);
}
void WeaponDetail3DView::Weapon3DViewOnRevie()
{
	if (m_p3DView != nullptr)
	{
		m_pCamera = m_p3DView->get3DCamera();
		m_pCamera->SetFOV(I3_DEG2RAD(41.5f));
	}

	Weapon3DViewCreateEffect();
};
void WeaponDetail3DView::Weapon3DViewCreateEffect()
{
	//현재는 안쓰는 부분이라 내용을 삭제 하였음.
	//추후에 사용할지도 몰라 일단 함수 껍데기만 남겨놈.
}
void WeaponDetail3DView::InitSingleWeaponPos()
{
	// 3D Viewer에 넣기 전에 무기의 회전, 위치 정보를 셋팅 하는 부분
	i3SceneObject* weapon_obj_scene = weapon_obj->getSceneObject();
	i3BoundBox* obj_bound_box = weapon_obj_scene->GetBody()->GetBound();

	if (obj_bound_box == nullptr) return;

	i3::string str_tmp;
	if (weapon_obj_scene->GetBody())
	{
		weapon_obj_scene->GetBody()->StopAnim();

		//탄피 미출력
		str_tmp = i3::format_string("Vol_Magazine_Dummy");
		i3Node* pNode = i3Scene::FindNodeByName(weapon_obj_scene->GetBody(), str_tmp.c_str());
		if (pNode)
		{
			pNode->SetFlag(I3_NODEFLAG_DISABLE);
		}

		//탄피 미출력
		for (int i = 0; i < BULLETCOUNT; i++)
		{
			if (i < 1)	str_tmp = i3::format_string("VolAnimationDummy");
			else str_tmp = i3::format_string("VolAnimationDummy_0%d", i);

			pNode = i3Scene::FindNodeByName(weapon_obj_scene->GetBody(), str_tmp.c_str());
			if (pNode)
			{
				pNode->SetFlag(I3_NODEFLAG_DISABLE);
			}
		}

		//소음기
		if (!weapon_obj->getWeaponInfo()->IsShop3DViewSilencer())
		{
			str_tmp = i3::format_string("Silencer_Material");
			pNode = i3Scene::FindNodeByName(weapon_obj_scene->GetBody(), str_tmp.c_str());
			if (pNode)
			{
				pNode->SetFlag(0x00000000);
			}
			str_tmp = i3::format_string("SilencerAniDummy");
			pNode = i3Scene::FindNodeByName(weapon_obj_scene->GetBody(), str_tmp.c_str());
			if (pNode)
			{
				pNode->SetFlag(0x00000000);
			}
		}
		else
		{
			str_tmp = i3::format_string("Silencer_Material");
			pNode = i3Scene::FindNodeByName(weapon_obj_scene->GetBody(), str_tmp.c_str());
			if (pNode)
			{
				pNode->SetFlag(I3_NODEFLAG_DISABLE);
			}
			str_tmp = i3::format_string("SilencerAniDummy");
			pNode = i3Scene::FindNodeByName(weapon_obj_scene->GetBody(), str_tmp.c_str());
			if (pNode)
			{
				pNode->SetFlag(I3_NODEFLAG_DISABLE);
			}
		}

		//착검
		str_tmp = i3::format_string("M_7_Material");
		pNode = i3Scene::FindNodeByName(weapon_obj_scene->GetBody(), str_tmp.c_str());
		if (pNode)
		{
			pNode->SetFlag(0x00000000);
		}

		str_tmp = i3::format_string("FXLaserSightDummy");
		pNode = i3Scene::FindNodeByName(weapon_obj_scene->GetBody(), str_tmp.c_str());
		if (pNode)
		{
			pNode->SetFlag(I3_NODEFLAG_DISABLE);
		}
	}


	VEC3D Min;
	VEC3D Max;
	VEC3D vec;

	Min = *obj_bound_box->GetMin();
	Max = *obj_bound_box->GetMax();

	obj_bound_box->GetCenter(&vec);


	//boundary box 원점으로 무기 중심점을 구한다.
	m_ObjCenterPos.x = vec.y; //정확하게는 모르겠지만, i3 engine은 local 회전이 아닌가벼?????
	m_ObjCenterPos.y = -vec.z;
	m_fDummyDist -= vec.x;

	i3Vector::Sub(&vec, &Min, &Max);
	// 차후 배율은 pef로 빼서 따로 저장할 수 있도록 수정합니다.

	CWeaponInfo* pinfo = weapon_obj->getWeaponInfo();

	m_Dist = pinfo->GetShop3DViewDist();

	if (pinfo->GetTypeClass() == WEAPON_CLASS_THROWING_GRENADE ||
		pinfo->GetTypeClass() == WEAPON_CLASS_KNUCKLE ||
		pinfo->GetTypeClass() == WEAPON_CLASS_THROWING_HEAL ||
		pinfo->GetTypeClass() == WEAPON_CLASS_THROWING_CONTAIN ||
		pinfo->GetTypeClass() == WEAPON_CLASS_THROWING_ATTACK)
	{
		//m_BaseCamDist = i3Math::Mul(i3Vector::Length(&vec), __RT(1.45f + m_fDummyDist + dist));
		//테스트
		m_BaseCamDist = i3Math::Mul(i3Vector::Length(&vec), 1.0f + m_fDummyDist + m_Dist);
	}
	else if (pinfo->GetTypeClass() == WEAPON_CLASS_HANDGUN)
	{
		m_BaseCamDist = i3Math::Mul(i3Vector::Length(&vec), 1.0f + m_fDummyDist + m_Dist);
	}
	else
	{
		m_BaseCamDist = i3Math::Mul(i3Vector::Length(&vec), 1.0f + m_fDummyDist + m_Dist);
	}


	/*I3TRACE("BoundBox X Length : %f\n", abs(obj_bound_box->GetMax()->x - obj_bound_box->GetMin()->x));
	I3TRACE("BoundBox Y Length : %f\n", abs(obj_bound_box->GetMax()->y - obj_bound_box->GetMin()->y));
	I3TRACE("BoundBox Z Length : %f\n", abs(obj_bound_box->GetMax()->z - obj_bound_box->GetMin()->z));*/

	//zoom 제한 길이 설정
	m_ZoomMinDist = m_BaseCamDist * 0.1f;
	m_ZoomMaxDist = m_BaseCamDist * 10.1f;

	weapon_obj_scene->AddFlag(I3_NODEFLAG_DISABLE_SHADOW_RECEIVER);

	Init3DViewBox(weapon_obj_scene);
}
void WeaponDetail3DView::InitDualWeaponPos()
{
	i3SceneObject* weapon_obj_scene = weapon_obj->getSceneObject();
	i3SceneObject* leftweapon_obj_scene = left_weapon_obj->getSceneObject();
	i3BoundBox* obj_bound_box = weapon_obj_scene->GetBody()->GetBound();

	i3::string str_tmp;

	if (weapon_obj_scene->GetBody())
	{
		weapon_obj_scene->GetBody()->StopAnim();
		i3Node* pNode = nullptr;
		//소음기
		if (!weapon_obj->getWeaponInfo()->IsShop3DViewSilencer())
		{
			str_tmp = i3::format_string("Silencer_Material");
			pNode = i3Scene::FindNodeByName(weapon_obj_scene->GetBody(), str_tmp.c_str());
			if (pNode)
			{
				pNode->SetFlag(0x00000000);
			}

			str_tmp = i3::format_string("SilencerAniDummy");
			pNode = i3Scene::FindNodeByName(weapon_obj_scene->GetBody(), str_tmp.c_str());
			if (pNode)
			{
				pNode->SetFlag(0x00000000);
			}
		}
		else
		{
			str_tmp = i3::format_string("Silencer_Material");
			pNode = i3Scene::FindNodeByName(weapon_obj_scene->GetBody(), str_tmp.c_str());
			if (pNode)
			{
				pNode->SetFlag(I3_NODEFLAG_DISABLE);
			}

			str_tmp = i3::format_string("SilencerAniDummy");
			pNode = i3Scene::FindNodeByName(weapon_obj_scene->GetBody(), str_tmp.c_str());
			if (pNode)
			{
				pNode->SetFlag(I3_NODEFLAG_DISABLE);
			}
		}
	}
	if (leftweapon_obj_scene->GetBody())
	{
		leftweapon_obj_scene->GetBody()->StopAnim();
		i3Node* pNode = nullptr;
		//소음기
		if (!left_weapon_obj->getWeaponInfo()->IsShop3DViewSilencer())
		{
			str_tmp = i3::format_string("Silencer_Material");
			pNode = i3Scene::FindNodeByName(leftweapon_obj_scene->GetBody(), str_tmp.c_str());
			if (pNode)
			{
				pNode->SetFlag(0x00000000);
			}
			str_tmp = i3::format_string("SilencerAniDummy");
			pNode = i3Scene::FindNodeByName(weapon_obj_scene->GetBody(), str_tmp.c_str());
			if (pNode)
			{
				pNode->SetFlag(0x00000000);
			}
		}
		else
		{
			str_tmp = i3::format_string("Silencer_Material");
			pNode = i3Scene::FindNodeByName(leftweapon_obj_scene->GetBody(), str_tmp.c_str());
			if (pNode)
			{
				pNode->SetFlag(I3_NODEFLAG_DISABLE);
			}
			str_tmp = i3::format_string("SilencerAniDummy");
			pNode = i3Scene::FindNodeByName(weapon_obj_scene->GetBody(), str_tmp.c_str());
			if (pNode)
			{
				pNode->SetFlag(I3_NODEFLAG_DISABLE);
			}
		}
	}

	VEC3D Min;
	VEC3D Max;

	Min = *obj_bound_box->GetMin();
	Max = *obj_bound_box->GetMax();

	VEC3D vec;
	
	i3Vector::Sub(&vec, &Max, &Min);

	m_Dist = weapon_obj->getWeaponInfo()->GetShop3DViewDist();

	m_BaseCamDist = i3Math::Mul(i3Vector::Length(&vec), 1.0f + m_fDummyDist + m_Dist);

	//zoom 제한 길이 설정
	m_ZoomMinDist = m_BaseCamDist * 0.1f;
	m_ZoomMaxDist = m_BaseCamDist * 10.1f;

	Init3DViewBox(nullptr);
}
void WeaponDetail3DView::Init3DViewBox(i3SceneObject * pSceneObj)
{
	I3_SAFE_RELEASE(m_p3DViewNode);
	m_p3DViewNode = i3Node::new_object();

	if (left_weapon_obj)
	{
		i3SceneFile file;

		if (file.Load(m_rcDummyName.c_str()) == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_WARN, "Couldn't find %s. Load Default Setting", "Gunshowcaseroom_Lighting.i3s");
			return;
		}

		m_pDummyNode = file.GetSceneGraph();
		I3_MUST_ADDREF(m_pDummyNode);


		i3::string str_tmp = i3::format_string("WeaponPointDummyLeft");
		i3Node* pNode = i3Scene::FindNodeByName(m_pDummyNode, str_tmp.c_str());
		if (pNode)
		{
			pNode->AddChild(left_weapon_obj->getSceneObject());
		}

		str_tmp = i3::format_string("WeaponPointDummyRight");
		pNode = i3Scene::FindNodeByName(m_pDummyNode, str_tmp.c_str());
		if (pNode)
		{
			pNode->AddChild(weapon_obj->getSceneObject());
		}

		m_p3DViewNode->AddChild(m_pDummyNode);
	}
	else
	{
		// attach weapon scene
		{
			m_p3DViewNode->AddChild(pSceneObj);
		}
	}

	//이펙트생성
	Weapon3DViewCreateEffect();

	m_p3DView = i3UI3DViewBox::new_object();

	view_frame_wnd->AddChild(m_p3DView);
	m_p3DView->setSize(view_frame_wnd->getWidth() - 2, view_frame_wnd->getHeight() - 2);
	m_p3DView->setPos(1, 1);

	m_p3DView->Create3DView();

	i3AttrSet* pAttrSet = m_p3DView->getAttrSetNode();

	if (pAttrSet)
	{
		i3ImageFile refFile;
		i3TextureCube * pTex;

		i3::rc_string strFileName;
		if ( g_pEnvSet->GetUIVersion() == UFV_1_5 || g_pEnvSet->GetUIVersion() == UFV_1_2)
		{
			i3::sprintf(strFileName, "Gui/Showroom/UI_V15/Gunshowcaseroom_Ref.dds");
		}
		else
		{
			i3::sprintf(strFileName, "Gui/Showroom/UI_V10/Gunshowcaseroom_Ref.dds");
		}

		pTex = (i3TextureCube*)refFile.Load( strFileName.c_str());

		I3ASSERT(pTex != NULL);

		if (m_pRefAttr == nullptr)
		{
			m_pRefAttr = i3ReflectMapBindAttr::new_object();
			m_pRefAttr->SetTexture(pTex);
			I3_MUST_RELEASE(pTex);

			pAttrSet->AddAttr(m_pRefAttr);
		}
	}

	g_pFramework->setShadowQuality(0);
	m_p3DView->Set3DView(m_p3DViewNode);
	m_p3DView->OnVisibleScene();
}
void WeaponDetail3DView::ResetObjectPosition(void)
{
	if (left_weapon_obj == 0)
		ResetSingleObjectPos();
	else
		ResetDualObjectPos();
}
void WeaponDetail3DView::ResetCameraPosition(void)
{
	MATRIX * pCameraMat = m_pCamera->GetMatrix();

	i3Matrix::Identity(pCameraMat);
	i3Matrix::SetTranslate(pCameraMat, getX(&m_BasePos), getY(&m_BasePos), getZ(&m_BasePos) + m_BaseCamDist);
}
void WeaponDetail3DView::ResetSingleObjectPos(void)
{
	MATRIX mat;
	VEC3D vecPos;
	
	QUATERNION Quat = weapon_obj->getWeaponInfo()->GetShop3DViewObjQuat();

	VEC3D vec = weapon_obj->getWeaponInfo()->GetShop3DViewObjPos();
	VEC3D centerpos = m_ObjCenterPos;
	if (Quat.x != 0.0f || Quat.y != 0.0f || Quat.z != 0.0f)
	{
		i3Matrix::SetRotateQuat(&mat, &Quat);

		MATRIX* pBoneMat = weapon_obj->GetMatrix();
		i3Matrix::Identity(pBoneMat);
		i3Matrix::PostRotateYDeg(pBoneMat, static_cast<REAL32>(90.0f));
		i3Matrix::PostRotateZDeg(pBoneMat, static_cast<REAL32>(90.0f));


		MATRIX posMat;

		i3Matrix::SetTranslate(&posMat, centerpos.x, centerpos.y, centerpos.z);
		i3Matrix::Mul(pBoneMat, pBoneMat, &posMat);

		i3Matrix::PostRotateYDeg(&posMat, static_cast<REAL32>(180));
		i3Matrix::Mul(pBoneMat, pBoneMat, &posMat);

		i3Matrix::Mul(pBoneMat, pBoneMat, &mat);

		i3Matrix::SetTranslate(&posMat, vec.x, vec.y, vec.z);
		i3Matrix::Mul(pBoneMat, pBoneMat, &posMat);
	}
	else
	{
		CWeaponInfo* pinfo = weapon_obj->getWeaponInfo();

		char pszRes[256] = "";

		INT32 len = i3::generic_string_size(pinfo->GetName());
		if (len > 0 && len < 255)
		{
			GlobalFunc::DecryptValue(pszRes, pinfo->GetName(), len, ENC_SHIFT_WEAPONINFO_RESNAME);
		}

		if (strcmp("PP19-BIZON", pszRes) != 0)
		{
			i3Matrix::PostRotateYDeg(&mat, 90.0f);
			i3Matrix::PostRotateZDeg(&mat, 90.0f);
		}

		centerpos += vec;
		i3Matrix::SetPos(&mat, &centerpos);
		weapon_obj->SetMatrix(&mat);
	}
}
void WeaponDetail3DView::ResetDualObjectPos(void)
{
	i3Transform* fxDummy = (i3Transform*)i3Scene::FindNodeByExactType(m_pDummyNode, i3Transform::static_meta());

	MATRIX mtx;
	
	weapon_obj->SetMatrix(&mtx);
	left_weapon_obj->SetMatrix(&mtx);



	MATRIX mat;
	
	MATRIX posMat;
	VEC3D vecPos = VEC3D(0, 0, 0);
	VEC3D vec = weapon_obj->getWeaponInfo()->GetShop3DViewObjPos();
	vec += m_vecDummyPos;
	i3Matrix::SetTranslate(&posMat, vec.x, vec.y, vec.z);
	fxDummy->SetMatrix(&posMat);
}

bool WeaponDetail3DView::IsWeaponKeyWord(char* pchName, CWeaponInfo* pWeaponInfo)
{
	CWeaponInfo* pinfo = nullptr;
	char pszRes[256] = "";

	if (pWeaponInfo)
	{
		pinfo = pWeaponInfo;
		i3::rc_wstring weaponname;

		pinfo->GetWeaponName(weaponname);
		i3::string temp;
		i3::utf16_to_mb(weaponname, temp);
		strcpy_s(pszRes, (char*)temp.c_str());
	}
	else
	{
		pinfo = weapon_obj->getWeaponInfo();
		INT32 len = i3::generic_string_size(pinfo->GetName());
		if (len > 0 && len < 255)
		{
			GlobalFunc::DecryptValue(pszRes, pinfo->GetName(), len, ENC_SHIFT_WEAPONINFO_RESNAME);
		}
	}

	i3::rc_string rcWeaponName = pszRes;
	INT32 nPos = rcWeaponName.find(pchName);
	if (nPos != -1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void WeaponDetail3DView::mouse_btn_down(INT32 btn_type, POINT point)
{
	switch (btn_type)
	{
	case I3I_MOUSE_LBUTTON:
		i3Matrix::Copy(&m_DragStartMatrix, m_pCamera->GetMatrix());
		m_DragStartPt = point;

		break;

	case I3I_MOUSE_RBUTTON:
		ResetObjectPosition();
		ResetCameraPosition();
		break;
	}
}
void WeaponDetail3DView::mouse_btn_up(INT32 btn_type, POINT point)
{
	switch (btn_type)
	{
	case I3I_MOUSE_LBUTTON:
		break;

	case I3I_MOUSE_RBUTTON:
		break;
	}
}
void WeaponDetail3DView::mouse_move(POINT point, bool LeftMBTNDown)
{
	if (LeftMBTNDown)
	{
		const REAL32 amount = -0.5f;
		REAL32 xDeg, yDeg;

		xDeg = (REAL32)(m_DragStartPt.x - point.x) * amount;
		yDeg = (REAL32)(m_DragStartPt.y - point.y) * amount;

		MATRIX mtx;
		
		i3Matrix::PostRotateXDeg(&mtx, yDeg);
		i3Matrix::PostRotateYDeg(&mtx, xDeg);

		if (left_weapon_obj)
		{
			i3Transform* fxDummy = (i3Transform*)i3Scene::FindNodeByExactType(m_pDummyNode, i3Transform::static_meta());
			i3Matrix::Mul(fxDummy->GetMatrix(), fxDummy->GetMatrix(), &mtx);
		}
		else
		{
			i3SceneObject * pSgObj = static_cast<i3SceneObject*>(i3Scene::FindNodeByExactType(m_p3DView->getAttrSetNode(), i3SceneObject::static_meta()));
			i3Matrix::Mul(pSgObj->GetMatrix(), pSgObj->GetMatrix(), &mtx);
		}


		m_DragStartPt.x = point.x;
		m_DragStartPt.y = point.y;
	}

	_WeaponCollisonTestCam();
}
void WeaponDetail3DView::mouse_wheel(INT32 wheel_type, POINT point)
{
	VEC3D vec;
	MATRIX * pCameraMat = m_pCamera->GetMatrix();
	MATRIX tmp;
	REAL32 len = 0.05f;

	i3Vector::Copy(&vec, i3Matrix::GetAt(pCameraMat));
	i3Vector::Normalize(&vec, &vec);

	VEC3D* cam_pos = i3Matrix::GetPos(pCameraMat);
	REAL32 length = i3Vector::Length(cam_pos);

	if (wheel_type == I3I_MOUSE_WHEELUP)
	{
		if (length < m_ZoomMinDist) len = 0;
		i3Vector::Scale(&vec, &vec, -m_BaseCamDist * len);
	}
	else
	{
		if (length > m_ZoomMaxDist) len = 0;
		i3Vector::Scale(&vec, &vec, m_BaseCamDist * len);
	}

	i3Matrix::SetTranslate(&tmp, getX(&vec), getY(&vec), getZ(&vec));
	i3Matrix::Mul(pCameraMat, pCameraMat, &tmp);

	OnUpdateCameraTransform();
	OnUpdateCameraProjection();

	i3Matrix::Copy(&m_DragStartMatrix, pCameraMat);
	m_DragStartPt = point;
}
void WeaponDetail3DView::Zoom(REAL32 rate)
{
	VEC3D vec;
	MATRIX * pCameraMat = m_pCamera->GetMatrix();
	MATRIX tmp;

	rate = i3Math::Mul(rate, 2.0f);

	i3Vector::Copy(&vec, i3Matrix::GetAt(pCameraMat));
	i3Vector::Normalize(&vec, &vec);
	i3Vector::Scale(&vec, &vec, rate);

	i3Matrix::SetTranslate(&tmp, getX(&vec), getY(&vec), getZ(&vec));

	i3Matrix::Mul(pCameraMat, pCameraMat, &tmp);
}

void WeaponDetail3DView::OnUpdateCameraTransform()
{
}
void WeaponDetail3DView::OnUpdateCameraProjection()
{
	switch (m_pCamera->GetMode())
	{
	case i3Camera::I3_CAMERA_MODE_ORTHOGONAL:
	{
		REAL32 w, h, l;
		MATRIX * pCameraMat = m_pCamera->GetMatrix();
		VEC3D vec, pc;

		i3Vector::Copy(&vec, i3Matrix::GetAt(pCameraMat));
		i3Vector::Normalize(&vec, &vec);
		i3Vector::Scale(&vec, &vec, -1.0f);

#if defined( I3_FIXED_POINT)
		VEC3D campos, basepos;

		i3Vector::Copy(&basepos, &m_BasePos);
		i3Vector::Copy(&campos, i3Matrix::GetPos(pCameraMat));
		l = NearestPlanePoint(&basepos, &vec, &campos, &pc);
#else
		l = NearestPlanePoint(&m_BasePos, &vec, i3Matrix::GetPos(pCameraMat), &pc);
#endif

		l = i3Math::abs(l);

		w = l;
		h = l * ((REAL32)m_p3DView->getHeight() / m_p3DView->getWidth());

		m_pCamera->SetOrthogonal(w, h, m_pCamera->GetNear(), m_pCamera->GetFar());
	}
	break;

	default:
	{
		REAL32 w, h;

#if defined( I3_FIXED_POINT)
		w = i3FXD::FromInt(m_p3DView->getWidth());
		h = i3FXD::FromInt(m_p3DView->getHeight());
#else
		w = (REAL32)m_p3DView->getWidth();
		h = (REAL32)m_p3DView->getHeight();
#endif

		m_pCamera->SetPerspective(m_pCamera->GetFOV(), w, h, m_pCamera->GetNear(), m_pCamera->GetFar());
	}
	break;
	}
}

void WeaponDetail3DView::_WeaponCollisonTestCam()
{
	MATRIX * pCameraMat = m_pCamera->GetMatrix();
	VEC3D* cam_pos = i3Matrix::GetPos(pCameraMat);
	i3::vec3 aabb_center;
	aabb_center.x = cam_pos->x;
	aabb_center.y = cam_pos->y;
	aabb_center.z = cam_pos->z;

	i3::aabb cam_aabb;
	cam_aabb.set_center_extent(aabb_center, i3::vec3(0.1f,0.1f,0.1f) );

	const i3::aabb* weapon_aabb = nullptr;
	if( weapon_obj ) weapon_aabb = weapon_obj->getSceneObject()->GetAABB();

	const i3::aabb* left_weapon_aabb = nullptr;
	if(left_weapon_obj) left_weapon_aabb = left_weapon_obj->getSceneObject()->GetAABB();

	bool test_weapon_result = false;
	bool test_left_weapon_result = false;

	if (weapon_aabb)		test_weapon_result = aabb_aabbCollisontest(weapon_aabb, &cam_aabb);
	if (left_weapon_aabb)	test_left_weapon_result = aabb_aabbCollisontest(left_weapon_aabb, &cam_aabb);

	if (test_weapon_result || test_left_weapon_result)
	{
		_ForceCamZoomout();
	}
}

void WeaponDetail3DView::_ForceCamZoomout()
{
	VEC3D vec;
	MATRIX * pCameraMat = m_pCamera->GetMatrix();
	MATRIX tmp;
	REAL32 len = 0.05f;

	i3Vector::Copy(&vec, i3Matrix::GetAt(pCameraMat));
	i3Vector::Normalize(&vec, &vec);

	VEC3D* cam_pos = i3Matrix::GetPos(pCameraMat);
	REAL32 length = i3Vector::Length(cam_pos);

	if (length > m_ZoomMaxDist) len = 0;
	i3Vector::Scale(&vec, &vec, m_BaseCamDist * len);

	i3Matrix::SetTranslate(&tmp, getX(&vec), getY(&vec), getZ(&vec));
	i3Matrix::Mul(pCameraMat, pCameraMat, &tmp);

	OnUpdateCameraTransform();
	OnUpdateCameraProjection();

	i3Matrix::Copy(&m_DragStartMatrix, pCameraMat);
}

void WeaponDetail3DView::Set_ChangeDist_RealTime(bool plus)
{
	plus ? m_Dist += m_change_dist : m_Dist -= m_change_dist;

	i3SceneObject* weapon_obj_scene = weapon_obj->getSceneObject();
	i3BoundBox* obj_bound_box = weapon_obj_scene->GetBody()->GetBound();

	VEC3D Min;
	VEC3D Max;

	Min = *obj_bound_box->GetMin();
	Max = *obj_bound_box->GetMax();

	VEC3D vec;
	
	i3Vector::Sub(&vec, &Max, &Min);

	m_BaseCamDist = i3Math::Mul(i3Vector::Length(&vec), 1.0f + m_fDummyDist + m_Dist);

	ResetCameraPosition();
}
void WeaponDetail3DView::Set_ChangeDist_Offset(bool plus)
{
	if (m_change_dist < 1.0f && plus)
		m_change_dist *= 10.0f;

	if (m_change_dist > 0.001f && !plus)
		m_change_dist *= 0.1f;
}
const i3::rc_wstring  WeaponDetail3DView::GetTest_Data()
{
	char _output[256];
	sprintf(_output, " 최종 카메라 기본 거리 값 : %1.3f (더미 거리 %1.1f) , 수정 한 거리(PEF용) : %1.3f [키보드 이동거리 %1.3f] \n - MAKE KDY 2015", m_BaseCamDist, m_fDummyDist, m_Dist, m_change_dist );

	i3::rc_wstring output;
	i3::mb_to_utf16( _output, output);

	return output;
}

#pragma pop_macro("max")
#pragma pop_macro("min")
