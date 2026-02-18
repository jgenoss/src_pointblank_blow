#if !defined( __AVATAR_GEN_H)
#define __AVATAR_GEN_H

#include "AvatarDefine.h"

#if defined( CLIENT_SIDE)
#define USE_EXCHANGE_HEAD
#endif

typedef UINT32 AVATAR_LOAD_STYLE;
#define AVATAR_LOAD_STYLE_MERGE			0x0001
#define AVATAR_LOAD_STYLE_1PV			0x0002

class AvatarGen;
class Avatar;

struct AVT_BUILD_CTX
{
	AvatarGen *			m_pAvatar = nullptr;
	const AVT_PARTINFO *	m_pPart = nullptr;
	const AVATAR::TEXINFO *	m_pTexInfo = nullptr;
	i3LOD *				m_pDestLOD = nullptr;
	i3Body *			m_pSrcBody = nullptr;
	i3Skeleton *		m_pFrameSkeleton = nullptr;
	INT32				m_PrimCount = 0;
	INT32				m_LOD = -1;

	INT32				_boneMap[ 128];			// [initialize at constructor]

	typedef i3::unordered_map<i3VertexArray*, i3VertexArray*> VASharedCon;
	VASharedCon			m_mapList;

	AVT_BUILD_CTX()
	{
		i3mem::Fill( _boneMap, -1, sizeof( _boneMap));
	}

	~AVT_BUILD_CTX()
	{
		VASharedCon::iterator it = m_mapList.begin();
		while( it != m_mapList.end())
		{
			I3_SAFE_RELEASE( it->second);
			it++;
		}
		m_mapList.clear();
	}
	
	void				RegistVA( i3VertexArray * pVA);
	bool				SharedVA( i3VertexArray * pSrcVA, i3VertexArray ** pDestVA);
};

class AvatarGen : public i3ElementBase
{
	I3_CLASS_DEFINE( AvatarGen, i3ElementBase);

	friend class Avatar;
#if defined( CLIENT_SIDE)
	friend class CGameCharaManager;
#endif

private:
	enum STATE {
		STATE_NONE,
		STATE_READY,			// 일반 상태
		STATE_LOADSTART,		// Thread에 로드를 명령하는 상태
		STATE_LOADING,			// 로딩중
		STATE_LOADING_SKIN,		// 로딩중 (Skin용)
	};
	
	enum TEX_TYPE {
		DIFFUSE = 0,
		NORMAL,
		SPECULAR,
		REF_MASK,
	};

	enum TEX_LOAD_LEVEL {
		TEX_LOAD_LOW = 0,
		TEX_LOAD_HIGH,
		TEX_LOAD_LEVEL_COUNT
	};

protected:
	STATE				m_LoadState = STATE_NONE;
	AVATAR_LOAD_STYLE	m_LoadStyle = 0;

	T_ItemID			m_ItemID[EQUIP::ePART_COUNT] = { 0 };
	i3::rc_string		m_strName[ EQUIP::ePART_COUNT];

	INT32				m_IO[ EQUIP::ePART_COUNT][AVT_LOD_COUNT];			// [initialize at constructor] default -1
	bool				m_bReported[EQUIP::ePART_COUNT][AVT_LOD_COUNT] = { false };
	i3SceneGraphInfo *	m_pSgInfo[EQUIP::ePART_COUNT][AVT_LOD_COUNT] = { nullptr };

	// Skin parts
	T_ItemID			m_SkinItemID[EQUIP::eSKIN_COUNT] = { 0 };
	i3::rc_string		m_strSkinName[ EQUIP::eSKIN_COUNT];
	INT32				m_IOSkin[EQUIP::eSKIN_COUNT][AVT_LOD_COUNT] = { 0 };
	i3SceneGraphInfo *	m_pSgInfoSkin[EQUIP::eSKIN_COUNT][AVT_LOD_COUNT] = { nullptr };

