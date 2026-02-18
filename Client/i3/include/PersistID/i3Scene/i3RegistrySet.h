#if !defined( _I3REGISTRY_SET_H__)
#define _I3REGISTRY_SET_H__

#include "i3RegINT32.h"
#include "i3RegREAL32.h"
#include "i3RegString.h"
#include "i3RegVEC2D.h"
#include "i3RegVEC3D.h"

#include "i3RegVEC4D.h"
#include "i3RegCOLOR.h"
#include "i3RegMATRIX.h"
#include "i3RegKey.h"

enum REGISTRY_FILE_TYPE
{
	REGISTRY_FILE_BINARY,
	REGISTRY_FILE_XML,
};

class I3_EXPORT_SCENE i3RegistrySet :	public i3NamedElement
{
	I3_CLASS_DEFINE( i3RegistrySet);

protected:
	i3RegKey	*	m_pRoot;
	char			m_szFileName[MAX_PATH];

public:
	i3RegistrySet(void);
	virtual ~i3RegistrySet(void);

	void	setRoot( i3RegKey * pRoot)
	{
		I3_REF_CHANGE( m_pRoot, pRoot);
	}

	i3RegKey *	getRoot()	{	return m_pRoot;}

	UINT32	SaveRegistrySet( const char * pszFileName = NULL, REGISTRY_FILE_TYPE nFileType = REGISTRY_FILE_BINARY);
	UINT32	LoadRegistrySet( const char * pszFileName, REGISTRY_FILE_TYPE nFileType = REGISTRY_FILE_BINARY);

private:
	//	For XML
	const char * _GetRegStyleName(UINT32 nStyle);	
	TiXmlElement * _XML_CreateRootNode(void);	
	TiXmlElement * _XML_ConvertRegToXML(i3RegKey * pRegNode);
	TiXmlElement * _XML_ConvertDataToXML(i3RegKey * pRegNode, INT32 idxData);
	i3RegKey * _XML_ConvertXMLToReg( TiXmlElement * pRoot);
	i3RegData * _XML_ConvertXMLToData(TiXmlElement * pNode);
	TiXmlElement * _XML_CreateHeaderNode(void);
	BOOL _XML_CheckHeaderNode(TiXmlElement * pHeader);
};

#endif