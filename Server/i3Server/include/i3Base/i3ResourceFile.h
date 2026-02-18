#if !defined( __I3_RESOURCE_FILE_H)
#define __I3_RESOURCE_FILE_H

//#include "i3CommonType.h"
#include "i3ElementBase.h"
#include "i3PersistantElement.h"
#include "i3List.h"
#include "i3Stream.h"
#include "i3NamedResourceManager.h"

#include "itl/vector.h"

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

#define		I3_RSCFILE_STYLE_BIGENDIAN				0x00000001

#define		I3_RSCFILE_HEAD_10		"I3RF"
#define		I3_RSCFILE_HEAD_20		"I3R2"

typedef struct PACKED i3ResourceFileHeader1
{
	UINT8		m_MajorVersion;
	UINT8		m_MinorVersion;

	UINT16		m_StringTableCount;
	UINT32		m_StringTableOffset;
	UINT32		m_StringTableSize;

	UINT16		m_ObjectCount;
	UINT32		m_ObjectDirOffset;
	UINT32		m_ObjectDirSize;
} I3_RESOURCE_FILE_HEADER1;

typedef struct PACKED i3ResourceFileHeader2
{
	UINT32		m_Style;
	UINT16		m_MajorVersion;
	UINT16		m_MinorVersion;

	UINT32		m_StringTableCount;
	UINT64		m_StringTableOffset;
	UINT64		m_StringTableSize;

	UINT32		m_ObjectCount;
	UINT64		m_ObjectDirOffset;
	UINT64		m_ObjectDirSize;

	OBJREF64	m_pKeyObject;

	UINT32		m_ChecksumCode[4];
	UINT32		pad[28];
} I3_RESOURCE_FILE_HEADER2;

typedef struct PACKED i3ResourceFileObjectInfo1
{
	UINT16		m_ClassNameIndex;
	UINT32		m_InstanceID;
	UINT32		m_Offset;
	UINT32		m_Size;
} I3_RESOURCE_FILE_OBJECT_INFO1;

typedef struct PACKED i3ResourceFileObjectInfo2
{
	UINT32		m_ClassNameIndex;
	UINT64		m_InstanceID;
	UINT64		m_Offset;
	UINT64		m_Size;
} I3_RESOURCE_FILE_OBJECT_INFO2;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

typedef struct i3ResourceManagerTableInfo
{
	i3ClassMeta *				m_pMeta;
	i3NamedResourceManager *	m_pManager;
} I3_RESOURCE_MANAGER_INFO;

typedef struct _tagObjectIdTableInfo
{
	OBJREF			m_ID;
	INT32			m_Ordinal;
} I3_RES_OBJECT_ID_INFO;

typedef void (*PROGRESSPROC)( void * pUserData, UINT32 curPos, UINT32 maxPos);
typedef void (*I3_EXTERNALREF_REPORT)( i3PersistantElement * pElm, const char * pszPath, void * pUserData);
typedef bool (*I3_EXTERNALREF_RESOLVE)( const char * pszPath, char * pszNewPath, INT32 lenBuff, const char * pszWorkPath, bool bSearch);

class I3_EXPORT_BASE i3ResourceFile : public i3ElementBase
{
	I3_CLASS_DEFINE( i3ResourceFile);

	enum RESOURCE_HEADER_PAD
	{
		LOW_DATA_TIME = 0,
		HIGH_DATA_TIME
	};
protected:
	i3StringList					m_StrTable;
	i3Stream *						m_pStream;
	i3::vector<i3PersistantElement*> m_ObjList;
	I3_RESOURCE_FILE_OBJECT_INFO2 *	m_pObjInfo;
	I3_RESOURCE_FILE_HEADER2		m_Header;

	INT32							m_IDCount;
	I3_RES_OBJECT_ID_INFO *			m_pIDTable;

	i3PersistantElement *			m_pKeyObject;

	PROGRESSPROC					m_pUserProgressProc;
	void *							m_pUserData;

	bool							m_bOverwriteExternalRes;
	char							m_szFileName[ MAX_PATH];

	bool							m_bBackgroundLoad;

protected:
	UINT32				GetAlignedPos( UINT32 pos);
	UINT32				Align( i3Stream * pStream);

	i3PersistantElement *	_ResolveExtern( INT32 idx);
	UINT32					_SaveExtern( INT32 i, i3ResourceObject * pObj);

