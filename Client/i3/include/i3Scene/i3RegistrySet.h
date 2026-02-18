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
#include "i3RegArray.h"

#include "i3RegKey.h"

enum REGISTRY_FILE_TYPE
{
	REGISTRY_FILE_UNKNOWN = -1,
	REGISTRY_FILE_BINARY = 0,
	REGISTRY_FILE_XML_BY_UTF8,
	REGISTRY_FILE_XML_BY_EUC_KR,
};

class I3_EXPORT_SCENE i3RegistrySet :	public i3NamedElement
{
	I3_EXPORT_CLASS_DEFINE( i3RegistrySet, i3NamedElement);

protected:
	i3RegKey	*	m_pRoot;				// [initialize at constructor]
	char			m_szFileName[MAX_PATH] = { 0 };
	INT32			m_nCodePage = I3_LANG_CODE_ANSI;

	REGISTRY_FILE_TYPE  m_nFileType = REGISTRY_FILE_UNKNOWN;

public:
	i3RegistrySet(void);
	virtual ~i3RegistrySet(void);

	void	setRoot( i3RegKey * pRoot)
	{
		I3_REF_CHANGE( m_pRoot, pRoot);
	}

	i3RegKey *	getRoot()	
	{
		return m_pRoot;
	}

	UINT32	SaveRegistrySet( const char * pszFileName = nullptr, REGISTRY_FILE_TYPE nFileType = REGISTRY_FILE_BINARY);
	UINT32	LoadRegistrySet( const char * pszFileName, REGISTRY_FILE_TYPE nFileType = REGISTRY_FILE_BINARY);

	bool	ExportStringFile( const char * FileName );

	void	SetCodePage(INT32 nCodePage)		{ m_nCodePage = nCodePage;		}
	INT32	GetCodePage(void)					{ return m_nCodePage;			}

	// 현재 작업폴더에 Nation.nif에 기록된 나라갯수를 리턴한다.
	// 현재 PEF에 Array형태로 들어간항목의 갯수와 나라갯수가 틀릴경우에 처리하기위해서 필요하다.
	virtual INT32 GetNationCount() { return -1; }

private:
	char * _Parse_FindRegKey( char * pBuf, i3RegKey* pSrc, i3RegKey ** pOut);
	char * _Parse_FindDataName( char * pBuf, i3RegKey* pSrc, i3RegData ** pOut);
	char * _Parse_FindDataString( char * pBuf, i3RegData* pSrc, char ** pOut, INT32 nLength);
	bool _IsValidRegKey(char * pBuf);
	bool _IsValidRegData(char * pBuf);

	void	_RecursiveChild(i3RegKey* pKey, i3::vector<i3::rc_wstring>& Out);

private:
	//	For XML
	const char * _GetRegStyleName(UINT32 nStyle);
	bool _XML_CheckHeaderNode(TiXmlElement * pHeader);

	TiXmlElement * _XML_CreateRootNode(void);	
	TiXmlElement * _XML_CreateHeaderNode(void);

	//하위 위치에 데이터를 저장한다.
	TiXmlElement * _XML_ConvertRegToXML(i3RegKey * pRegNode);
	//하위 위치에 데이터 이름을 저장한다.
	TiXmlElement * _XML_ConvertDataToXML(i3RegKey * pRegNode, i3RegData * pData);
	//하위 위치에 데이터 이름과 내용을 저장한다.
	TiXmlElement * _XML_ConvertDataToXML(TiXmlElement * pXMLNode, i3RegData * pData, size_t Index);

	//읽기시에 사용된다.
	i3RegKey * _XML_ConvertXMLToReg( TiXmlElement * pRoot);
	i3RegData * _XML_ConvertXMLToData(TiXmlElement * pNode);

	//UTF8 -> MultiByte
	void _UTF8_MultiByte(char* UTF8, size_t sizeOfUTF8, const char* MultiByte);
	//MultiByte -> UTF8
	void _MultiByte_UTF8(char* MultiByte, size_t sizeOfMultiByte, const char* UTF8);

	//Xml에서 해당 Name에 대한 String을 얻어 온다
	void _XML_SetRegKeyNameFromXML( i3RegKey * pDest, TiXmlElement * pSrc);
	void _XML_SetRegDataNameFromXML( i3RegData * pDest, TiXmlElement * pSrc);
	void _XML_SetRegDataNameFromXML( i3RegData * pDest, TiXmlAttribute * pSrc);
	void _XML_SetRegElementStringFromXML( i3RegString * pDest, TiXmlElement * pSrc, TiXmlAttribute * pAttribute);

	//Registry에서 Text에 대한 값을 얻어 온다.
	void _XML_SetXMLNameFromRegKey( TiXmlElement * pDest, i3RegKey * pSrc);
	void _XML_SetXMLNameFromRegData( TiXmlElement * pDest, i3RegData * pSrc);
	void _XML_SetXMLValueFromRegData( TiXmlElement * pDest, i3RegData * pSrc, const char* szName);

protected:
	virtual void _XML_ConvertDataToXML_Name(UINT32 I3Reg_Type, 
		size_t Index, const char* szPreName,
		char * _DstBuf, size_t _SizeInBytes);

	void ClearStamp(i3RegKey * pKey);
};

#endif