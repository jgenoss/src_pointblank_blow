#if !defined( __I3_RESOURCE_MANAGER_H)
#define __I3_RESOURCE_MANAGER_H

class i3Sound;
class i3ResourceManager;
class i3Framework;
class i3Chara;
class i3World;

#include "i3GameNode.h"
#include "i3GameRes.h"

typedef i3Sound* (*i3LoadSoundCallBackFunc)( i3ResourceManager *pResMgr, const char *szPath , UINT32	nState);

typedef void	(I3_LOAD_COMPLETION_PROC)( );

class I3_EXPORT_FRAMEWORK i3QueueFileInfo : public i3ElementBase
{
	I3_CLASS_DEFINE( i3QueueFileInfo);
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
		TYPE_CLONE,
		TYPE_OBJECT,
		TYPE_LIGHT,
		TYPE_RESPAWN,
		TYPE_PATH,
		TYPE_EVENT,
	};

protected:
	char		m_szPath[128];
	TYPE		m_Type;
	void *		m_pUserData;
	INT32		m_UserParam1;
	INT32		m_UserParam2;

public:
	i3QueueFileInfo(void);
	virtual ~i3QueueFileInfo(void);

	char *		GetPath(void)				{ return m_szPath; }
	void		SetPath( char * pszPath)	{ i3String::Copy( m_szPath, pszPath); }

	TYPE		GetType(void)				{ return m_Type; }
	void		SetType( TYPE type)			{ m_Type = type; }

	void *		GetUserPointer(void)		{ return m_pUserData; }
	void		SetUserPointer( void * p)	{ m_pUserData = p; }

	INT32		GetUserParam1(void)			{ return m_UserParam1; }
	void		SetUserParam1( INT32 param)	{ m_UserParam1 = param; }

	INT32		GetUserParam2(void)			{ return m_UserParam2; }
	void		SetUserParam2( INT32 param)	{ m_UserParam2 = param; }
};

class I3_EXPORT_FRAMEWORK i3ResourceManager : public i3GameNode
{
	I3_CLASS_DEFINE( i3ResourceManager);
protected:
	i3Framework *		m_pFramework;
	bool				m_bCommonRes;

	i3List				m_QueueFileList;
	i3FixedMemoryPool *	m_pQueueMemPool;
	INT32				m_MaxQueueFileCount;
	RT_REAL32			m_timeLoadInterval;
	i3Timer				m_timer;

	i3SceneGraphInfo *	_LoadI3S( const char * pszPath, BOOL bClone );
	i3Animation *		_LoadI3A( const char * pszPath);
	i3Texture *			_LoadImage( const char * pszImage);

	i3LoadSoundCallBackFunc		m_pLoadSoundFunc;

	char				m_szLangPostfix[64];

	i3BinList			m_ResList;

	void			ReportErr( const char *szFmt, ... );
	virtual void	ReportErrMsg( const char* szMsg );

public:
	i3ResourceManager(void);
	virtual ~i3ResourceManager(void);

	void	BindFramework( i3Framework * pFramework)			{ m_pFramework = pFramework; }

	bool	getCommonResourceState(void)						{ return m_bCommonRes; }
	void	setCommonResourceState( bool bFlag)					{ m_bCommonRes = bFlag; }

	// 언어 설정에 영향을 받는 파일들의 파일명을 정하는데 사용하는 접미사 설정
	void	SetLanguagePostfix( const char *szPostfix );

	// 현재 지정된 언어에 의한 접미사를 사용한 파일명 얻기
	void	GetFileNameByLanguage( char * newPath , const char * szPath );

	// Loading 한 I3P Pack File의 정보를 완전히 Release한다.
	// 새로운 Scene으로 전환할 때에는 항상 호출되어야 한다.
	void	Reset(void);

	// 모델링 파일 읽기
	i3SceneGraphInfo *	LoadModel( 
		const char * szPath,
		const char* szPostFix = NULL
		); 

	// i3Scene 파일 읽기
	i3SceneGraphInfo*	LoadScene( const char * szPath, i3NamedResourceManager *pNamedResMgr = NULL, BOOL bClone = FALSE );

	// i3Chara 파일 읽기 
	i3Chara *		LoadCharactor( const char * szPath, i3Chara * pChar = NULL,	BOOL RotateToMax = 0, const char* szPostFix = NULL, BOOL bClone = FALSE);
	i3Chara *		_Loadi3Chara( const char * pszPath);
	//

	// i3World 파일 읽기
	i3World	*		LoadWorld( const char * szPath, i3World * pWorld = NULL, BOOL RotateToMax = 0, const char* szPostFix = NULL, BOOL bClone = FALSE);
	i3World *		_LoadWorld( const char * pszPath);

	// 애니메이션 파일 읽기
	i3Animation*	LoadAnimation( const char *szPath );
	void			LoadAnimationPack( const char * pszPath);

	// 텍스쳐 로딩
	i3Texture*		LoadTexture( const char *szPath, BOOL bRef = FALSE );

	// 기타 일반 파일을 읽기 위한 스트림 얻기
	i3Stream *		OpenStream( const char * pszPath);

	// 파일이 존재하는 지 여부 판단
	BOOL			IsExist( const char *szPath );

	// 사운드 컨텍스트 설정
	void			SetLoadSoundCallBackFunc( i3LoadSoundCallBackFunc pFunc ){ m_pLoadSoundFunc = pFunc; }

	// 사운드 로딩
	i3Sound*		LoadSound( const char *szPath , UINT32	nState = 0x00010000 );

	// 폰트 로딩
	i3Font*			LoadFont( const char *szPath, BOOL bUseLangPostfix = FALSE );

	// 무자열 테이블 로딩
	i3StringTable*	LoadStringTable( const char *szPath, BOOL bUseLangPostfix = FALSE );

	void			AddLoadRequest( char * pszPath, void * pUserData, INT32 param1, INT32 param2);
	void			AddLoadRequest( char * pszPath, i3QueueFileInfo::TYPE type, void * pUserData, INT32 param1, INT32 param2);
	void			RemoveAllLoadRequest(void);

	RT_REAL32		GetLoadInterval( void)					{ return m_timeLoadInterval; }
	void			SetLoadInterval( RT_REAL32 tm)			{ m_timeLoadInterval = tm; }

	////////////////////////
	// Resource
	void			AddResource( i3GameRes * pRes);
	void			RemoveAllResource( BOOL bReleaseCommon = FALSE);
	i3GameRes *		FindResourceByPath( const char * pszName);
	i3GameRes *		FindResourceByHash( UINT32 * hash);
	i3GameRes *		FindResourceByOnlyName( const char * pszName);
	i3GameRes *		FindResourceByType( INT32 type);
	INT32			GetResourceCount(void)								{ return m_ResList.GetCount(); }
	i3GameRes *		GetResource( INT32 idx)								{ return (i3GameRes *) m_ResList.Items[idx]; }
	BOOL			LoadRSC( const char * pszPath, BOOL bCommonRes = FALSE);
	i3GameRes *		LoadResource( const char * pszPath, INT32 layer, BOOL bCommonRes = FALSE);

	// i3P Pack file의 Loading의 진행율을 Percent 값으로 반환한다.
	// 완전히 종료된 경우에는 100을 반환한다.
	INT32	GetLoadingProgress(void);

	virtual void	OnUpdate( RT_REAL32 timeDelta);
};

#endif
