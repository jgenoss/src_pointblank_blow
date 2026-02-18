#pragma once

#include "UIHUD_Domination_Base.h"
#include "UIHudDomi_Flow.h"

#include "i3Base/itl/map.h"

// 스킬 이벤트 리스트
enum	HUD_DOMINATION_EVENT_INDICATOR_TYPE
{
	HUD_DMN_EVT_CLEAR_INDICATIONS	= 0,
	
	HUD_DMN_EVT_DELETE_INSTALL_OBJECT,
	HUD_DMN_EVT_DELETE_STAGE_OBJECT,
	HUD_DMN_EVT_DELETE_CHARACTER_OBJECT,
	HUD_DMN_EVT_DELETE_RESPAWN_OBJECT,
	HUD_DMN_EVT_DELETE_MEATBOMB,
	HUD_DMN_EVT_OBJ_MAX
};

using namespace ModeScript;

class	gmode_domination;
class	i3Texture;
class	i3Sprite2D;
class	i3TextNodeDX2;

namespace UI_Util
{
	class CIndicateHUD
	{
		friend class CIndicateMeatBomb;
		i3Sprite2D*				m_pSprite;
		i3TextNodeDX2*			m_pText;

	public:
		REAL32					m_fLengh;
		REAL32					m_fSize;

		bool					m_bInScreen;
		REAL32					m_fInterpolateTime;
		VEC2D					m_vPrePos;
		VEC2D					m_vPos;

		CIndicateHUD() : m_pSprite(nullptr), m_pText(nullptr), m_bInScreen(false), m_fInterpolateTime(0.f), m_fLengh(0.f) {}
		~CIndicateHUD() { I3_SAFE_RELEASE(m_pSprite); I3_SAFE_RELEASE(m_pText); }

		void OnCreate( 	i3AttrSet* pHUDRoot, i3Texture* pTexture);
		void SetEnable( bool bEnable, INT32 iType = 0);
		void SetRect( UINT32 Index, REAL32 x, REAL32 y, REAL32 width, REAL32 height) { m_pSprite->SetRect( Index, x, y, width, height); m_pText->setPos( (UINT32)x, (UINT32)(y + height)); }
		void SetText( const i3::wliteral_range& wTextRng) { m_pText->SetText( wTextRng); }
	};

	enum OBJECT_TYPE
	{
		NONE,
		INSTALL_OBJECT,
		STAGE_OBJECT,
		RESPAWN_OBJECT,
		CHARACTER_OBJECT,

		//
		DOT_OBJECT,
		MEATBOMB,
	};

	enum IDC_STATE { IDC_IN_SCREEN = 0, IDC_OUT_SCREEN};

	class CIndicateObject
	{
	protected:
		INT32 m_nHudType;
		i3GameObj* m_pObj;
		CIndicateHUD* m_pHud;

		virtual void _Init() {}

	public:
		IDC_STATE m_IdcCur;
		dui::time_event_ptr event_ptr;

		CIndicateObject() : m_nHudType(-1), m_pHud(nullptr), m_pObj(nullptr), m_IdcCur(IDC_IN_SCREEN) { event_ptr.reset();}
		virtual ~CIndicateObject() {}

		virtual bool IsSame( INT32 idx, INT32 nObjectType) { return false; }
		virtual void Create( i3GameObj* pObj, INT32 iType);
	
		virtual OBJECT_TYPE GetObjectType() = 0;
		INT32 GetHudType() { return m_nHudType; }
		i3GameObj* GetObject() { return m_pObj; }
		CIndicateHUD* GetHud() { return m_pHud; }

		virtual VEC3D* GetObjPos() { return m_pObj->GetPos();}
	};

	class CIndicateInstallObject : public CIndicateObject
	{
	public:
		CIndicateInstallObject( CIndicateHUD* pHud) { m_pHud = pHud; }
		~CIndicateInstallObject() {}

		OBJECT_TYPE GetObjectType() { return INSTALL_OBJECT; }
		bool IsSame( INT32 idx, INT32 nObjectType);
	};

	class CIndicateStageObject : public CIndicateObject
	{
	public:
		CIndicateStageObject( CIndicateHUD* pHud) { m_pHud = pHud; }
		~CIndicateStageObject() {}

		OBJECT_TYPE GetObjectType() { return STAGE_OBJECT; }
		bool IsSame( INT32 idx, INT32 nObjectType);
	};

	class CIndicateRespawnObject : public CIndicateObject
	{
	public:
		CIndicateRespawnObject( CIndicateHUD* pHud) { m_pHud = pHud; }
		~CIndicateRespawnObject() {}

		OBJECT_TYPE GetObjectType() { return RESPAWN_OBJECT; }
		bool IsSame( INT32 idx, INT32 nObjectType);
	};

	class CIndicateCharacter : public CIndicateObject
	{
		INT32 m_nNetIdx;		//
	public:
		CIndicateCharacter( CIndicateHUD* pHud) { m_pHud = pHud; }
		~CIndicateCharacter() {}

