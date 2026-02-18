#if !defined( __I3_Level_RESOURCE_H)
#define __I3_Level_RESOURCE_H

#include "i3LevelDefine.h"
#include "i3LevelPropertyContainer.h"
#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/string/algorithm/to_upper.h"

#define		I3LV_TYPE_MASK_VOLATILE				0x80000000
#define		I3LV_TYPE_MASK_CONCRETIVE			0x40000000

#define	I3LV_TYPE_PRIMITIVE						(0x00000001 | I3LV_TYPE_MASK_VOLATILE | I3LV_TYPE_MASK_CONCRETIVE)
#define	I3LV_TYPE_CHARA							(0x00000003 | I3LV_TYPE_MASK_CONCRETIVE)
#define	I3LV_TYPE_WORLD							(0x00000005 | I3LV_TYPE_MASK_CONCRETIVE)
#define	I3LV_TYPE_OBJECT						(0x00000007 | I3LV_TYPE_MASK_CONCRETIVE)
#define	I3LV_TYPE_EFFECT						(0x00000009 | I3LV_TYPE_MASK_CONCRETIVE)
#define	I3LV_TYPE_LIGHT							(0x0000000B | I3LV_TYPE_MASK_VOLATILE | I3LV_TYPE_MASK_CONCRETIVE)
#define	I3LV_TYPE_CAMERA						(0x0000000D | I3LV_TYPE_MASK_VOLATILE | I3LV_TYPE_MASK_CONCRETIVE)
#define	I3LV_TYPE_TEXTURE						(0x0000000F)
#define	I3LV_TYPE_ANIMATION						(0x00000011)
#define	I3LV_TYPE_SOUND							(0x00000013)
#define	I3LV_TYPE_SCENEGRAPH					(0x00000015 | I3LV_TYPE_MASK_CONCRETIVE)
#define I3LV_TYPE_EVENT							(0x00000017 | I3LV_TYPE_MASK_VOLATILE | I3LV_TYPE_MASK_CONCRETIVE)
#define I3LV_TYPE_PATH							(0x00000019 | I3LV_TYPE_MASK_CONCRETIVE)

#define	I3LV_RES_STATE_LOADED				0x00000001
#define	I3LV_RES_STATE_BRIEF_LOADED			0x00000002
#define	I3LV_RES_STATE_MODIFIED				0x00000004

class i3LevelElement3D;
class i3LevelScene;
class i3LevelRes;

bool		operator<(const i3LevelRes& a1, const i3LevelRes& a2);
template<> struct i3::less<i3LevelRes*> : i3::less_ptr<i3LevelRes*> {};

class I3_EXPORT_TDK i3LevelRes : public i3LevelPropertyContainer
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3LevelRes, i3LevelPropertyContainer);
protected:
	char				m_szPath[MAX_PATH] = { 0 };
	UINT32				m_ResType = 0;
	UINT32				m_ResState = 0;
	UINT32				m_HashCode[4] = { 0xFFFFFFFF, };

	INT64				m_LastUpdateFileTime;
	INT64				m_LastExportFileTime;

	i3::vector<i3LevelRes*>	m_RefResList;			// 외부에서 추가로 읽어들인 Res.들에 대한 List

public:
	const char *	getPath(void)				{ return m_szPath; }
	void			setPath( const char* pszPath)	
	{ 
		i3::safe_string_copy( m_szPath, pszPath, MAX_PATH);
		i3::make_unix_path( m_szPath);

		char szTemp[ MAX_PATH];
		i3::safe_string_copy( szTemp, m_szPath, MAX_PATH);
		i3::to_upper( szTemp);

		//m_HashCode = CRC32( 0xFFFFFFFF, (UINT8 *) szTemp, strlen( szTemp));

		i3MD5::Gen( szTemp, i3::generic_string_size(szTemp) * sizeof(char), (char *) m_HashCode);
	}

	void			getFullPath( char * pszPath);

	const UINT32 *		getHashCode(void)	const		{ return m_HashCode; }

	UINT32			getResType(void)				{ return m_ResType; }
	bool			isVolatileRes(void)				{ return (m_ResType & I3LV_TYPE_MASK_VOLATILE) != 0; }		// 파일로 존재하지 않는 놈?
	bool			isConcretiveRes(void)			{ return (m_ResType & I3LV_TYPE_MASK_CONCRETIVE) != 0; }	// Level에 배치 가능한 놈?

	UINT32			getResState(void)				{ return m_ResState; }
	void			setResState( UINT32 state)		{ m_ResState = state; }
	void			addResState( UINT32 state)		{ m_ResState |= state; }
	void			removeResState( UINT32 state)	{ m_ResState &= ~state; }
	bool			isResState( UINT32 mask)		{ return (m_ResState & mask) == mask; }
	bool			isBriefLoaded(void)				{ return isResState( I3LV_RES_STATE_BRIEF_LOADED); }
	bool			isLoaded(void)					{ return isResState( I3LV_RES_STATE_LOADED); }
	bool			isModified(void)				{ return isResState( I3LV_RES_STATE_MODIFIED); }

	INT64			getLastUpdateFileTime(void)		{ return m_LastUpdateFileTime; }
	void			setLastUpdateFileTime( FILETIME * pTime)
	{
		m_LastUpdateFileTime = ((INT64)pTime->dwHighDateTime << 32) | pTime->dwLowDateTime;
	}

	INT64				getLastExportFileTime(void)		{ return m_LastExportFileTime; }
	void				setLastExportFileTime( FILETIME * pTime)
	{
		m_LastExportFileTime = ((INT64) pTime->dwHighDateTime << 32) | pTime->dwLowDateTime;
	}

	bool				IsLastestVersion(void);

	INT32				getDependantResCount(void)		{ return (INT32)m_RefResList.size(); }
	i3LevelRes *		getDependantRes( INT32 idx)		{ return m_RefResList[idx]; }
	void				RemoveDependantResByName( const char * pszPath);

	virtual void		OnChangeScene( i3LevelScene * pScene);
	virtual i3LevelElement3D *		CreateInstance( i3LevelScene * pScene);

	// 간략한 정보를 Validate 시키기 위해 Scene에서 호출된다.
	// 실제 Resource를 위한 메모리를 할당하는 등의 처리는 하지 않는다.
	virtual bool		OnValidateBrief( bool bForce) = 0;

	// 실제 Resource를 메모리로 읽어 들이는 단계.
	virtual void		BeginValidate(void);
	virtual bool		OnValidate( i3LevelScene * pScene, bool bForce) = 0;
	virtual void		EndValidate(void);

	virtual bool		OnSerialize(void);

	virtual void		OnQueryExportPath( i3LevelScene * pScene, i3LevelElement3D * pObj, char * pszPath, UINT16 nBufSize);
	virtual void		OnQueryDependancy( i3::vector<i3LevelRes*>& List);
	virtual bool		OnExport( i3LevelScene * pScene, const char * pszPath);
	virtual i3GameRes *	CreateGameRes(void);

	virtual i3Node *	GetElementAttachNode( i3LevelScene * pScene);

	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;

	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;

protected:
	static void			_ExternRefCallback( i3PersistantElement * pElm, const char * pszPath, void * pUserData);
};

#endif