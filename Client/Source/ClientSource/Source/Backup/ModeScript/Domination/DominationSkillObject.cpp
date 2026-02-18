#include "pch.h"
#include "DominationSkillObject.h"

#include "DominationSkillSystem.h"
#include "../../CommonSource/CommonDediCli.h"
#include "../../UI/UIBattleFrame.h"

#include "../../BattleHud.h"

namespace
{
	bool __CalcProjectionPos( REAL32 * pOutX, REAL32 * pOutY, VEC3D * pIn )
	{
		VEC3D	vTemp;
		VEC3D	vTempIn;
		MATRIX	CamMat;
		VEC3D*	pvCamPos;
		VEC3D*	pvCamAt;
		VEC3D	vSub;
		REAL32	Height = 1.9f;

		pvCamPos = i3Matrix::GetPos( g_pCamera->getCamMatrix());
		pvCamAt = i3Matrix::GetAt( g_pCamera->getCamMatrix());

		i3Vector::Sub( &vSub, pvCamPos, pIn);
		i3Vector::Normalize( &vSub, &vSub );

		if( i3Vector::Dot( pvCamAt, &vSub) < 0.f ) 
			return false;

		i3Matrix::Mul( &CamMat, g_pCamera->getViewMatrix(), g_pCamera->getProjectionMatrix());

		i3Vector::Set( &vTempIn, i3Vector::GetX( pIn), i3Vector::GetY( pIn) + Height, i3Vector::GetZ( pIn));

		i3Vector::TransformCoord( &vTemp, &vTempIn, &CamMat);

		*pOutX	= i3Vector::GetX( &vTemp );
		*pOutY	= i3Vector::GetY( &vTemp );

		return true;
	}
}

namespace DominationSkill
{
	//----------------------------------------------
	// CIdentity : 스킬 오브젝트 고유 속성
	//----------------------------------------------
	CIdentity::CIdentity(void) : m_ItemID(0), m_TYPE_SLOT(-1), m_ObjIdx(-1), m_Nick("")
	{
	}

	CIdentity::~CIdentity(void)
	{
	}

	void CIdentity::Instantiate( i3Chara* pChara)
	{
		MATRIX mat;
		I3_ONINSTANCING_INFO info;
		info._pBase = pChara;
		info._pMatrixStack = g_pFramework->GetViewer()->GetSceneTracer()->GetModelViewMatrixStack();
		
		i3Matrix::Identity( &mat);
		i3Matrix::SetPos( &mat, 0.f, g_pConfigEx->GetPhysix().HeightWeapon, 0.f);
		
		info._pMatrixStack->Push( &mat, NULL);
		pChara->OnInstancing( &info);
		info._pMatrixStack->Pop();
	}

	//------------------------------------------------------------------
	// CServiceable : 스킬 오브젝트 상태 및 요청자 SP에 따른 서비스 처리
	//------------------------------------------------------------------
	struct Actor
	{
		virtual ~Actor() {}
		virtual void operator()( CServiceable* o, CGameCharaBase* customer) {}
	};

	struct Actor_None : Actor
	{	
		virtual void operator()( CServiceable* o, CGameCharaBase* customer) {} 
	} g_actor_none;

	struct Actor_Install : Actor 
	{
		virtual void operator()( CServiceable* o, CGameCharaBase* customer) 
		{
			INT32 customerIdx = customer->getCharaNetIndex();
			INT8 objectIdx = static_cast<INT8>(o->GetID());
			GAMEEVENT::Write_DomiSkillObjectInstall(customerIdx, objectIdx);
		}
	} g_actor_install;

	struct Actor_Upgrade : Actor
	{
		virtual void operator()( CServiceable* o, CGameCharaBase* customer) 
		{
			INT32 customerIdx = customer->getCharaNetIndex();
			INT8 objectIdx = static_cast<INT8>(o->GetID());
			INT8 lv = o->GetWeapon() ? static_cast<INT8>(o->GetWeapon()->getWeaponInfo()->GetLevel()+1) : 0;
			GAMEEVENT::Write_DomiSkillObjectUpgrade(customerIdx, objectIdx, lv);
		}
	} g_actor_upgrade;

