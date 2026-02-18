#if !defined( __I3_RESOURCE_MANAGER_H)
#define __I3_RESOURCE_MANAGER_H

class i3Sound;
class i3ResourceManager;
class i3Framework;
class i3Chara;
class i3Object;
class i3World;
class i3GamePath;

#include "i3GameNode.h"
#include "i3GameRes.h"

typedef i3Sound* (*i3LoadSoundCallBackFunc)( i3ResourceManager *pResMgr, const char *szPath , UINT32	nState);

typedef void	(I3_LOAD_COMPLETION_PROC)( );
typedef bool	(*cbPreLoadFun)(char *, bool);

class I3_EXPORT_FRAMEWORK i3QueueFileInfo : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3QueueFileInfo, i3ElementBase);
public:
	enum TYPE
	{
		TYPE_SCENE = 0,
		TYPE_CHARA,
		TYPE_WORLD,
		TYPE_ANIMATION,
		TYPE_TEXTURE,
		TYPE_SOUND,
		TYPE_BINARY,
		TYPE_ANIMATIONPACK,
		TYPE_OBJECT,
		TYPE_UI,

		TYPE_LIGHT,
		TYPE_RESPAWN,
		TYPE_PATH,
		TYPE_EVENT,
		
	};

protected:
	i3::rc_string m_Path;
	TYPE		m_Type = TYPE_SCENE;
	void *		m_pUserData = nullptr;
	INT32		m_UserParam1 = 0;
	INT32		m_UserParam2 = 0;

public:
	const char *		GetPath(void) const		{ return m_Path.c_str(); }
	void		SetPath( const char * pszPath)	{	m_Path = pszPath; }		

	TYPE		GetType(void)				{ return m_Type; }
	void		SetType( TYPE type)			{ m_Type = type; }

	void *		GetUserPointer(void)		{ return m_pUserData; }
	void		SetUserPointer( void * p)	{ m_pUserData = p; }

	INT32		GetUserParam1(void)			{ return m_UserParam1; }
	void		SetUserParam1( INT32 param)	{ m_UserParam1 = param; }

	INT32		GetUserParam2(void)			{ return m_UserParam2; }
	void		SetUserParam2( INT32 param)	{ m_UserParam2 = param; }
};


bool		operator<(const i3GameRes& a1, const i3GameRes& a2);
template<> struct i3::less<i3GameRes*> : i3::less_ptr<i3GameRes*> {};


class I3_EXPORT_FRAMEWORK i3ResourceManager : public i3GameNode
{
	I3_EXPORT_CLASS_DEFINE( i3ResourceManager, i3GameNode);
protected:
	i3Framework *		m_pFramework = nullptr;
	bool				m_bCommonRes = false;

	i3::vector<i3QueueFileInfo*> m_QueueFileList;
	
	INT32				m_MaxQueueFileCount = 0;
	// 30 FPS 기준
	REAL32			m_timeLoadInterval = 0.033f;
	i3Timer				m_timer;

	i3SceneGraphInfo *	_LoadI3S( const char * pszPath, bool bInThread, bool bBGLoadEnable, bool bClone, UINT32 * pSize );
	i3Animation *		_LoadI3A( const char * pszPath, UINT32 * pSize);
	i3Texture *			_LoadImage( const char * pszImage);

	i3::rc_string		m_strLangPostfix;

	i3::vector_multiset<i3GameRes*>		m_ResList;

	void			ReportErr( const char *szFmt, ... );
	virtual void	ReportErrMsg( const char* szMsg );

	// 모델링 파일 읽기
	i3SceneGraphInfo *	LoadModel( const char * szPath, bool bInThread = false, bool bBGLoadEnable = false, const char* szPostFix = nullptr, UINT32 * pSize = nullptr );

private:

	i3World *		_LoadWorld( const char * pszPath);

	// 사운드 로딩
	i3Sound*		_LoadSound( const char *szPath, UINT32 * pSize = nullptr );

	void			__AddLoadRequest( char * pszPath, void * pUserData, INT32 param1, INT32 param2);
	void			__AddLoadRequest( char * pszPath, i3QueueFileInfo::TYPE type, void * pUserData, INT32 param1, INT32 param2);

	i3GameRes *		__LoadResource( const char * pszPath, i3GameRes * pSrcRes = nullptr, INT32 layer = 0, bool bCommonRes = false, bool bInThread = false, bool bBGLoadEnable = false);

	I3_GAMERES_TYPE	__ConvertExt2ResType( const char * pszExt);

public:
	i3ResourceManager(void);
	virtual ~i3ResourceManager(void);

	void			BindFramework( i3Framework * pFramework)			{ m_pFramework = pFramework; }

	bool			getCommonResourceState(void)						{ return m_bCommonRes; }
	void			setCommonResourceState( bool bFlag)
	{
		i3NamedResourceManager::SetCommonResourceState( bFlag);
		m_bCommonRes = bFlag;
	}

	// 언어 설정에 영향을 받는 파일들의 파일명을 정하는데 사용하는 접미사 설정
	void			SetLanguagePostfix( const char *szPostfix );

	// 현재 지정된 언어에 의한 접미사를 사용한 파일명 얻기
	void			GetFileNameByLanguage( char * newPath , const char * szPath, UINT32 nBufSize );

	void			RemoveAllLoadRequest(void);

	i3Chara *		LoadCharactor( const char * szPath, i3Chara * pChar = nullptr,
		bool RotateToMax = false, const char* szPostFix = nullptr, bool bClone = false,
		bool bCommonRes = false, bool bInThread = false, UINT32 * pSize = nullptr);
	
