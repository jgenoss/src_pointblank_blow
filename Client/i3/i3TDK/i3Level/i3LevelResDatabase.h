#if !defined( __I3_LV_RES_DATABASE_H)
#define __I3_LV_RES_DATABASE_H

#include "i3LevelRes.h"
#include "i3LevelResScanThread.h"
#include "i3LevelResRef.h"

#include "i3Base/itl/set.h"

enum I3LV_RESDB_ROOT_TYPE
{
	I3LV_RESDB_TYPE_PRIMITIVE = 0,
	I3LV_RESDB_TYPE_CHARA,
	I3LV_RESDB_TYPE_OBJECT,
	I3LV_RESDB_TYPE_WORLD,
	I3LV_RESDB_TYPE_LIGHT,
	I3LV_RESDB_TYPE_CAMERA,
	I3LV_RESDB_TYPE_SCENEGRAPH,
	I3LV_RESDB_TYPE_EFFECT,
	I3LV_RESDB_TYPE_ANIMATION,
	I3LV_RESDB_TYPE_SOUND,
	I3LV_RESDB_TYPE_TEXTURE,
	I3LV_RESDB_TYPE_EVENT,
	I3LV_RESDB_TYPE_PATH,

	I3LV_RESDB_TYPE_COUNT
};

struct __FIND_RESREF_INFO
{
	i3LevelRes *	m_pRes = nullptr;
	i3LevelResRef *	m_pRef = nullptr;
} ;

class i3LevelScene;

class I3_EXPORT_TDK i3LevelResDatabase : public i3PersistantElement
{
	I3_EXPORT_CLASS_DEFINE( i3LevelResDatabase, i3PersistantElement);
protected:
	i3Mutex					m_Mutex;
	i3LevelResRef *			m_pRoot = nullptr;
	i3LevelResRef *			m_pTypeRoot[I3LV_RESDB_TYPE_COUNT] = { nullptr };

	i3::vector_set<i3LevelRes*> m_ResList;
	i3::set< i3::string >		m_RemoveResPathList;

	i3LevelResScanThread *	m_pThread = nullptr;

	bool					m_bModified = false;
	bool					m_bUpdateEnable = true;

protected:
	void			_BuildDefTree(void);
	i3LevelResRef *	_PreparePath( i3LevelResRef * pRoot, char * pszPath);
	void			_RefreshResList(void);

	static BOOL		_RefreshResListProc( INT32 depth, i3LevelResRef * pNode, void * pUserData);
	static BOOL		_FindRefResProc( INT32 depth, i3LevelResRef * pNode, __FIND_RESREF_INFO * pInfo);
	static BOOL		_FlushResProc( INT32 depth, i3LevelResRef * pNode, void * pData);

public:
	i3LevelResDatabase(void);
	virtual ~i3LevelResDatabase(void);

	void			Reset(void);
	void			BindScene( i3LevelScene * pScene);

	i3LevelResRef * getResRoot(void)	{	return m_pRoot;}
	i3LevelResRef * getResTypeRoot(INT32 idx)	{	return m_pTypeRoot[idx];}
	void			RemoveAllRes(void);
	void			RemoveRes( i3LevelRes * pRes);
	void			AddRes( i3LevelRes * pRes);
	void			AddRes( i3LevelResRef *	pParent, i3LevelResRef * pNode);
	i3LevelRes *	AddRes( const char * pszPath, WIN32_FIND_DATAA * pInfo = nullptr);
	i3LevelRes *	FindResByPathHash( UINT32 * hashcode);
	i3LevelRes *	FindResByOldPathHash( UINT32 code);
	i3LevelRes *	FindResByPath( const char * pszPath);
	void			FindResByType( i3ClassMeta * pMeta, i3::vector<i3LevelRes*>& List);
	i3LevelResRef *	GetResRef( i3LevelRes * pRes);
	void			GetHierarchyPath( i3LevelRes * pRes, i3::vector<i3LevelResRef*>& List);
	void			GetUniqueResName( I3LV_RESDB_ROOT_TYPE type, const char * pszPath, const char * pszPrefix, char * pszName, UINT16 nBufSize);

	i3LevelRes *	GetRes( const char * pszPath);

	INT32			getResCount(void)	{	return (INT32)m_ResList.size(); }
	i3LevelRes *	getRes( INT32 idx)	{	return m_ResList.get_vector()[idx]; }

	void			setUpdateEnableState( bool bState)				{ m_bUpdateEnable = bState; }

	void			Validate( i3LevelScene * pScene, i3LevelRes * pRes);
	void			StartScan( HWND hwnd, const char * pszPath, const i3::set< i3::string >& ignoreFolder, I3LV_RES_SCAN_PROC pProc, bool bForce = false);
	void			StopScan(void);

	void			FlushDB(void);

	void			Dump(void);
	void			Check(void);

	void			CheckDuplicateID(void);

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