	struct Actor_Repair : Actor
	{
		virtual void operator()( CServiceable* o, CGameCharaBase* customer) 
		{
			INT32 customerIdx = customer->getCharaNetIndex();
			INT8 objectIdx = static_cast<INT8>(o->GetID());
			GAMEEVENT::Write_DomiSkillObjectRepair(customerIdx, objectIdx);
		}
	} g_actor_repair;

	struct Actor_Supplement_HP_AMMO : Actor
	{
		virtual void operator()( CServiceable* o, CGameCharaBase* customer) 
		{
			INT32 customerIdx = customer->getCharaNetIndex();
			INT8 objectIdx = static_cast<INT8>(o->GetID());
			GAMEEVENT::Write_DomiSkillObjectSupplyUse(customerIdx, objectIdx);
		}
	} g_actor_supplement_hp_ammo;

	struct Actor_UserSkill_Shop : Actor
	{
		virtual void operator()( CServiceable* o, CGameCharaBase* customer)
		{
			I3TRACE("OPEN USER_SKILL_SHOP\n");
			// E버튼을 누름에 따라 상점 UI 출력
			UIBattleFrame::i()->OpenPopup(UBP_SKILL_SHOP);
		}
	} g_actor_user_skill_shop;

	CServiceable::CServiceable() : m_state(STATE_NONE)
	{
		m_map.insert(std::tr1::unordered_map<UINT32, Actor*>::value_type(STATE_INSTALLABLE, &g_actor_install));
		m_map.insert(std::tr1::unordered_map<UINT32, Actor*>::value_type(STATE_UPGRADABLE, &g_actor_upgrade));
		m_map.insert(std::tr1::unordered_map<UINT32, Actor*>::value_type(STATE_REPAIRABLE, &g_actor_repair));
		m_map.insert(std::tr1::unordered_map<UINT32, Actor*>::value_type(STATE_HP_RECOVER, &g_actor_supplement_hp_ammo));
		m_map.insert(std::tr1::unordered_map<UINT32, Actor*>::value_type(STATE_AMMO_RECOVER, &g_actor_supplement_hp_ammo));
		m_map.insert(std::tr1::unordered_map<UINT32, Actor*>::value_type(STATE_SHOP_SALABLE, &g_actor_user_skill_shop));

		m_flagPermitUserIdx.Clear();
	}

	CServiceable::~CServiceable()
	{
		m_state = STATE_NONE;
		m_map.clear();
		m_flagPermitUserIdx.Clear();
	}

	void CServiceable::OnButtonClicked(CGameCharaBase* customer)
	{
		if (customer == NULL) return;
		if (m_flagPermitUserIdx.TestBit(customer->getCharaNetIndex()) == false) return;

		std::tr1::unordered_map<UINT32, Actor*>::iterator it = m_map.find(m_state);
		if (it != m_map.end())
		{
			Actor* o= it->second;
			(*o)(this, customer);
		}
	}

	//----------------------------------------------
	// CRawObject : 설치 전 투명 스킬 오브젝트
	//----------------------------------------------
	CRawObject::CRawObject(void) : m_pChara(NULL), m_pAmbientAttr(NULL), m_Info(NULL)
	{
	}

	CRawObject::~CRawObject(void)
	{
		I3_SAFE_RELEASE( m_pChara);
	}