	void					_BuildIDTable( INT32 count);
	void					_Rec_QuickSort( I3_RES_OBJECT_ID_INFO * pTable, INT32 idxMin, INT32 idxMax);

	bool					_isExtern( OBJREF ref);
	INT32					_getRefStringIndex( OBJREF ref);
	INT32					_getRefOrdinal( OBJREF ref);

public:
	i3ResourceFile(void);
	virtual ~i3ResourceFile(void);

	const char *			getFileName(void)					{ return m_szFileName; }

	i3StringList *			GetStringTable(void)				{ return & m_StrTable; }
	i3Stream *				GetStream(void)						{ return m_pStream; }
	void					SetStream( i3Stream * pStream)		{ m_pStream = pStream; }

	I3_RESOURCE_FILE_HEADER2 *		getHeader(void)				{ return &m_Header; }
	I3_RESOURCE_FILE_OBJECT_INFO2 *	getObjectInfo( INT32 i)		{ return &m_pObjInfo[ i]; }

	INT32					GetObjectCount(void)			{ return INT32(m_ObjList.size()); }
	i3PersistantElement *	GetObject( INT32 idx)			{ return m_ObjList[idx]; }
	void					SetObjects( i3::vector<i3PersistantElement*>& ObjList);
	void					AddObject( i3PersistantElement * pObj)		{ m_ObjList.push_back( pObj); }
	INT32					FindObjectInfoByID( OBJREF ID);
	i3PersistantElement *	FindObjectByID( OBJREF InstanceID);
	i3PersistantElement *	FindObjectByMeta( i3ClassMeta * pMeta);
	OBJREF					GetObjectPersistID( i3PersistantElement * pObj);

	i3PersistantElement *	getKeyObject(void)					{ return m_pKeyObject; }
	void					setKeyObject( i3PersistantElement * pObj)
	{
		m_pKeyObject = pObj;
	}

	UINT16					GetVersionMajor(void)				{ return m_Header.m_MajorVersion; }
	UINT16					GetVersionMinor(void)				{ return m_Header.m_MinorVersion; }

	bool					GetOverwriteExternalResStatus(void)	{	return m_bOverwriteExternalRes ; }
	void					SetOverwriteExternalResStatus( bool bFlag)
	{
		m_bOverwriteExternalRes = bFlag;
	}

	void					SetUserProgressProc( PROGRESSPROC pProc, void * pUserData)
	{ 
		m_pUserProgressProc = pProc;
		m_pUserData = pUserData;
	}

	UINT32					Read( void * pBuffer, UINT32 Length)
	{
		return m_pStream->Read( pBuffer, Length);
	}

	UINT32					Write( const void * pBuffer, UINT32 Length)
	{
		return m_pStream->Write( pBuffer, Length);
	}

	virtual UINT32				Load( i3Stream * pStream);
	virtual UINT32				Save( i3Stream * pStream);

	virtual UINT32				Load( const char * pszPath);
	virtual UINT32				Save( const char * pszPath);

	virtual UINT32				LoadHeader( i3Stream * pStream);

	bool						getBackgroundLoadEnable(void)				{ return m_bBackgroundLoad; }
	void						setBackgroundLoadEnable( bool bState)		{ m_bBackgroundLoad = bState; }

	static	void				AddResourceManager( i3ClassMeta * pMeta, i3NamedResourceManager * pResMnger);
	static	void				RemoveResourceManager( i3ClassMeta * pMeta);
	static	i3NamedResourceManager * GetResourceManager( i3ClassMeta * pMeta);
	static	void				SetExternalRefCallback( I3_EXTERNALREF_REPORT pUserProc, void * pUserData);
	static void					CallExternRefCallback( i3ResourceObject * pObj, const char * pszName);

	static	char *				GetWorkingFolder(void);
	static	void				SetWorkingFolder( char * pszPath);

	static void					setProfileEnable( bool bEnable);

	static bool					SetProtectionCode( const char * pszPath, const char * pKey, INT32 length);
	static bool					CheckProtectionCode( const char * pszPath, const char * pKey, INT32 length);

	static void					setExternRefResolveCallback( I3_EXTERNALREF_RESOLVE pUserProc, void * pUserData);
	static I3_EXTERNALREF_RESOLVE  getExternRefResolveCallback(void);
	static void					setSearchExternRefEnable( bool bFlag);
};

#endif
