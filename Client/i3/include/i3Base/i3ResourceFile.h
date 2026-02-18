#if !defined( __I3_RESOURCE_FILE_H)
#define __I3_RESOURCE_FILE_H

//#include "i3CommonType.h"
#include "i3ElementBase.h"
#include "i3PersistantElement.h"
#include "i3Stream.h"
#include "i3NamedResourceManager.h"
#include "itl/vector.h"
#include "string/rc_string.h"

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

#define		I3_RSCFILE_STYLE_BIGENDIAN				0x00000001

#define		I3_RSCFILE_HEAD_10		"I3RF"
#define		I3_RSCFILE_HEAD_20		"I3R2"

namespace i3
{
	namespace pack
	{
		struct PACKED RESOURCE_FILE_HEADER1
		{
			UINT8		m_MajorVersion = 0;
			UINT8		m_MinorVersion = 0;

			UINT16		m_StringTableCount = 0;
			UINT32		m_StringTableOffset = 0;
			UINT32		m_StringTableSize = 0;

			UINT16		m_ObjectCount = 0;
			UINT32		m_ObjectDirOffset = 0;
			UINT32		m_ObjectDirSize = 0;
		};

		struct PACKED RESOURCE_FILE_HEADER2
		{
			UINT32		m_Style = 0;
			UINT16		m_MajorVersion = 0;
			UINT16		m_MinorVersion = 0;

			UINT32		m_StringTableCount = 0;
			UINT64		m_StringTableOffset = 0;
			UINT64		m_StringTableSize = 0;

			UINT32		m_ObjectCount = 0;
			UINT64		m_ObjectDirOffset = 0;
			UINT64		m_ObjectDirSize = 0;

			OBJREF64	m_pKeyObject = 0;

			UINT32		m_ChecksumCode[4] = { 0 };
			UINT32		pad[28] = { 0 };
		};	// size 184

		struct PACKED RESOURCE_FILE_OBJECT_INFO1
		{
			UINT16		m_ClassNameIndex = 0;
			UINT32		m_InstanceID = 0;
			UINT32		m_Offset = 0;
			UINT32		m_Size = 0;
		};

		struct PACKED RESOURCE_FILE_OBJECT_INFO2
		{
			UINT32		m_ClassNameIndex = 0;
			UINT64		m_InstanceID = 0;
			UINT64		m_Offset = 0;
			UINT64		m_Size = 0;
		};
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

struct I3_RESOURCE_MANAGER_INFO
{
	i3ClassMeta *				m_pMeta = nullptr;
	i3NamedResourceManager *	m_pManager = nullptr;
} ;

struct I3_RES_OBJECT_ID_INFO
{
	OBJREF			m_ID = 0;
	INT32			m_Ordinal = 0;
} ;


struct I3_RESOURCE_FILE_HEAD_INFO
{
	UINT32		m_Style = 0;
	UINT16		m_MajorVersion = 0;
	UINT16		m_MinorVersion = 0;

	UINT32		m_StringTableCount = 0;
	UINT64		m_StringTableOffset = 0;
	UINT64		m_StringTableSize = 0;

	UINT32		m_ObjectCount = 0;
	UINT64		m_ObjectDirOffset = 0;
	UINT64		m_ObjectDirSize = 0;

	OBJREF64	m_pKeyObject = 0;

	UINT32		m_ChecksumCode[4] = { 0, 0, 0, 0 };

	void	CopyFromFile( i3::pack::RESOURCE_FILE_HEADER2 * pFile)
	{
		m_Style = pFile->m_Style;
		m_MajorVersion = pFile->m_MajorVersion;
		m_MinorVersion = pFile->m_MinorVersion;

		m_StringTableCount = pFile->m_StringTableCount;
		m_StringTableOffset = pFile->m_StringTableOffset;
		m_StringTableSize = pFile->m_StringTableSize;

		m_ObjectCount = pFile->m_ObjectCount;
		m_ObjectDirOffset = pFile->m_ObjectDirOffset;
		m_ObjectDirSize = pFile->m_ObjectDirSize;

		m_pKeyObject = pFile->m_pKeyObject;

		m_ChecksumCode[0] = pFile->m_ChecksumCode[0];
		m_ChecksumCode[1] = pFile->m_ChecksumCode[1];
		m_ChecksumCode[2] = pFile->m_ChecksumCode[2];
		m_ChecksumCode[3] = pFile->m_ChecksumCode[3];
	}
} ;


template<class T> 
UINT32	LoadFromResourceFileToList(i3::vector<T>& out_list, i3ResourceFile * pResFile, bool bAddRef = false);

typedef void (*PROGRESSPROC)( void * pUserData, UINT32 curPos, UINT32 maxPos);
typedef void (*I3_EXTERNALREF_REPORT)( i3PersistantElement * pElm, const char * pszPath, void * pUserData);
typedef bool (*I3_EXTERNALREF_RESOLVE)( const char * pszPath, char * pszNewPath, INT32 lenBuff, const char * pszWorkPath, bool bSearch);

class I3_EXPORT_BASE i3ResourceFile : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3ResourceFile, i3ElementBase);
public:
	enum RESOURCE_HEADER_PAD
	{
		LOW_DATA_TIME = 0,
		HIGH_DATA_TIME
	};
protected:

	i3::vector<i3::rc_string>		m_StrTable;

	i3Stream *						m_pStream = nullptr;
	i3::vector<i3PersistantElement*> m_ObjList;			// m_ObjList는 이제 레퍼런스카운팅이 들어감.(2012.09.17.수빈)
	i3::pack::RESOURCE_FILE_OBJECT_INFO2 *	m_pObjInfo = nullptr;
	I3_RESOURCE_FILE_HEAD_INFO		m_Header;