	void CRawObject::OnCreate( void* pRes, UINT32 ItemID, INT16 Type, INT16 Idx)
	{
		CIdentity::OnCreate( pRes, ItemID, Type, Idx);

		i3GameResChara* p = static_cast<i3GameResChara*>(pRes);

		m_pChara = i3Chara::new_object();
		m_pChara->Create( g_pFramework, p);
		Instantiate( m_pChara);

		{// Alpha
			i3AttrSet* pAttrSet = i3AttrSet::new_object_ref();
			pAttrSet->SetPriority( 0);	

			while( m_pChara->getAttachNode()->GetChildCount() > 0)
			{
				i3Node * pChild = m_pChara->getAttachNode()->GetChild( 0);
				pChild->AddRef();

				pChild->RemoveFromParent();
				pAttrSet->AddChild( pChild);

				pChild->Release();
			}	
			m_pChara->getAttachNode()->AddChild( pAttrSet);		

			{
				i3BlendEnableAttr* pAttr = i3BlendEnableAttr::new_object_ref();
				pAttr->Set( TRUE);
				pAttrSet->AddAttr( pAttr);
				pAttr->Set( true);
			}

			{
				i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();
				pAttr->SetSource( I3G_BLEND_SRCALPHA);
				pAttr->SetDestination( I3G_BLEND_INVSRCALPHA);
				pAttrSet->AddAttr( pAttr);
			}
			
			{
				i3MaterialAttr* pAttr = i3MaterialAttr::new_object_ref();
				pAttrSet->AddAttr( pAttr);

				COLORREAL col;

				i3Color::Set( &col, 1.0f, 1.0f, 1.0f, 1.0f);
				pAttr->SetDiffuse( &col);
				pAttr->SetSpecular( &col);

				pAttr->setValidEmissive( false);
				pAttr->setValidSpecular( true);
				pAttr->setValidShininess( false);

				i3Color::SetA( pAttr->GetDiffuse(), 0.5f);
				i3Color::SetA( pAttr->GetSpecular(), 0.5f);
			}

			{
				m_pAmbientAttr = i3AmbientAttr::new_object_ref();

				COLORREAL col;
				i3Color::Set( &col, 0.f, 0.7f, 0.f, 1.f);
				m_pAmbientAttr->Set( &col);

				pAttrSet->AddAttr( m_pAmbientAttr);
			}
		}

		i3::map<UINT32, dss::Info*>::iterator itr = tinst<CDominationSkillSystem>()->m_InfoList.find( m_ItemID);
		I3ASSERT( itr != tinst<CDominationSkillSystem>()->m_InfoList.end());

		m_Info = itr->second;
	}

	void CRawObject::OnInstall( VEC3D* pPos, REAL32 theta, CGameCharaBase* chara /*= NULL*/)
	{
		m_pChara->SetPos( pPos);
		m_pChara->AttachScene();
		m_pChara->SetEnable( TRUE);
	}

	VEC3D CRawObject::GetBoundSize()
	{
		VEC3D size;
		m_pChara->getSceneObject()->GetBody()->GetBound()->getSize( &size);
		return size;
	}

	VEC3D CRawObject::GetPos() const
	{
		MATRIX mat;
		GlobalFunc::GetNowFrameMatrix( &mat, m_pChara->getSceneObject());

		VEC3D pos = *(i3Matrix::GetPos( &mat));
		return pos;
	}

	//----------------------------------------------
	// CCompletedObject : 설치된 스킬 오브젝트
	//----------------------------------------------
	CCompletedObject::CCompletedObject(void) : m_pWeapon(NULL), m_owner_nick("")
	{
	}

	CCompletedObject::~CCompletedObject(void)
	{
	}
	
	void CCompletedObject::OnCreate( void* pRes, UINT32 ItemID, INT16 Type, INT16 Idx)
	{
		CIdentity::OnCreate( pRes, ItemID, Type, Idx);

		WeaponBase* pWeapon = static_cast<WeaponBase*>(pRes);

		m_pWeapon = pWeapon;
		m_pWeapon->setNetworkIdx( Idx);
		m_pWeapon->setObjectType( static_cast<DOMI_TYPE>(Type));
		m_pWeapon->SetEnable( true);
	}

	void CCompletedObject::OnDestroy()
	{
		m_pWeapon->ReturnInstance();
	}
	
