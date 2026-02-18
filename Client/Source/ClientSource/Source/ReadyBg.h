#ifndef _ReadyBg_h
#define _ReadyBg_h

#include "StageReady.h"
#include "GameCharaDef.h"

//	배경 모델 더미
enum BG_DUMMY_TRANSFORM
{
	BG_DUMMY_TRANSFORM_UNKNOWN = -1,
	BG_DUMMY_TRANSFORM_CHOPPER,
	BG_DUMMY_TRANSFORM_TRUCK,
	BG_DUMMY_TRANSFORM_GUNFIRE1,
	BG_DUMMY_TRANSFORM_GUNFIRE2,
	BG_DUMMY_TRANSFORM_EMERGENCY,

	BG_DUMMY_TRANSFORM_COUNT
};

enum CameraAnimationState
{
	CAS_IDLE = 0,
	CAS_IDLE_TO_LEFT,
	CAS_IDLE_TO_RIGHT,
	CAS_LEFT_TO_IDLE,
	CAS_LEFT_TO_RIGHT,
	CAS_RIGHT_TO_IDLE,
	CAS_RIGHT_TO_LEFT,
	
	CAS_MAX
};

class CGameCharaBase;

class CReadyBg: public i3GameNode
{
	I3_CLASS_DEFINE(CReadyBg);
public:
	CReadyBg();
	virtual ~CReadyBg();

	virtual void Create(i3GameNode* pParentNode);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);

	void		OnChangeVideo( i3VideoInfo * pInfo);

	BOOL UpdateCharacter(ViewType viewType,BOOL bWeaponLimit, VIEW_CHARA_INFO * pNewCharaInfo = NULL);
	void PlayCeremonyCharacter();
	void MoveCamera(CameraLook lookState = CL_IDLE);
	
	// 클랜 마크 관련.
	void CharaChangeClanMarkInReadyBg();	//	뷰 모델에 클랜 마크 적용

	ViewType GetCurrentViewType(void)		{ return m_currentViewType; }
	CGameCharaBase * GetViewChara(void)	{ return m_pCharacter; }
	VIEW_CHARA_INFO * GetViewCharaInfo(void)		{ return &m_viewCharaInfo; }

	void	_UpdateLight(void);

	i3Transform * GetBGTransformDummy( BG_DUMMY_TRANSFORM node)			{ I3_BOUNDCHK( node, BG_DUMMY_TRANSFORM_COUNT); return m_pBGTransformDummy[node]; }
	i3Camera * GetCamera(void)											{ return (i3Camera *) i3Scene::FindNodeByExactType( m_pCameraRoot, i3Camera::GetClassMeta()); }
private:
	void _InitAttrSet(void);
	void _InitLight(void);
	BOOL _InitBackground(void);
	void _InitCamera(void);	
	BOOL _RecreateDefaultChara(void);
	BOOL _RecreateSlotChara(void);
	BOOL _RecreateOtherChara(VIEW_CHARA_INFO * pViewCharaInfo);
	BOOL _RecreateDefaultAndSlotWeapon(NET_CHARA_INFO* pNetCharaInfo);
	BOOL _RecreateOtherWeapon(VIEW_CHARA_INFO * pNewCharaInfo);
	BOOL _RecreateDefaultAndSlotEquip(NET_CHARA_INFO* pNetCharaInfo);
	BOOL _RecreateOtherEquip(VIEW_CHARA_INFO * pNewCharaInfo);
	const char* _GetCameraAnimationFileName(CameraAnimationState state) const;

	void _CreateViewChara(VIEW_CHARA_INFO * pInfo);
	void _RemoveViewChara(void);

private:
	i3AttrSet* m_pAttrSet;
	i3Light* m_pLight;
	i3Light *	m_pPointLight;
	i3Node* m_pBackground;
	i3Transform* m_pBGTransformDummy[ BG_DUMMY_TRANSFORM_COUNT];
	
	VIEW_CHARA_INFO		m_viewCharaInfo;
	CGameCharaBase*	m_pCharacter;
	REAL32				m_rCharaChangePoseTime;

	REAL32		m_LightTheta;
	REAL32		m_LightRho;
	REAL32		m_LightDist;
	REAL32		m_timeLocal;

	ViewType m_currentViewType;

	i3Node* m_pCameraRoot;
	i3Node* m_pCameraAnimation[CAS_MAX];
	CameraAnimationState m_cameraState;

	INT32		m_lastViewTeam;	
	INT32		m_lastWeaponUsage;	
};

#endif //_ReadyBg_h
