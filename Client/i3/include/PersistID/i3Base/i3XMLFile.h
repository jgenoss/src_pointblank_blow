#if !defined( __I3_XML_FILE_H)
#define __I3_XML_FILE_H

#include "i3ElementBase.h"
#include "i3ResourceFile.h"
#include "i3XML.h"
#include "i3RefStack.h"

class I3_EXPORT_BASE i3XMLFile : public i3ElementBase
{
	I3_CLASS_DEFINE( i3XMLFile);
protected:
	i3XMLDocument *		m_pDoc;
	i3XMLElement *		m_pRoot;
	i3XMLElement *		m_pCur;

	i3Error::CODE					m_LastError;

	I3_RESOURCE_FILE_HEADER2		m_Header;
	i3StringList					m_StrTable;
	i3List							m_ObjList;

	INT32							m_IDCount;
	I3_RES_OBJECT_ID_INFO *			m_pIDTable;

	i3PersistantElement *			m_pKeyObject;

	bool							m_bOverwriteExternalRes;
	char							m_szFileName[ MAX_PATH];

	i3RefStack						m_ObjStack;

protected:
	void					_CreateXMLDocument( const char * pszPath);
	i3PersistantElement *	_ResolveExtern( i3XMLElement * pXML);
	UINT32					_SaveExtern( INT32 i, i3ResourceObject * pObj, i3XMLElement * pXML);

	void					_setLastError( i3Error::CODE error, bool bLog = true);

public:
	i3XMLFile(void);
	virtual ~i3XMLFile(void);

	i3XMLDocument *			getDocument(void)				{ return m_pDoc; }
	i3XMLElement *			getRoot(void)					{ return m_pRoot; }
	i3XMLElement *			getCurrent(void)				{ return m_pCur; }

	char *					getFileName(void)				{ return m_szFileName; }

	i3Error::CODE			getLastError(void)				{ return m_LastError; }

	INT32					GetObjectCount(void)			{ return m_ObjList.GetCount(); }
	i3PersistantElement *	GetObject( INT32 idx)			{ return (i3PersistantElement *) m_ObjList.Items[idx]; }
	void					SetObjects( i3List * pObjList);
	void					AddObject( i3PersistantElement * pObj)		{ m_ObjList.Add( pObj); }
	INT32					FindObjectInfoByID( OBJREF ID);
	i3PersistantElement *	FindObjectByID( OBJREF InstanceID);
	i3PersistantElement *	FindObjectByMeta( i3ClassMeta * pMeta);
	OBJREF					GetObjectPersistID( i3PersistantElement * pObj);

	void					pushCurrentObject( i3PersistantElement * pObj)			{ m_ObjStack.Push( pObj); }
	void					popCurrentObject(void)									{ m_ObjStack.Pop( NULL); }
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

#endif