	void CCompletedObject::OnInstall( VEC3D* pPos, REAL32 theta, CGameCharaBase* chara /*= NULL*/)
	{
		i3Stage* pStage = g_pFramework->GetCurrentStage();
		i3World* pWolrd = pStage->getWorld();
		I3ASSERT( pWolrd != NULL);

		m_pWeapon->SetOwner( chara);
		m_pWeapon->InstallToWorld( pWolrd->getSceneObject(), pWolrd, pPos, true, NULL, theta);

		InitializeAtInstall( chara);
	}

	VEC3D CCompletedObject::GetBoundSize()
	{
		VEC3D size;
		m_pWeapon->getSceneObject()->GetBody()->GetBound()->getSize( &size);
		return size;
	}

	VEC3D CCompletedObject::GetPos() const
	{
		MATRIX mat;
		GlobalFunc::GetNowFrameMatrix( &mat, m_pWeapon->getSceneObject());

		VEC3D pos = *(i3Matrix::GetPos( &mat));
		return pos;
	}

	void CCompletedObject::InitializeAtInstall( CGameCharaBase* chara /*= NULL*/)
	{
		m_Nick = "Lv ";

		//level
		char buf[16] = {0,};
		i3::map<UINT32, dss::Info*>::iterator itr = tinst<CDominationSkillSystem>()->m_InfoList.find( m_ItemID);
		I3ASSERT( itr != tinst<CDominationSkillSystem>()->m_InfoList.end());
		if( itr != tinst<CDominationSkillSystem>()->m_InfoList.end())
			itoa( itr->second->Level, buf, 10);

		//
		m_Nick += buf;


		//chara nick
		m_owner_nick = "";
		if( chara != NULL)
			m_owner_nick = g_pGameContext->getNickForSlot( chara->getCharaNetIndex());
	}

	void CCompletedObject::ShowNick()
	{
		REAL32 x, y;
		VEC3D pos = GetPos();
		if( __CalcProjectionPos( &x, &y, &pos))
		{
			x *= 0.5f;		x += 0.5f;
			y *= -0.5f;		y += 0.5f;

			CGameCharaBase* player = g_pCharaManager->getCharaByNetIdx( g_pGameContext->getMySlotIdx());
			I3ASSERT( player != NULL);

			VEC3D vTemp;
			i3Vector::Sub( &vTemp, &pos, player->GetPos());
			INT32 nLength = (INT32)i3Vector::Length(&vTemp);
			if( nLength > 9000 || nLength < -9000) nLength = 0;

			CHud::i()->SetCharaNick( m_Nick.c_str(), true, x, y, (REAL32)nLength);
		}
	}

	//----------------------------------------------
	// 실제 오브젝트들...
	//----------------------------------------------
	void CDummy::OnUnInstall()
	{	
		if( WeaponDinoDummy* p = i3::same_cast<WeaponDinoDummy*>(m_pWeapon))
			p->Uninstall();
	}

	void CDummy::InitializeAtInstall( CGameCharaBase* chara /*= NULL*/)
	{
		CCompletedObject::InitializeAtInstall( chara);

		m_Nick += " ";
		m_Nick += m_owner_nick;
		m_Nick += " 's Dummy";	
	}

	void CSentryGun::OnUnInstall()
	{
		if( WeaponSentryGun* p = i3::same_cast<WeaponSentryGun*>(m_pWeapon))
			p->Uninstall();
	}

	void CSentryGun::InitializeAtInstall( CGameCharaBase* chara /*= NULL*/)
	{
		CCompletedObject::InitializeAtInstall( chara);

		m_Nick += " ";
		m_Nick += m_owner_nick;
		m_Nick += " 's SentryGun";	
	}

	void CSupply::OnUnInstall()
	{
		if( WeaponSupply* p = i3::same_cast<WeaponSupply*>(m_pWeapon))
			p->Uninstall();
	}
};