	// 머리부분 3가지 타입으로 로딩해야한다.
	STATE				m_LoadHeadState = STATE_NONE;
	INT32				m_HeadIO[AVATAR::HEAD_TYPE_MAX][AVT_LOD_COUNT];		// [initialize at constructor] default -1
	i3SceneGraphInfo *	m_pHeadSgInfo[AVATAR::HEAD_TYPE_MAX][AVT_LOD_COUNT] = { nullptr };
	i3LOD *				m_pHeadLOD[AVATAR::HEAD_TYPE_MAX][AVT_LOD_COUNT] = { nullptr };	// 새로 만든 머리

	TEAM_TYPE			m_Team = TEAM_RED;

	i3VertexArray *		m_pTempVA[AVT_LOD_COUNT] = { nullptr };
	INT32				m_idxVA[AVT_LOD_COUNT] = { 0 };
	i3IndexArray *		m_pTempIA[AVT_LOD_COUNT] = { nullptr };
	INT32				m_idxIA[AVT_LOD_COUNT] = { 0 };

	i3LOD *				m_pNewLOD = nullptr;

	i3Body *			m_pBody = nullptr;

	i3GeometryAttr *	m_pNewGeoAttr = nullptr;

	INT32				m_FrameLODNum = 0;
	i3Skeleton *		m_pPreFrameSkeleton = nullptr;

	bool			m_bInThread = true;
	bool			m_bInstance = true;		// 인스턴스로 생성하는 경우 (실제 게임용)
	bool			m_bInLobby = false;
	bool			m_bLoaded = false;
	bool			m_bFemale = false;

	bool				m_bAlphaTestableMesh = false;
	UINT8				m_AlphaTestFuncValue = 0;

	i3::vector<i3GeometryAttr*>		m_CreatedGeoAttrList;

	INT32				m_TextureQuality = 0;
	bool				m_bEnableTexture[4] = { true, true, true, true };

	i3Texture *			m_pTex_Diff[TEX_LOAD_LEVEL_COUNT] = { nullptr, nullptr };
	i3Texture *			m_pTex_Norm[ TEX_LOAD_LEVEL_COUNT] = { nullptr, nullptr };
	i3Texture *			m_pTex_Spec[ TEX_LOAD_LEVEL_COUNT] = { nullptr, nullptr };
	i3Texture *			m_pTex_RefMask[ TEX_LOAD_LEVEL_COUNT] = { nullptr, nullptr };
	TEX_LOAD_LEVEL		m_CurrentTexLevel = TEX_LOAD_LOW;

	struct tagSrcTextureInfo {
		i3Texture *				_pTex;
		const AVATAR::TEXINFO *	_pInfo;
		bool					_bUseMacroTexture;

		tagSrcTextureInfo( i3Texture * pTex, const AVATAR::TEXINFO * pInfo, bool bUseMacroTexture) {
			_pTex = pTex;
			I3_SAFE_ADDREF(pTex);
			_pInfo = pInfo;
			_bUseMacroTexture = bUseMacroTexture;
		}

		~tagSrcTextureInfo() {
			I3_SAFE_RELEASE( _pTex);
		}
	};

	i3::vector<tagSrcTextureInfo*>		m_DiffTexList;
	i3::vector<tagSrcTextureInfo*>		m_NormTexList;
	i3::vector<tagSrcTextureInfo*>		m_SpecTexList;
	i3::vector<tagSrcTextureInfo*>		m_RefMTexList;

	// MacroTexture의 BgLoad를 위한 Mutex
	i3Mutex2			m_Mutex;

	i3BoneRef *			m_pEyeDummyLeft = nullptr;
	i3BoneRef *			m_pEyeDummyRight = nullptr;

private:
	void				__MakeFullPath( char * pszOut, const char * pszPath, UINT32 size);
	void				__HeadIOCheck( void);

	void				__LoadPart( EQUIP::ePART part, INT32 lod, bool bDummyAvatar);

	/** \brief 파츠 로딩 검사
		\return true : 해당 파츠는 로딩된 상태 */
	bool				__LoadingParts( EQUIP::ePART part, INT32 lod);