	INT32							m_IDCount = 0;
	I3_RES_OBJECT_ID_INFO *			m_pIDTable = nullptr;

	i3PersistantElement *			m_pKeyObject = nullptr;

	PROGRESSPROC					m_pUserProgressProc = nullptr;
	void *							m_pUserData = nullptr;

	i3::rc_string					m_FileName;

	bool							m_bOverwriteExternalRes= false;
	bool							m_bBackgroundLoad = false;
	bool							m_bInThread = false;

protected:
	UINT32				GetAlignedPos( UINT32 pos);
	UINT32				Align( i3Stream * pStream);

	i3PersistantElement *	_ResolveExtern( INT32 idx, bool bCommonRes);
	UINT32					_SaveExtern( INT32 i, i3ResourceObject * pObj);

	void					_BuildIDTable( INT32 count);
	void					_Rec_QuickSort( I3_RES_OBJECT_ID_INFO * pTable, INT32 idxMin, INT32 idxMax);

	bool					_isExtern( OBJREF ref);
	INT32					_getRefStringIndex( OBJREF ref);
	INT32					_getRefOrdinal( OBJREF ref);

	virtual bool			CheckExternNeedLoad(i3ClassMeta* RscClassMeta) const { return true; }

public:
	virtual bool			IsCheckedExternNeedLoad() const { return false; }

	i3ResourceFile(void);
	virtual ~i3ResourceFile(void);

	const char *			getFileName(void) const					{ return m_FileName.c_str(); }

	const i3::vector<i3::rc_string>&	GetStringTable(void) const	{ return m_StrTable; }

	i3Stream *				GetStream(void)						{ return m_pStream; }
	void					SetStream( i3Stream * pStream)		{ m_pStream = pStream; }

	I3_RESOURCE_FILE_HEAD_INFO *	getHeader(void)				{ return &m_Header; }
	i3::pack::RESOURCE_FILE_OBJECT_INFO2 *	getObjectInfo( INT32 i)		{ return &m_pObjInfo[ i]; }

	INT32					GetObjectCount(void)				{ return INT32(m_ObjList.size()); }
	i3::vector<i3PersistantElement*> *	GetObjectList( void)	{ return &m_ObjList; }
	i3PersistantElement *	GetObject( INT32 idx)				{ return m_ObjList[idx]; }
	void					SetObjects( i3::vector<i3PersistantElement*>& ObjList);
	void					AddObject( i3PersistantElement * pObj)
	{
		I3_MUST_ADDREF( pObj);
		m_ObjList.push_back( pObj);
	}
	void					DeleteObject(INT32 nID);		
	INT32					FindObjectInfoByID( OBJREF ID);
	i3PersistantElement *	FindObjectByID( OBJREF InstanceID);
	i3::rc_string			FindNameByID( OBJREF InstanceID);

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

	virtual UINT32				Load( i3Stream * pStream, bool bCommonRes = false);
	virtual UINT32				Save( i3Stream * pStream);

	virtual UINT32				Load( const char * pszPath, bool bCommonRes = false);
	virtual UINT32				Save( const char * pszPath);

	virtual UINT32				LoadHeader( i3Stream * pStream);
	

	bool						getBackgroundLoadEnable(void)				{ return m_bBackgroundLoad; }
	void						setBackgroundLoadEnable( bool bState)		{ m_bBackgroundLoad = bState; }

	bool						InThread( void)								{ return m_bInThread; }
	void						SetInThread( bool bVal)						{ m_bInThread = bVal; }

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

	i3PersistantElement *		ResolveExtern( INT32 objIdx );

	static void					RemoveAllResourceList();
};




/////////////////////////////////////////////////////////////////////////////////////////
// code bloating 가능성이 좀 있음..
//////

template<class T> 
UINT32 SaveFromListToResourceFile( const i3::vector<T>& in_list, i3ResourceFile* pResFile)
{
	
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	OBJREF ref;

	UINT32 cnt = in_list.size();

	// Count
	{
		Rc = pStream->Write( &cnt, sizeof(cnt));
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not write count data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	for(UINT32 i = 0; i < cnt ; i++)
	{
		ref = (OBJREF) pResFile->GetObjectPersistID( (i3PersistantElement *)in_list[i]);

		Rc = pStream->Write( &ref, sizeof(ref));
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not write pointer data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
		
}


template<class T> 
UINT32	LoadFromResourceFileToList(i3::vector<T>& out_list, i3ResourceFile * pResFile, bool bAddRef)
{
	UINT32 Rc, Result = 0;
	UINT32 cnt;
	OBJREF ref;
	i3ElementBase * pObj;
	i3Stream * pStream = pResFile->GetStream();

	// Count
	Rc = pStream->Read( &cnt, sizeof(cnt));
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not read count data.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( cnt > 0)
	{
		out_list.resize( cnt);

		for(size_t i = 0; i < (INT32) cnt; i++)
		{
			Rc = pStream->Read( &ref, sizeof(ref));
			if( Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Could not read pointer data.");
				return STREAM_ERR;
			}
			Result += Rc;

			if( bAddRef)
			{
				pObj = (i3ElementBase *) pResFile->FindObjectByID( ref);
				I3_SAFE_ADDREF( pObj);

				out_list[i] = static_cast<T>(pObj);
			}
			else
			{
				out_list[i] = reinterpret_cast<T>(ref);
			}
		}
	}

	return Result;
}


#endif
