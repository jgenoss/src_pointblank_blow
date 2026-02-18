#if !defined( __I3_XML_FILE_H)
#define __I3_XML_FILE_H

#include "i3ElementBase.h"
#include "i3ResourceFile.h"
#include "i3XML.h"
#include "i3RefStack.h"
#include "itl/vector.h"
#include "itl/map.h"

class I3_EXPORT_BASE i3XMLFile : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3XMLFile, i3ElementBase);
protected:
	i3XMLDocument *		m_pDoc = nullptr;
	i3XMLElement *		m_pRoot = nullptr;
	i3XMLElement *		m_pCur = nullptr;

	i3Error::CODE					m_LastError = i3Error::_NOERROR;

	i3::pack::RESOURCE_FILE_HEADER2		m_Header;
//	StringList					m_StrTable;					// 쓰이지 않는듯 하여 삭제..(2012.05.19.수빈)

	i3::vector<i3PersistantElement*>	m_ObjList;			// m_ObjList는 이제 레퍼런스카운팅이 들어감.(2012.09.17.수빈)

	INT32							m_IDCount = 0;
	I3_RES_OBJECT_ID_INFO *			m_pIDTable = nullptr;

	i3PersistantElement *			m_pKeyObject = nullptr;
	i3::rc_string					m_FileName;
	bool							m_bOverwriteExternalRes = false;


	i3RefStack						m_ObjStack;

protected:
	void					_CreateXMLDocument( const char * pszPath);
	i3PersistantElement *	_ResolveExtern( i3XMLElement * pXML);
	UINT32					_SaveExtern( INT32 i, i3ResourceObject * pObj, i3XMLElement * pXML);

	void					_setLastError( i3Error::CODE error, const char * pszMsg = nullptr, bool bLog = true);

public:
	i3XMLFile(void);
	virtual ~i3XMLFile(void);

	i3XMLDocument *			getDocument(void)				{ return m_pDoc; }
	i3XMLElement *			getRoot(void)					{ return m_pRoot; }
	i3XMLElement *			getCurrent(void)				{ return m_pCur; }

	const char *			getFileName(void) const			{ return m_FileName.c_str(); }

	i3Error::CODE			getLastError(void)				{ return m_LastError; }

	INT32					GetObjectCount(void)			{ return INT32(m_ObjList.size()); }
	i3PersistantElement *	GetObject( INT32 idx)			{ return m_ObjList[idx]; }
	i3::vector<i3PersistantElement*> * GetObjectList( void)	{ return &m_ObjList; }
	void					SetObjects( i3::vector<i3PersistantElement*>& ObjList);
	void					AddObject(i3PersistantElement * pObj) { I3ASSERT_RETURN(pObj);	 m_ObjList.push_back(pObj); pObj->AddRef(); }
	INT32					FindObjectInfoByID( OBJREF ID);
	i3PersistantElement *	FindObjectByID( OBJREF InstanceID);
	i3PersistantElement *	FindObjectByMeta( i3ClassMeta * pMeta);
	OBJREF					GetObjectPersistID( i3PersistantElement * pObj);

	void					pushCurrentObject( i3PersistantElement * pObj)			{ m_ObjStack.Push( pObj); }
	void					popCurrentObject(void)									{ m_ObjStack.Pop(nullptr); }
	i3PersistantElement *	getCurrentObject(void)									{ return (i3PersistantElement *) m_ObjStack.getTop(); }

	i3PersistantElement *	getKeyObject(void)					{ return m_pKeyObject; }
	void					setKeyObject( i3PersistantElement * pObj)
	{
		m_pKeyObject = pObj;
	}

	bool					GetOverwriteExternalResStatus(void)	{	return m_bOverwriteExternalRes ; }
	void					SetOverwriteExternalResStatus( bool bFlag)
	{
		m_bOverwriteExternalRes = bFlag;
	}

	UINT16					GetVersionMajor(void)				{ return m_Header.m_MajorVersion; }
	UINT16					GetVersionMinor(void)				{ return m_Header.m_MinorVersion; }

	virtual bool			Load( const char * pszPath);
	virtual bool			Save( const char * pszPath);
};

// 코드 블로팅 가능성이 좀 있다..

template<class T>
bool	SaveFromListToXML(const i3::vector<T>& in_list, const char * pszName, i3XMLFile * pFile, i3XMLElement * pXML, bool bRef)
{
	char szName[256];
	i3XMLElement * pChildXML;
	
	pChildXML = new i3XMLElement( pszName);

	pChildXML = (i3XMLElement*) pXML->LinkEndChild( pChildXML);
	I3ASSERT( pChildXML != nullptr);

	pChildXML->SetAttribute( "Count", (INT32)in_list.size());

	for(size_t i = 0; i < in_list.size(); i++)
	{
		sprintf( szName, "Item%d", i);

		if( bRef)
		{
			i3PersistantElement * pObj = (i3PersistantElement *) in_list[i];

			I3ASSERT( i3::kind_of<i3PersistantElement*>(pObj));

			OBJREF ref = pFile->GetObjectPersistID( pObj);
			I3ASSERT( ref > 0);

			pChildXML->SetAttribute( szName, (INT32) ref);
		}
		else
		{
			pChildXML->SetAttribute( szName, (INT32) in_list[i]);
		}
	}

	return true;
}

template<class T>
bool	LoadFromXMLToList( i3::vector<T>& out_list, const char * pszName, i3XMLFile * pFile, i3XMLElement * pXML, bool bAddRef)
{
	char szName[256];
	i3XMLElement * pChildXML;
	INT32 i, cnt;
	UINT32 val;

	pChildXML = (i3XMLElement *) pXML->FirstChildElement( pszName);
	I3ASSERT_RETURN( pChildXML != nullptr, false);

	pChildXML->Attribute( "Count", &cnt);

	if( cnt > 0)
	{
		out_list.resize( cnt);

		for( i = 0; i < cnt; i++)
		{
			sprintf( szName, "Item%d", i);

			if( pChildXML->Attribute( szName, (int *) &val) != nullptr)
			{
				if( bAddRef)
				{
					i3PersistantElement * pObj = pFile->FindObjectByID( (OBJREF) val);
					I3_MUST_ADDREF( pObj);

					out_list[i] = static_cast<T>(pObj);
				}
				else
				{
					out_list[i] = reinterpret_cast<T>(val);
				}
			}
			else
			{
				I3PRINTLOG(I3LOG_WARN,  "Found not found %d XML element from %s", szName, pszName);

				out_list[i] = nullptr;
			}
		}
	}

	return true;
}

#endif