	void				__ReplaceRenderOrder( void);

protected:
	INT32				_FindLockedVA( i3VertexArray * pVA);
	bool				_LockVA( i3VertexArray * pVA);

	static bool			_CanMergeGeometry( i3AttrSet * pAttrSet);
	static bool			_CanMergeTexture( i3AttrSet * pAttrSet, bool bInLobby, bool bSkin = false);

	void				_BuildBoneMap( AVT_BUILD_CTX * pCtx);
	void				_BuildBoneMap( i3LOD * pLOD, i3Skeleton * pFrameSkel );

	i3LOD *				BuildLOD( INT32 lod, i3Skeleton * pFrameSkel);
	i3LOD *				_FindBaseLOD( UINT32 * pMaxBoneCount);
	void				_BuildShape( void);
	void				BuildShape( void);
	bool				PrepareGeo( INT32 lod);

	// Geometry를 Merge하지 않고 바로 Rendering합니다.
	i3LOD *				_BuildLODNoMerge( INT32 lod, i3Skeleton * pFrameSkel );
	/** \brief 기존의 Geometry를 복사해서 갖는다.
		\note 기존의 geometry는 원본 Geometry이다. 이는 사용할 수 없음.
				VertexArrayMem으로 되어 있기때문이다.*/
	static void			_ReplaceNewGeometry( i3Node * pNode, AVT_BUILD_CTX * pCtx);

	static void			_ReplaceTeamTexture( i3AttrSet * pAttrSet, const AVT_BUILD_CTX * pCtx);

	/** \brief i3TextureMem을 i3TextureDX로 변환해준다.*/
	static void			_ReplaceDXTexture( i3Node * pNode);
	void				_CopyLODShapeNode( INT32 lod, EQUIP::ePART part, i3LOD * pTarget);
	void				_BuildShapeNoMerge( void);
	static I3SG_TRAVERSAL_RESULT _Rec_CopyLODProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack);

	static void			AddMeshFunc( AVT_BUILD_CTX * pCtx, i3::vector<i3Node*> &list, i3MatrixStack * pStack);
	static I3SG_TRAVERSAL_RESULT _Rec_MergeGeoProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack);
	INT32				MergeGeo( INT32 lod, EQUIP::ePART part, i3LOD * pTargetLOD);
	void				AddMesh( AVT_BUILD_CTX * pCtx, i3GeometryAttr * pGeoAttr, MATRIX * pMat);
	
	
	bool				TransTex( i3Texture * pDestTex, i3Texture * pDestTex2, i3::vector<tagSrcTextureInfo*> &list, INT32 scaleFactor);
	bool				CopyTex( i3TextureBindAttr * pAttr, i3Texture * pTargetTex, INT32 x, INT32 y, INT32 w, INT32 h, INT32 lv, bool bUseMacroTexture);
	bool				CopyTex( i3Texture * pSrcTex, i3Texture * pTargetTex, INT32 x, INT32 y, INT32 w, INT32 h, INT32 lv, bool bUseMacroTexture);

	static void			CopySurface( i3Texture * pTargetTex , i3Texture * pSrcTex, INT32 x, INT32 y, INT32 w, INT32 h, INT32 scale);

	void				_LoadParts( const char * pszPath, EQUIP::ePART part, INT32 lod, bool bDummyAvatar );
	void				_LoadHeads( const char * pszPath, AVATAR::HEAD_TYPE type, INT32 lod);

	/** \brief Macro Texture 생성 */
	void				_BuildMacroTex( bool bRebuild = false);

	/** \brief 미리 Baking된 Texture file을 읽어온다. */
	void				_PreloadMacroTex( void);

protected:
	bool				isSameBoneStruct( i3Skeleton * pSkel1, i3Skeleton * pSkel2);

	i3Skeleton *		FindFrameSkeleton(void);

	static I3SG_TRAVERSAL_RESULT	_OptimizeForSkin( i3Node * pNode, void * pUserData, i3MatrixStack * pStack);