	// i3World 파일 읽기
	i3World	*		LoadWorld( const char * szPath, i3World * pWorld = nullptr, bool RotateToMax = 0, const char* szPostFix = nullptr, bool bClone = false);

	// 애니메이션 파일 읽기
	i3Animation*	LoadAnimation( const char *szPath, UINT32 * pSize = nullptr );
	void			LoadAnimationPack( const char * pszPath);

	// 텍스쳐 로딩
	i3Texture*		LoadTexture( const char *szPath, bool bRef = false, UINT32 * pSize = nullptr );

	// Path
	i3GamePath *	LoadPath( const char * pszPath);

	// Loading 한 I3P Pack File의 정보를 완전히 Release한다.
	// 새로운 Scene으로 전환할 때에는 항상 호출되어야 한다.
	void			Reset(void);

	// 기타 일반 파일을 읽기 위한 스트림 얻기
	i3Stream *		OpenStream( const char * pszPath);

	// 파일이 존재하는 지 여부 판단
	bool			IsExist( const char *szPath );

	// 폰트 로딩
	i3Font*			LoadFont( const char *szPath, bool bUseLangPostfix = false);

	// 무자열 테이블 로딩
	i3StringTable*	LoadStringTable( const char *szPath, bool bUseLangPostfix = false);

	REAL32		GetLoadInterval( void)					{ return m_timeLoadInterval; }
	void			SetLoadInterval( REAL32 tm)			{ m_timeLoadInterval = tm; }

	////////////////////////
	// Resource
	void			AddResource( i3GameRes * pRes);
	void			RemoveAllResource( bool bReleaseCommon = false);
	void			RemoveResource( i3GameRes * pRes);
	i3GameRes *		FindResourceByPath( const char * pszName);
	i3GameRes *		FindResourceByHash( UINT128 * hash);

	// 매우 느립니다. 이것보다 Path에 의한 search를 사용하길 권장
	i3GameRes *		FindResourceByOnlyName( const char * pszName);
	i3GameRes *		FindResourceByOnlyNameAndClassMeta(const char* pszName, i3::class_meta* pMeta);

	i3GameRes *		FindResourceByType( INT32 type);
	INT32			GetResourceCount(void)								{ return (INT32) m_ResList.size(); }
	i3GameRes *		GetResource( INT32 idx)								{ return m_ResList.get_vector()[idx]; }
	bool			LoadRSC( const char * pszPath, bool bCommonRes = false);
	bool			LoadRSG(const char * pszPath, bool bCommonRes = false);
	
	static bool		DumpRSCInfo(const char* rscPath, const char* srcMediaPath, const char* savePath, 
								bool genResInfo=true, bool copyRefRes=false);
	static bool		MakePathDirectory(const char* path);

	i3GameRes *		LoadResource( const char * pszPath, INT32 layer, bool bCommonRes = false, bool bInThread = false, bool bBGLoadEnable = false);
	i3ResourceObject *	FindResourceFromManager( const char * pszName, i3ClassMeta * pMeta);

	void			DumpAllResources(void);

	// i3P Pack file의 Loading의 진행율을 Percent 값으로 반환한다.
	// 완전히 종료된 경우에는 100을 반환한다.
	INT32	GetLoadingProgress(void);

	virtual void	OnUpdate( REAL32 timeDelta) override;

	////
	void				AddChrThreadLoad(char * pszPath, void * pUserData, INT32 param1, INT32 param2);
	//	List				m_ChraQueFileList;			// 쓰이지 않아서 주석..

	i3SceneIOManager *		m_pIOManager = nullptr;

	void		EnableOccMeshLoad(bool bEnable) {	m_bOccMeshLoadEnable = bEnable;	}
	bool		IsOccMeshLoadEnable() const { return m_bOccMeshLoadEnable;  }
private:
	struct tagBACKGROUNDRES_INFO
	{
		i3QueueFileInfo::TYPE _type = i3QueueFileInfo::TYPE::TYPE_SCENE;
		i3GameRes *	_pRes = nullptr;
		INT32	_param1 = 0;
		INT32	_param2 = 0;
	};
	// Background load를 하는 리소스
	// Stage가 끝나거나 일정량 메모리를 점유하면 리소스를 삭제하자?
	i3::vector_map< std::string, tagBACKGROUNDRES_INFO*> m_BackgroundFileList;

	bool m_bOccMeshLoadEnable = true;

	void			__AddBackgroundFile( const char * pszPath, i3QueueFileInfo::TYPE type, i3GameRes * pRes, INT32 param1, INT32 param2);
	tagBACKGROUNDRES_INFO *	__FindBackgroundRes( const char * pszName);

public: 
	const char *	FindFullPathByOnlyName( const char * pszName);
	i3GameRes *		FindBackgroundRes( const char * pszName);
	/** \brief Load Resource
		\node Only for thread
		\param[in] pszName File Name
		\param[in] bCommonLoad CommonRes 여부
		\param[in] bInThread Thread 여부 */
	i3GameRes *		LoadBackgroundRes( const char * pszName, bool bCommonLoad = false, bool bInThread = false);
	bool			isCommonLoadRes( char* szFilePath, bool isCommon);

private:
	cbPreLoadFun	cbPreLoad = nullptr;

public:
	void			setCbPreLoad(cbPreLoadFun fn) {cbPreLoad = fn;};
};

#endif
