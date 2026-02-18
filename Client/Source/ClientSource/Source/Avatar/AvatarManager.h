#pragma once
#include <queue>
#include "Avatar.h"
#include "AvatarThread.h"
#include "AvatarLoadMacroTexThread.h"

#define MAX_AVATAR_INFO_COUNT		16
#define LOCKINFO_BUF_COUNT	32
#define LOADMACROTEXQUE_COUNT		512

class Avatar;

class cAvatarManager : public i3ElementBase
{
	I3_CLASS_DEFINE( cAvatarManager, i3ElementBase);
	
	friend class AvatarGen;
	friend class Avatar;
	friend class AvatarLoadMacroTexThread;

private:
	i3::vector<cThreadAvatar*>	m_ThreadList;
	UINT32						m_MaxThreadCount = 1;

	AVATAR_LOAD_INFO 	m_pLoadInfo[ MAX_AVATAR_INFO_COUNT];

	std::queue<AVATAR_LOAD_INFO*>	m_qAvatarCon;
	i3::vector<AVATAR_LOAD_INFO*>	m_AvatarLoadingList;
	i3::vector<AVATAR_LOAD_INFO*>	m_AvatarLoadedList;

	enum LOADSTATE {
		LOADSTATE_NONE,
		LOADSTATE_REQ,
		LOADSTATE_CREATING_VA,
		LOADSTATE_CREATING_IA,
		LOADSTATE_OPTIMIZING,
		LOADSTATE_CLEARING
	};

	tagLOADMACROTEXDATA		m_LoadMacroTexQueBuffer[ LOADMACROTEXQUE_COUNT];
	UINT32			m_MacrotexQueRead = 0;
	UINT32			m_MacrotexQueWrite = 0;

	AvatarLoadMacroTexThread * 		m_pMacroTexThread;			// [initialize at constructor]
	i3Mutex2		m_Mutex;

	// 팀밴드용 텍스쳐
	i3Texture *		m_pTeamBandTexture[TEAM_COUNT] = { nullptr, nullptr };
	
protected:
	void	_CreateThread( void);
	cThreadAvatar *	_FindWaitingThread( void);

	INT32	_FindEmptyInfo( INT32 idx, CHARA_PARTS_INFO * pInfo);
	void	_LoadTeamBandTexture( void);
	
public:
	cAvatarManager(void);
	virtual ~cAvatarManager(void);

	void	Create( void);

	void	OnUpdate( void);

	void	LeaveStage( void);

	void	LostDevice( void);
	void	Revive( i3RenderContext * pCtx);

	void	WaitingForLoadAvatar( void);

	INT32	LoadAvatarQue( void (*fn)(void*,INT32,AI_BIND_DATA*), GLOBAL_CHARA_INFO* pCharaInfo, INT32 idx, bool bUseAvatar);
	
	// 로딩전에 생성된 Avatar Dummy를 반환한다.
	Avatar * GetLoadAvatarQue( INT32 loadIndex, bool bUseAvatar);
	Avatar * GetLoadedAvatar();

	/** \brief Thread에서 로드된 리소스를 VRAM으로 넘긴다.
		\note Thread에서만 호출되며 해당 VA, IA를 Que해놓고 mainthread에서 처리하도록 한다. */
	void		LoadComplatedGeometry( i3VertexArray * pSrcVA, i3IndexArray * pSrcIA, INT32 primCnt, i3GeometryAttr * pGeoAttr, i3VertexArray * pPrimitiveVA);

	UINT32	CopySetGeometry( i3GeometryAttr * pGeoAttr, UINT32 primCount, I3G_PRIMITIVE primitive, UINT32 startIdx,
		i3VertexArray * pSrcVA, i3VertexArray * pDestVA, INT32 * pBoneMap, i3IndexArray * pSrcIA, i3IndexArray * pDestIA );

	bool	IsLoadGeometry( UINT32 idx);

	void	_LoadMacroTexQue( AvatarGen * pOwner, i3Texture * pSrc, i3Texture * pDest, INT32 x, INT32 y, INT32 width, INT32 height, bool bCommonRes, const AVATAR::TEXINFO * pTexInfo);
	void	LoadMacroTexQue( AvatarGen * pOwner, i3Texture * pSrc, i3Texture * pDest, INT32 x, INT32 y, INT32 width, INT32 height, bool bInThread, bool bCommonRes, const AVATAR::TEXINFO * pTexInfo);
	void	UpdateLoadMacroTex( void);
	void	FlushMacroTexQue( void);

	i3Mutex2 *	getMutex( void)				{ return &m_Mutex; }

	void	PauseThread( void);
	void	ResumeThread( void);

	static void	CopyResizingTexture( i3Texture * pDestTex, i3Texture * pSrcTex, UINT32 destLV, INT32 lvGap);

	/** \brief Src의 x,y 좌표에서 w, h크기만큼 pDest에 복사 */
	static void CopyTexture( i3Texture * pDestTex, i3Texture * pSrcTex, UINT32 destLV, INT32 srcLV, INT32 x, INT32 y, INT32 w, INT32 h);

	i3Texture *		GetTeamBandTexture( TEAM_TYPE type)		{ I3ASSERT( type < TEAM_COUNT); return m_pTeamBandTexture[ type]; }
};