public:
	void				setThreadMode( bool bInThread)			{ m_bInThread = bInThread; }

	i3Texture *			getDiffuseTexture( void) const			{ return m_pTex_Diff[ m_CurrentTexLevel]; }
	i3Texture *			getNormalMap( void) const				{ return m_pTex_Norm[ m_CurrentTexLevel]; }
	i3Texture *			getSpecularMap( void) const				{ return m_pTex_Spec[ m_CurrentTexLevel]; }
	i3Texture *			getReflectionMaskMap( void) const		{ return m_pTex_RefMask[ m_CurrentTexLevel]; }

	void				setInstanceMode( bool bVal)				{ m_bInstance = bVal; }

	bool				IsGenComplate( void) const				{ return m_bLoaded; }

	// Load를 시작한다.
	// Thread에 명령하여 File을 로드합니다.
	void				LoadStart( bool bDummyAvatar);

	/** \brief 로드 상태 확인 */
	LOADSTATE			getLoadState( EQUIP::ePART part, INT32 lod);

	/** \brief 모든 파츠 로드 여부
		\return true Load complate */
	bool				LoadingParts(void);
	
	/** \brief Upper Body 로드 여부
		\desc Upper Body의 Skeleton이 있어야 다른 LOD를 구성할 수 있다.
			최소한 UpperBody가 로드되면 Head 생성가능 */
	bool				isBodyLoaded( void);

	INT32				getIOHandle( EQUIP::ePART part, INT32 lod);

	void				CreateBody( void);
	void				BuildScene( bool bPreloadMacroTex = false);
	void				BuildSceneForSkin( void);
	i3Body *			getBody( void)				{ return m_pBody; }

	void				__LoadHead( INT32 lod);
	void				LoadStartHead( );
	bool				__LoadingPartsHead( AVATAR::HEAD_TYPE type, INT32 lod);
	bool				LoadingPartsHead(void);
	void				_BuildHeadScene( const AVT_PARTINFO * pPart);
	void				BuildHeadScene( void);

	void				CheckValid(void);

	void				BuildBoneMap( i3LOD * pLOD, i3Skeleton * pFrameSkel)	{ _BuildBoneMap( pLOD, pFrameSkel); }

	i3LOD *				getHeadLOD( AVATAR::HEAD_TYPE type, INT32 lod) const	{ return m_pHeadLOD[type][lod]; }

	// 장비들의 UV좌표를 Macrotexture에 맞춘다.
	void				PrepareEquipmentSG( EQUIP::ePART part);

	void				setFemale( bool bVal)			{ m_bFemale = bVal; }
	bool				isFemale( void)					{ return m_bFemale; }	

	bool				hasSkinParts( void) const		{ return m_strName[ EQUIP::ePART_SKIN].empty()==false; }

	i3BoneRef *			getEyeDummyLeft( void) const	{ return m_pEyeDummyLeft; }
	i3BoneRef *			getEyeDummyRight( void) const	{ return m_pEyeDummyRight; }

public:
	AvatarGen(void);
	~AvatarGen(void);

	void				Destroy( void);

	void				setLoadStyle( AVATAR_LOAD_STYLE style)				{ m_LoadStyle = style; }

	bool				isReadyToLoad( void)								{ return (m_LoadState==STATE_NONE); }

	T_ItemID				getItemID( EQUIP::ePART part)						{ return m_ItemID[part]; }
	i3::rc_string		getPartName( EQUIP::ePART part) const;

	void				setTextureQuality( INT32 qual);
	void				setEnableTexture( AVATAR::TEX_TYPE type, bool bVal)	{ m_bEnableTexture[ type] = bVal; }

	// 이 함수를 호출하면 다시 로딩하게 됩니다.
#if defined( CLIENT_SIDE)
	void				SetPart( T_ItemID itemID, bool bInLobby, bool bForceRebuild = false);