		OBJECT_TYPE GetObjectType() { return CHARACTER_OBJECT; }
		bool IsSame( INT32 idx, INT32 nObjectType);

		void _Init();
	};

	class CIndicateDot : public CIndicateObject
	{
		VEC3D m_vPos;
		INT32 m_nIdx;
	public:
		CIndicateDot( CIndicateHUD* pHud)
		{ 
			m_pHud = pHud;
			m_nIdx = -1;
			memset( &m_vPos, 0, sizeof(VEC3D));
		}
		~CIndicateDot() {}

		void SetDotIdx( INT32 idx) { m_nIdx = idx;}
		INT32 GetDotIdx() const { return m_nIdx;}

		OBJECT_TYPE GetObjectType() { return DOT_OBJECT; }
		bool IsSame( INT32 idx, INT32 nObjectTyp);

		VEC3D* GetObjPos() { return &m_vPos;}
	};

	static const REAL32 explosionDelayTime = 10.0f;

	class CIndicateMeatBomb : public CIndicateObject
	{
	public:
		explicit CIndicateMeatBomb( CIndicateHUD* pHud) : m_networkIdx(-1), m_explosionRemainTime(explosionDelayTime)
		{ 
			m_pHud = pHud; 

			m_textExplosionCount = i3TextNodeDX2::new_object();
			m_textExplosionCount->Create(BATTLE_CHARA_SET, FONT_COUNT_32, 15, FW_NORMAL, false, false, ANTIALIASED_QUALITY);
			m_textExplosionCount->setSize(70, 50);
			m_textExplosionCount->SetColor(255, 0, 0, 255);
			m_textExplosionCount->SetEnable(true);
			pHud->m_pText->GetParent()->AddChild(m_textExplosionCount);
		}

		virtual ~CIndicateMeatBomb() 
		{
			m_explosionRemainTime = 0.0f;
			m_textExplosionCount->SetEnable(false);
		}

		OBJECT_TYPE GetObjectType() { return MEATBOMB; }
		bool IsSame( INT32 idx, INT32 nObjectType);

		void  SetNetIdx(INT32 netIdx)	{ m_networkIdx = netIdx; }
		INT32 GetNetIdx() const			{ return m_networkIdx; }

		const i3TextNodeDX2* GetExplosionCountText() const { return m_textExplosionCount; }
		i3TextNodeDX2*		 GetExplosionCountText()	   { return m_textExplosionCount; }

		REAL32	DecreaseExplosionRemainTime(REAL32 tm)   { return m_explosionRemainTime -= tm; }

	private:
		INT32			m_networkIdx;
		i3TextNodeDX2*	m_textExplosionCount;
		REAL32			m_explosionRemainTime;

		CIndicateMeatBomb(const CIndicateMeatBomb& src);
		CIndicateMeatBomb& operator=(const CIndicateMeatBomb& src);
	};
};

namespace tmp_uu = UI_Util;

// 다른 HUD와는 다르게 구 UI를 사용한다.
class CUIHudDomi_Indicator : public UIHUDDominationBase
{
	I3_CLASS_DEFINE( CUIHudDomi_Indicator, UIHUDDominationBase );	
	
private:
	bool	_CalcProjectionPos( REAL32 * fOutX, REAL32 * fOutY, VEC3D * vIn);	// HUD_TargerHP 에서 그냥 가져온 것 나중에 합쳐도 됨
	tmp_uu::IDC_STATE _ProcessIndicate( VEC3D* pPos, UI_Util::CIndicateHUD* pHud);
	

	void	_FindInstallObject();
	void	_DeleteIndication( INT32 idx, INT32 nObjectType);
	void	_ClearIndications( tmp_uu::OBJECT_TYPE obj_type = tmp_uu::NONE);

public:
	CUIHudDomi_Indicator(void);
	virtual ~CUIHudDomi_Indicator(void);

	virtual void	OnCreate( i3GameNode* pParent ) override;
	virtual void	OnUpdate( REAL32 rDeltaSec ) override;
	virtual bool	OnEntranceStart( void* arg1 = nullptr, void* arg2 = nullptr ) override;
	virtual bool	OnExitStart(void) override;

	virtual bool	SetHudEvent( INT32 nEventNum, INT32 arg = 0 ) override;

	UI_Util::CIndicateObject*			InsertIndication( i3GameObj* pObject, INT32 nHudType, INT32 nObjectType);
	UI_Util::CIndicateObject*			Find( INT32 Idx, INT32 nObjectType);
private:
	gmode_domination*		m_pDomination;

	i3AttrSet *				m_pHUDRoot;
	i3Texture*				m_pTexture;
	
	i3::vector<UI_Util::CIndicateHUD*>		m_vecFreeHuds;		//사용 가능한 HUD들
	i3::vector<UI_Util::CIndicateObject*>	m_vecIndications;

public:
	dui::time_event_list	m_time_event_list;
	void cb_time_event( void* p1, void* p2);

	//
	INT32 m_dot_count;
	i3::vector_map<INT32, VEC3D> m_dot_idx_list;
};