#endif

	void				SetPartName( EQUIP::ePART part, const i3::string& strPath, bool bForceRebuild);

	i3SceneGraphInfo *	getSgInfo( EQUIP::ePART part, INT32 lod)
	{
		I3_BOUNDCHK( part, EQUIP::ePART_COUNT);
		I3_BOUNDCHK( lod, AVT_LOD_COUNT);
		return m_pSgInfo[ part][lod];
	}

	void				setSgInfo( EQUIP::ePART part, INT32 lod, i3SceneGraphInfo * pSGInfo)
	{
		I3_BOUNDCHK( part, EQUIP::ePART_COUNT);
		I3_BOUNDCHK( lod, AVT_LOD_COUNT);
		I3_REF_CHANGE( m_pSgInfo[ part][lod], pSGInfo);
	}

	i3SceneGraphInfo * getHeadSgInfo( AVATAR::HEAD_TYPE type, INT32 lod)
	{
		I3_BOUNDCHK( type, AVATAR::HEAD_TYPE_MAX);
		I3_BOUNDCHK( lod, AVT_LOD_COUNT);
		return m_pHeadSgInfo[type][lod];
	}

	bool				isReadyScene(void);

	/** \brief 완성된 Body를 반환한다.
		\note 머리 부분을 이때 붙이기 때문에 bUseMacrotexture 여부에 따라 머리도 macrotexture를 사용하는지 확인해야한다. 	*/
	i3Body *			GetGeneratedBody( bool bUseMacrotexture);

	void				UpdateInThread( bool bDummyAvatar = false);

	void				GetResPath( EQUIP::ePART part, INT32 lod, char * pszDest, INT32 len);

	i3BoneRef*			SetDummyBody(char* szDummyName);

	void				SetTeam( TEAM_TYPE team);
	TEAM_TYPE			GetTeam( void)			{ return m_Team; }

	/** \brief SG에서 Texture를 찾아서 리스트에 보관한다.*/
	void				SearchTexture( void);

	/** \brief list에서 pSrcTex의 Index를 반환한다. */
	INT32				FindSrcTextureFromList( i3::vector<tagSrcTextureInfo*> & list, const i3Texture * pSrcTex, INT32 x, INT32 y, INT32 w, INT32 h);

	/** \brief list에서 pSrcTex의 Index를 반환한다. */
	INT32				FindSrcTextureFromList( i3::vector<tagSrcTextureInfo*> & list, const i3Texture * pSrcTex, const AVATAR::TEXINFO * pInfo);

	/** \brief MacroTexture 로딩 완료시 SrcTexture를 리스트에서 제거하고 모델링에 적용
		\param[in] pDestTex MacroTexture에 해당 하며 리스트를 찾기 위한 용도
		\param[in] pSrcTex SrcTexture
		\see BindMacroTexture()
		\note for only thread in AvatarLoadMacroTexThread */
	bool				FinishLoadMacroTexture( const i3Texture * pDestTex, const i3Texture * pSrcTex, const AVATAR::TEXINFO * pTexInfo);

	/** \brief High quality의 macroTexture로 Body에 bind 한다. */
	void				BindMacroTexture( i3Node * pRoot, TEX_LOAD_LEVEL lv, bool bReleasePrev = false);

	i3Texture *			LoadTex( const char * pszTexName);

	/** \brief 생성된 Macro Texture를 저장한다. */
	void				SaveMacroTex( const char * pszPath);

	void				LostDevice( void);

	/** \brief DummyCharacter의 리소스를 CommonRes로 설정
		\note 더미 캐릭터는 종료하기 전까지 해제될 일이 없다. */
	void				KeepDummyCharaResource( i3SceneGraphInfo * pSgInfo);

	/** \brief width, height에 따른 mipmap count를 반환
		\node 최소 32x32를 기준으로 계산된다*/
	INT32				GetMinMapCount( INT32 w, INT32 h);
};

void RecMergeGeo( i3Node * pRoot, I3_SG_TRAVERSE_PROC pUserProc, void * pUserData, i3MatrixStack * pStack);

#endif
