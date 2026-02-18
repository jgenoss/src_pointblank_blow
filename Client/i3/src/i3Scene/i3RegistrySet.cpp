#include "i3SceneDef.h"
#include "i3RegistrySet.h"
#include "i3Registry.h"
#include "i3Base/string/ext/remove_blank_from_head.h"
#include "i3Base/string/ext/remove_blank_from_tail.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/generic_string_size.h"
#include "i3Base/string/ext/contain_string_head.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/ext/generic_string_replace_all.h"
#include "i3Base/string/ext/utf8_to_utf16.h"
#include "i3Base/string/ext/utf16_to_utf8.h"
#include "i3Base/string/ext/extract_filetitle_fileext.h"
#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/compare/generic_compare.h"

#define XML_HEADER_SYSTEM		"i3Engine"
#define XML_HEADER_ID			"PropertyEditorFile"


static WCHAR16 *	s_pszWideBuffer = nullptr;
static INT32		s_nWideLength = 0;

static void _CheckBuffer(const char * pString, INT32 len)
{
	if( s_pszWideBuffer == nullptr || s_nWideLength < len)
	{
		I3MEM_SAFE_FREE( s_pszWideBuffer);

		INT32 def = 1024;

		while( def < len)
			def *= 2;

		s_nWideLength = def;

		s_pszWideBuffer = (WCHAR16*) i3MemAlloc( sizeof(WCHAR16) * s_nWideLength);
		I3ASSERT( s_pszWideBuffer != nullptr);
	}

	i3mem::FillZero(s_pszWideBuffer, sizeof(WCHAR16) * s_nWideLength);
}

static void _ReleaseBuffer(void)
{
	I3MEM_SAFE_FREE(s_pszWideBuffer);
	s_nWideLength = 0;
}

//	MultiByte -> UTF-8
void _ConvertMultiByteToUTF8( INT32 nCP, const char * pSrc, INT32 nSrclen, char * pOut, INT32 nOutSize)
{
	_CheckBuffer(pSrc, nSrclen);

	INT32 cp = (I3_LANG_CODE_ANSI == nCP ? CP_ACP : nCP);

	i3String::MultiByteToWideChar( cp, 0, pSrc, nSrclen, s_pszWideBuffer, s_nWideLength);
	i3String::WideCharToMultiByte( CP_UTF8, 0, s_pszWideBuffer, s_nWideLength, pOut, nOutSize, 0, 0);
}

//	UTF-8 -> MultiByte
void _ConvertUTF8ToMultiByte( INT32 nCP, const char * pSrc, INT32 nSrclen, char * pOut, INT32 nOutSize)
{
	_CheckBuffer(pSrc, nSrclen);

	INT32 cp = (I3_LANG_CODE_ANSI == nCP ? CP_ACP : nCP);

	i3String::MultiByteToWideChar( CP_UTF8, 0, pSrc, nSrclen, s_pszWideBuffer, s_nWideLength);
	i3String::WideCharToMultiByte( cp, 0, s_pszWideBuffer, s_nWideLength, pOut, nOutSize, 0, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

I3_CLASS_INSTANCE( i3RegistrySet);

i3RegistrySet::i3RegistrySet(void)
{
	m_pRoot = i3RegKey::new_object();
	m_pRoot->setNodeStyle( I3REG_STYLE_ROOT);
}

i3RegistrySet::~i3RegistrySet(void)
{
	I3_SAFE_RELEASE( m_pRoot);
}

void i3RegistrySet::ClearStamp(i3RegKey * pKey)
{
	pKey->ClearStampList();

	INT32	nItemCount = pKey->getChildCount();
	for( INT32 i = 0; i < nItemCount; ++i)
	{
		i3RegKey * pChildKey = (i3RegKey *)pKey->getChild( i);
		ClearStamp( pChildKey );
	}
}

bool i3RegistrySet::_XML_CheckHeaderNode(TiXmlElement * pHeader)
{
	const char * pszSystem = pHeader->Attribute("System");
	const char * pszID = pHeader->Attribute("ID");

	if( i3::generic_is_iequal(pszSystem, XML_HEADER_SYSTEM) == false)
		return false;

	if( i3::generic_is_iequal(pszID, XML_HEADER_ID) == false)
		return false;

	return true;
}


UINT32	i3RegistrySet::SaveRegistrySet( const char * pszFileName, REGISTRY_FILE_TYPE nSaveType)
{
	if( m_pRoot == nullptr)	return 0;

	char szFileName[MAX_PATH];
	UINT32 result = 0;

	if( pszFileName == nullptr)
		i3::string_ncopy_nullpad( szFileName, m_szFileName, MAX_PATH - 1);
	else
		i3::string_ncopy_nullpad( szFileName, pszFileName, MAX_PATH - 1);

	I3ASSERT( szFileName[0] != 0);

	m_nFileType = nSaveType;

	switch( nSaveType)
	{
	case REGISTRY_FILE_BINARY:
		{
			i3ResourceFile File;

			i3::vector<i3PersistantElement*> list(1, m_pRoot);
			//list.Add( m_pRoot);

			File.SetObjects( list);

			result = File.Save( szFileName);
		}
		break;
	case REGISTRY_FILE_XML_BY_UTF8:
	case REGISTRY_FILE_XML_BY_EUC_KR:
		{
			//	XML Document 생성
			TiXmlDocument doc( szFileName);

			//	XML 포멧 선언 (다국어를 위해 UTF-8 포멧만 사용하도록 한다.)
			//	XML 버전은 1.0을 사용하도록 한다. 문서 버전이 아님)
			if( nSaveType == REGISTRY_FILE_XML_BY_UTF8)
			{
				TiXmlDeclaration decl("1.0", "UTF-8", "yes");
				doc.InsertEndChild(decl);
			}
			else if( nSaveType == REGISTRY_FILE_XML_BY_EUC_KR)
			{
				TiXmlDeclaration decl("1.0", "euc-kr", "yes");
				doc.InsertEndChild(decl);
			}

			TiXmlElement * pRoot = _XML_CreateRootNode();

			//	XML document에 root node 추가
			doc.LinkEndChild(pRoot);

			//	XML 파일 저장
			if(doc.SaveFile())
			{
				result = 1;
			}
			else
			{
				I3PRINTLOG(I3LOG_FATAL, "Cannot save to XML format file.");
			}
		}
		break;
	}

	_ReleaseBuffer();

	return result;
}

TiXmlElement * i3RegistrySet::_XML_CreateHeaderNode(void)
{
	TiXmlElement * pHeader = new TiXmlElement("HEADER");

	//	사용 시스템
	pHeader->SetAttribute("System", XML_HEADER_SYSTEM);
	//	ID
	pHeader->SetAttribute("ID", XML_HEADER_ID);
	//	버전
	pHeader->SetAttribute("Version", "1.0");

	return pHeader;
}

const char * i3RegistrySet::_GetRegStyleName(UINT32 nStyle)
{
	if( nStyle & I3REG_STYLE_ROOT)
	{
		return "ROOT";
	}

	switch( nStyle & I3REG_TYPE_MASK)
	{
	case I3REG_TYPE_INT32:			return "INT32";
	case I3REG_TYPE_REAL32:			return "REAL32";
	case I3REG_TYPE_STRING:			return "STRING";
	case I3REG_TYPE_VEC2D:			return "VEC2D";
	case I3REG_TYPE_VEC3D:			return "VEC3D";
	case I3REG_TYPE_VEC4D:			return "VEC4D";
	case I3REG_TYPE_COLOR:			return "COLOR";
	case I3REG_TYPE_MATRIX:			return "MATRIX";
	case I3REG_TYPE_INT32EX:		return "INT32EX";
	}

	return nullptr;
}

TiXmlElement * i3RegistrySet::_XML_CreateRootNode(void)
{
	TiXmlElement * pXMLRoot = new TiXmlElement("ROOT");

	const char * pszName = m_pRoot->GetName();
	if( pszName != nullptr)
		pXMLRoot->SetAttribute("Name", m_pRoot->GetName());
	else
		pXMLRoot->SetAttribute("Name", "");

	TiXmlElement * pXMLHeader = _XML_CreateHeaderNode();
	pXMLRoot->LinkEndChild(pXMLHeader);

	//	RegData를 XML Element로 Convert하여 node에 링크
	for(INT32 i=0; i<m_pRoot->GetDataCount(); i++)
	{
		i3RegData * pData = m_pRoot->GetData(i);
		TiXmlElement * pXMLData = _XML_ConvertDataToXML(m_pRoot, pData);

		i3RegArray* pRegArray = nullptr;
		size_t size = 1;

		if( pData->getDataType() == I3REG_TYPE_ARRAY )
		{
			pRegArray = (i3RegArray*)pData;
			size = pRegArray->getSize();
		}

		for(size_t index = 0; index<size; index++)
		{
			if( pRegArray )
				_XML_ConvertDataToXML( pXMLData, (*pRegArray)[index], index );
			else
				_XML_ConvertDataToXML( pXMLData, pData, size_t(-1) );
		}

		pXMLRoot->LinkEndChild(pXMLData);
	}

	//	자식노드는 재귀호출
	for(INT32 i=0; i<m_pRoot->getChildCount(); i++)
	{
		i3RegKey * pRegChild = (i3RegKey*) m_pRoot->getChild(i);

		TiXmlElement * pXMLChild = _XML_ConvertRegToXML(pRegChild);

		pXMLRoot->LinkEndChild(pXMLChild);
	}

	return pXMLRoot;
}

TiXmlElement * i3RegistrySet::_XML_ConvertDataToXML(i3RegKey * pRegNode, i3RegData * pData)
{
	UINT32 Type = pData->getDataType();

	if( Type == I3REG_TYPE_ARRAY )
	{
		i3RegArray *pArrayData = (i3RegArray *)pData;
		Type = pArrayData->getElementType();
	}

	const char * pszStyle = _GetRegStyleName( Type );

	char szName[256] = "";

	INT32 nRelationCount = 0;
	i3RegKey * pParentNode = (i3RegKey*) pRegNode->getParent();
	while( pParentNode)
	{
		nRelationCount++;

		pParentNode = (i3RegKey*) pParentNode->getParent();
	}

	sprintf( szName, "%s_%d", pszStyle, nRelationCount);

	TiXmlElement * pXMLNode = new TiXmlElement(szName);

	const char * pszName = pData->GetName();
	if( pszName != nullptr)
	{
		_XML_SetXMLNameFromRegData( pXMLNode, pData);
	}
	else
		pXMLNode->SetAttribute("Name", "");

	return pXMLNode;
}

TiXmlElement * i3RegistrySet::_XML_ConvertRegToXML(i3RegKey * pRegNode)
{
	char szStyle[256] = "";

	INT32 nRelationCount = 0;
	i3RegKey * pParentNode = (i3RegKey*) pRegNode->getParent();
	while( pParentNode)
	{
		nRelationCount++;

		pParentNode = (i3RegKey*) pParentNode->getParent();
	}

	//	엘레멘트 이름
	if(!(pRegNode->getNodeStyle() & I3REG_STYLE_ROOT))
	{
		sprintf( szStyle, "KEY_%d", nRelationCount);
	}
	else
	{
		sprintf( szStyle, "UNKNOWN");
	}

	TiXmlElement * pXMLNode = new TiXmlElement(szStyle);

	const char * pszName = pRegNode->GetName();
	if( pszName != nullptr)
	{
		_XML_SetXMLNameFromRegKey( pXMLNode, pRegNode);
	}
	else
		pXMLNode->SetAttribute("Name", "");

	//	RegData를 XML Element로 Convert하여 node에 링크
	for(INT32 i=0; i<pRegNode->GetDataCount(); i++)
	{
		i3RegData * pData = pRegNode->GetData(i);
		TiXmlElement * pXMLData = _XML_ConvertDataToXML(pRegNode, pData);

		i3RegArray* pRegArray = nullptr;
		size_t size = 1;

		if( pData->getDataType() == I3REG_TYPE_ARRAY )
		{
			pRegArray = (i3RegArray*)pData;
			size = pRegArray->getSize();
		}

		for(size_t index = 0; index<size; index++)
		{
			if( pRegArray )
				_XML_ConvertDataToXML( pXMLData, (*pRegArray)[index], index );
			else
				_XML_ConvertDataToXML( pXMLData, pData, size_t(-1) );
		}

		pXMLNode->LinkEndChild(pXMLData);
	}

	//	자식노드는 재귀호출
	for(INT32 i=0; i<pRegNode->getChildCount(); i++)
	{
		i3RegKey * pRegChild = (i3RegKey*) pRegNode->getChild(i);

		TiXmlElement * pXMLChild = _XML_ConvertRegToXML(pRegChild);

		pXMLNode->LinkEndChild(pXMLChild);
	}

	return pXMLNode;
}

void i3RegistrySet::_XML_ConvertDataToXML_Name(UINT32 I3Reg_Type, 
											   size_t Index, const char* szPreName,
											   char * _DstBuf, size_t _SizeInBytes)
{
	if( Index == size_t(-1) )
		sprintf_s( _DstBuf, _SizeInBytes, "%s", szPreName);
	else
		sprintf_s( _DstBuf, _SizeInBytes, "%s_%d", szPreName, Index);
}

TiXmlElement * i3RegistrySet::_XML_ConvertDataToXML(TiXmlElement * pXMLNode, i3RegData * pData, size_t Index)
{
	char szName[256] = "";

	switch(pData->getDataType() & I3REG_TYPE_MASK)
	{
	case I3REG_TYPE_INT32:
		{
			INT32 val = 0;
			pData->GetValue(&val);
			
			_XML_ConvertDataToXML_Name(pData->getDataType(), Index, "Value", szName, 256);

			pXMLNode->SetAttribute(szName, val);
		}
		break;
	case I3REG_TYPE_REAL32:
		{
			REAL32 val = 0;
			pData->GetValue(&val);

			_XML_ConvertDataToXML_Name(pData->getDataType(), Index, "Value", szName, 256);

			pXMLNode->SetDoubleAttribute(szName, val);
		}
		break;
	case I3REG_TYPE_STRING:
		{
			_XML_ConvertDataToXML_Name(pData->getDataType(), Index, "Value", szName, 256);

			//	숫자, 영문만 있는 스트링일 경우 UTF-8로 변환할 필요가 없으므로 String 데이터만 UTF-8 로 저장
			_XML_SetXMLValueFromRegData( pXMLNode, pData, szName);
		}
		break;
	case I3REG_TYPE_VEC2D:
		{
			VEC2D val;
			pData->GetValue(&val);

			_XML_ConvertDataToXML_Name(pData->getDataType(), Index, "X", szName, 256);
			pXMLNode->SetDoubleAttribute(szName, val.x);

			_XML_ConvertDataToXML_Name(pData->getDataType(), Index, "Y", szName, 256);
			pXMLNode->SetDoubleAttribute(szName, val.y);
		}
		break;
	case I3REG_TYPE_VEC3D:
		{
			VEC3D val;
			pData->GetValue(&val);

			_XML_ConvertDataToXML_Name(pData->getDataType(), Index, "X", szName, 256);
			pXMLNode->SetDoubleAttribute(szName, val.x);

			_XML_ConvertDataToXML_Name(pData->getDataType(), Index, "Y", szName, 256);
			pXMLNode->SetDoubleAttribute(szName, val.y);

			_XML_ConvertDataToXML_Name(pData->getDataType(), Index, "Z", szName, 256);
			pXMLNode->SetDoubleAttribute(szName, val.z);
		}
		break;
	case I3REG_TYPE_VEC4D:
		{
			VEC4D val;

			pData->GetValue(&val);

			_XML_ConvertDataToXML_Name(pData->getDataType(), Index, "X", szName, 256);
			pXMLNode->SetDoubleAttribute(szName, val.x);

			_XML_ConvertDataToXML_Name(pData->getDataType(), Index, "Y", szName, 256);
			pXMLNode->SetDoubleAttribute(szName, val.y);

			_XML_ConvertDataToXML_Name(pData->getDataType(), Index, "Z", szName, 256);
			pXMLNode->SetDoubleAttribute(szName, val.z);

			_XML_ConvertDataToXML_Name(pData->getDataType(), Index, "W", szName, 256);
			pXMLNode->SetDoubleAttribute(szName, val.w);
		}
		break;
	case I3REG_TYPE_COLOR:
		{
			I3COLOR val;

			pData->GetValue(&val);

			_XML_ConvertDataToXML_Name(pData->getDataType(), Index, "R", szName, 256);
			pXMLNode->SetAttribute(szName, val.r);

			_XML_ConvertDataToXML_Name(pData->getDataType(), Index, "G", szName, 256);
			pXMLNode->SetAttribute(szName, val.g);

			_XML_ConvertDataToXML_Name(pData->getDataType(), Index, "B", szName, 256);
			pXMLNode->SetAttribute(szName, val.b);

			_XML_ConvertDataToXML_Name(pData->getDataType(), Index, "A", szName, 256);
			pXMLNode->SetAttribute(szName, val.a);
		}
		break;
	case I3REG_TYPE_MATRIX:
		{
			MATRIX val;
			char temp[64] = "";

			pData->GetValue(&val);

			for(INT32 i=0; i<4; i++)
			{
				for(INT32 j=0; j<4; j++)
				{
					sprintf(temp, "m%d%d", i, j);
					pXMLNode->SetDoubleAttribute(temp, val.m[i][j]);
				}
			}
		}
		break;
	}

	return pXMLNode;
}

UINT32	i3RegistrySet::LoadRegistrySet( const char * pszFileName, REGISTRY_FILE_TYPE nFileType)
{
	UINT32	nReadBytes = STREAM_ERR;

	m_nFileType = nFileType;

	switch( nFileType)
	{
	case REGISTRY_FILE_BINARY:
		{
			i3ResourceFile File;
			if (i3FileStream::isFile(pszFileName))
			{
				nReadBytes = File.Load(pszFileName);
			}
			else
			{
				char szFileName[MAX_PATH];
				char szExt[MAX_PATH];
				i3::extract_filetitle_fileext(pszFileName, szFileName, szExt);
				if (i3::generic_compare(szExt, "pef") == 0)
				{
					i3::generic_copy_range(szFileName, pszFileName);
					i3::change_file_ext((char*)szFileName, "i3RegXML");
					return LoadRegistrySet(szFileName, REGISTRY_FILE_XML_BY_UTF8);
				}
			}

			if( nReadBytes == STREAM_ERR)
			{
				return STREAM_ERR;
			}

			INT32 nObjCount = File.GetObjectCount();
			for( INT32 nKey = 0; nKey < nObjCount; ++nKey)
			{
				i3RegKey * pKey = (i3RegKey*)File.GetObject(nKey);
				if( pKey->getNodeStyle() == I3REG_STYLE_ROOT)
				{
					setRoot( pKey);
					break;
				}
			}


		}

		if( !i3Registry::Get_LoadStamp() )						// i3ResourceFile소멸 보다 늦게 처리하면 TRACE경고(False Positive)를 제거할수 있는것으로 보인다.
			ClearStamp( m_pRoot );								// 2013.03.06.수빈.

		i3::string_ncopy_nullpad( m_szFileName, pszFileName, MAX_PATH - 1);

		break;
	case REGISTRY_FILE_XML_BY_UTF8:
	case REGISTRY_FILE_XML_BY_EUC_KR:
		{
			bool ret = false;
			TiXmlDocument doc(pszFileName);

			if( nFileType == REGISTRY_FILE_XML_BY_UTF8)
				ret = doc.LoadFile(TIXML_ENCODING_UTF8);
			else if( nFileType == REGISTRY_FILE_XML_BY_EUC_KR)
				ret = doc.LoadFile();

			if(ret)
			{
				TiXmlElement * pXMLRoot = doc.RootElement();	//	Root
				TiXmlElement * pXMLHeader = nullptr;
				TiXmlElement * pXMLChild = pXMLRoot->FirstChildElement();

				//	Header node 를 찾는다.
				while(pXMLChild)
				{
					if( i3::generic_is_iequal( pXMLChild->Value(), "HEADER") )
					{
						pXMLHeader = pXMLChild;
						break;
					}

					pXMLChild = pXMLChild->NextSiblingElement();
				}

				if( pXMLHeader != nullptr)
				{
					//	파일이 맞는지 헤더 부분을 체크합니다.
					if( _XML_CheckHeaderNode(pXMLHeader))
					{
						//	Root RegKey
						i3RegKey * pRootKey = _XML_ConvertXMLToReg(pXMLRoot);

						setRoot( pRootKey);

						nReadBytes = doc.GetSize();
					}
				}
			}
		}
		if (!i3Registry::Get_LoadStamp())						// i3ResourceFile소멸 보다 늦게 처리하면 TRACE경고(False Positive)를 제거할수 있는것으로 보인다.
			ClearStamp(m_pRoot);								// 2013.03.06.수빈.
		break;
	}

	_ReleaseBuffer();

	return nReadBytes;
}

i3RegKey * i3RegistrySet::_XML_ConvertXMLToReg( TiXmlElement * pNode)
{
	i3RegKey * pRegKey = i3RegKey::new_object_ref();
	TiXmlElement * pXMLChild = pNode->FirstChildElement();

	_XML_SetRegKeyNameFromXML( pRegKey, pNode);

	if( i3::generic_is_iequal(pNode->Value(), "ROOT") )
	{
		pRegKey->setNodeStyle( I3REG_STYLE_ROOT);
	}

	while(pXMLChild)
	{
		//	Child
		if( i3::contain_string_head(pXMLChild->Value() , "KEY") )
		{
			i3RegKey * pChildKey = _XML_ConvertXMLToReg(pXMLChild);

			if( pChildKey != nullptr)
				pRegKey->AddChild( pChildKey);
		}
		//	Data
		else if( (i3::contain_string_head(pXMLChild->Value(), _GetRegStyleName(I3REG_TYPE_INT32)) ) ||
			(i3::contain_string_head(pXMLChild->Value(), _GetRegStyleName(I3REG_TYPE_REAL32)) ) ||
			(i3::contain_string_head(pXMLChild->Value(), _GetRegStyleName(I3REG_TYPE_STRING)) ) ||
			(i3::contain_string_head(pXMLChild->Value(), _GetRegStyleName(I3REG_TYPE_VEC2D)) ) ||
			(i3::contain_string_head(pXMLChild->Value(), _GetRegStyleName(I3REG_TYPE_VEC3D)) ) ||
			(i3::contain_string_head(pXMLChild->Value(), _GetRegStyleName(I3REG_TYPE_VEC4D)) ) ||
			(i3::contain_string_head(pXMLChild->Value(), _GetRegStyleName(I3REG_TYPE_COLOR)) ) ||
			(i3::contain_string_head(pXMLChild->Value(), _GetRegStyleName(I3REG_TYPE_MATRIX)) ) ||
			(i3::contain_string_head(pXMLChild->Value(), _GetRegStyleName(I3REG_TYPE_INT32EX)) ) )
		{
			//	자식 노드가 있으면 재귀호출
			i3RegData * pRegData = _XML_ConvertXMLToData(pXMLChild);
		
// 			if (strcmp(pRegData->GetName(), "__Update Time") == 0 || strcmp(pRegData->GetName(), "__Make Time") == 0)
// 			{
// 				I3_MUST_RELEASE(pRegData);
// 				pXMLChild = pXMLChild->NextSiblingElement();
// 				continue;
// 			}
			if( pRegData != nullptr && !i3Registry::IsStampString(pRegData->GetName()))
				pRegKey->AddData(pRegData);
			else if(pRegData != nullptr )
				pRegKey->PushStemValue(pRegData);
		}

		pXMLChild = pXMLChild->NextSiblingElement();
	}

	return pRegKey;
}

i3RegData * i3RegistrySet::_XML_ConvertXMLToData(TiXmlElement * pNode)
{
	i3RegData * pRegData = nullptr;

	//단일, 다중 판단
	UINT32 Count = 0;
	TiXmlAttribute* pAttribute = (TiXmlAttribute*)pNode->FirstAttribute()->Next();
	for (TiXmlAttribute* pAtt = pAttribute; pAtt; pAtt = pAtt->Next())
		++Count;

	bool bSpecificNation = (i3Registry::Get_LocationIndex() != 0);

	//	I3REG_TYPE_INT32
	if (i3::contain_string_head(pNode->Value(), _GetRegStyleName(I3REG_TYPE_INT32)))
	{
		if (Count == 1)
		{
			i3RegINT32 * pRegINT32 = i3RegINT32::new_object();
			_XML_SetRegDataNameFromXML(pRegINT32, pNode);

			INT32 value = 0;
			const char* Str = pAttribute->Name();
			pNode->Attribute(Str, &value);
			pRegINT32->setValue(value);

			pRegData = pRegINT32;
		}
		else
		{
			i3RegArray * pRegArrayData = i3RegArray::new_object();

			pRegArrayData->setElementType(I3REG_TYPE_INT32);
			_XML_SetRegDataNameFromXML(pRegArrayData, pNode);

			INT32 nLocationCount = 0;
			for (; pAttribute != nullptr; pAttribute = pAttribute->Next())
			{
				if (nLocationCount == i3Registry::Get_LocationIndex() || !bSpecificNation)
				{
					i3RegINT32* pRegData = (i3RegINT32*)pRegArrayData->createRegData();
					pRegArrayData->addRegData(pRegData);

					_XML_SetRegDataNameFromXML(pRegData, pAttribute);

					INT32 value = 0;
					const char* Str = pAttribute->Name();
					pNode->Attribute(Str, &value);
					pRegData->setValue(value);
					if (bSpecificNation)	//국가가 있으면 루프에서 나온다.
						break;
				}
				nLocationCount++;
			}

			pRegData = pRegArrayData;
		}
	}
	//	I3REG_TYPE_REAL32
	else if (i3::contain_string_head(pNode->Value(), _GetRegStyleName(I3REG_TYPE_REAL32)))
	{
		if (Count == 1)
		{
			i3RegREAL32 * pRegREAL32 = i3RegREAL32::new_object();
			_XML_SetRegDataNameFromXML(pRegREAL32, pNode);

			double value = 0.0f;
			const char* Str = pAttribute->Name();
			pNode->Attribute(Str, &value);
			pRegREAL32->setValue((REAL32)value);

			pRegData = pRegREAL32;
		}
		else
		{
			i3RegArray * pRegArrayData = i3RegArray::new_object();

			pRegArrayData->setElementType(I3REG_TYPE_REAL32);
			_XML_SetRegDataNameFromXML(pRegArrayData, pNode);

			INT32 nLocationCount = 0;
			for (pAttribute; pAttribute; pAttribute = pAttribute->Next())
			{
				if (nLocationCount == i3Registry::Get_LocationIndex() || !bSpecificNation)
				{
					i3RegREAL32* pRegData = (i3RegREAL32*)pRegArrayData->createRegData();
					pRegArrayData->addRegData(pRegData);

					_XML_SetRegDataNameFromXML(pRegData, pAttribute);

					double value = 0.f;
					const char* Str = pAttribute->Name();
					pNode->Attribute(Str, &value);
					pRegData->setValue((REAL32)value);
					if (bSpecificNation)	//국가가 있으면 루프에서 나온다.
						break;
				}
				nLocationCount++;
			}

			pRegData = pRegArrayData;
		}
	}
	//	I3REG_TYPE_STRING
	else if (i3::contain_string_head(pNode->Value(), _GetRegStyleName(I3REG_TYPE_STRING)))
	{
		if (Count == 1)
		{
			i3RegString * pRegString = i3RegString::new_object();

			_XML_SetRegDataNameFromXML(pRegString, pNode);
			_XML_SetRegElementStringFromXML(pRegString, pNode, pAttribute);

			pRegData = pRegString;
		}
		else
		{
			i3RegArray * pRegArrayData = i3RegArray::new_object();

			pRegArrayData->setElementType(I3REG_TYPE_STRING);
			_XML_SetRegDataNameFromXML(pRegArrayData, pNode);

			INT32 nLocationCount = 0;
			while (pAttribute != nullptr)
			{
				if (nLocationCount == i3Registry::Get_LocationIndex() || !bSpecificNation)
				{
					i3RegString* pRegData = (i3RegString*)pRegArrayData->createRegData();
					pRegArrayData->addRegData(pRegData);

					_XML_SetRegElementStringFromXML(pRegData, pNode, pAttribute);
					if (bSpecificNation)	//국가가 있으면 루프에서 나온다.
						break;
				}
				nLocationCount++;
				pAttribute = pAttribute->Next();
			}

			pRegData = pRegArrayData;
		}
	}
	//	I3REG_TYPE_VEC2D
	else if (i3::contain_string_head(pNode->Value(), _GetRegStyleName(I3REG_TYPE_VEC2D)))
	{
		i3RegVEC2D * pRegVEC2D = nullptr;
		i3RegArray * pRegArrayData = nullptr;

		if (Count == 2)
		{
			pRegVEC2D = i3RegVEC2D::new_object();
			_XML_SetRegDataNameFromXML(pRegVEC2D, pNode);
		}
		else
		{
			pRegArrayData = i3RegArray::new_object();
			pRegArrayData->setElementType(I3REG_TYPE_VEC2D);
			_XML_SetRegDataNameFromXML(pRegArrayData, pNode);
		}

		int Index = 0;
		VEC2D value;
		double Data[2];	//x,y

		INT32 nLocationCount = 0;
		for (pAttribute; pAttribute; pAttribute = pAttribute->Next())
		{
			const char* Str = pAttribute->Name();
			pNode->Attribute(Str, &Data[Index]);

			if (Index == 1)
			{
				if (Count == 2)
				{
					value.x = (REAL32)Data[0];
					value.y = (REAL32)Data[1];

					pRegVEC2D->setValue(&value);
				}
				else
				{
					if (nLocationCount == i3Registry::Get_LocationIndex() || !bSpecificNation)
					{
						i3RegVEC2D* pRegData = (i3RegVEC2D*)pRegArrayData->createRegData();
						pRegArrayData->addRegData(pRegData);

						value.x = (REAL32)Data[0];
						value.y = (REAL32)Data[1];

						pRegData->setValue(&value);
						if (bSpecificNation)	//국가가 있으면 루프에서 나온다.
							break;
					}
					nLocationCount++;
				}

				Index = 0;
			}
			else
				Index++;
		}

		if (Count == 2)
		{
			pRegData = pRegVEC2D;
		}
		else
		{
			pRegData = pRegArrayData;
		}
	}
	//	I3REG_TYPE_VEC3D
	else if (i3::contain_string_head(pNode->Value(), _GetRegStyleName(I3REG_TYPE_VEC3D)))
	{
		i3RegVEC3D * pRegVEC3D = nullptr;
		i3RegArray * pRegArrayData = nullptr;

		if (Count == 3)
		{
			pRegVEC3D = i3RegVEC3D::new_object();
			_XML_SetRegDataNameFromXML(pRegVEC3D, pNode);
		}
		else
		{
			pRegArrayData = i3RegArray::new_object();
			pRegArrayData->setElementType(I3REG_TYPE_VEC3D);
			_XML_SetRegDataNameFromXML(pRegArrayData, pNode);
		}

		int Index = 0;
		VEC3D value;
		double Data[3];	//x,y,z
		INT32 nLocationCount = 0;

		for (pAttribute; pAttribute; pAttribute = pAttribute->Next())
		{
			const char* Str = pAttribute->Name();
			pNode->Attribute(Str, &Data[Index]);

			if (Index == 2)
			{
				if (Count == 3)
				{
					value.x = (REAL32)Data[0];
					value.y = (REAL32)Data[1];
					value.z = (REAL32)Data[2];

					pRegVEC3D->setValue(&value);
				}
				else
				{
					if (nLocationCount == i3Registry::Get_LocationIndex() || !bSpecificNation)
					{
						i3RegVEC3D* pRegData = (i3RegVEC3D*)pRegArrayData->createRegData();
						pRegArrayData->addRegData(pRegData);

						value.x = (REAL32)Data[0];
						value.y = (REAL32)Data[1];
						value.z = (REAL32)Data[2];

						pRegData->setValue(&value);
						if (bSpecificNation)	//국가가 있으면 루프에서 나온다.
							break;
					}
					nLocationCount++;
				}

				Index = 0;
			}
			else
				Index++;
		}

		if (Count == 3)
		{
			pRegData = pRegVEC3D;
		}
		else
		{
			pRegData = pRegArrayData;
		}
	}
	//	I3REG_TYPE_VEC4D
	else if (i3::contain_string_head(pNode->Value(), _GetRegStyleName(I3REG_TYPE_VEC4D)))
	{
		i3RegVEC4D * pRegVEC4D = nullptr;
		i3RegArray * pRegArrayData = nullptr;

		if (Count == 4)
		{
			pRegVEC4D = i3RegVEC4D::new_object();
			_XML_SetRegDataNameFromXML(pRegVEC4D, pNode);
		}
		else
		{
			pRegArrayData = i3RegArray::new_object();
			pRegArrayData->setElementType(I3REG_TYPE_VEC4D);
			_XML_SetRegDataNameFromXML(pRegArrayData, pNode);
		}

		int Index = 0;
		VEC4D value;
		double Data[4];	//x,y,z,w
		INT32 nLocationCount = 0;

		for (pAttribute; pAttribute; pAttribute = pAttribute->Next())
		{
			const char* Str = pAttribute->Name();
			pNode->Attribute(Str, &Data[Index]);

			if (Index == 3)
			{
				if (Count == 4)
				{
					value.x = (REAL32)Data[0];
					value.y = (REAL32)Data[1];
					value.z = (REAL32)Data[2];
					value.w = (REAL32)Data[3];

					pRegVEC4D->setValue(&value);
				}
				else
				{
					if (nLocationCount == i3Registry::Get_LocationIndex() || !bSpecificNation)
					{
						i3RegVEC4D* pRegData = (i3RegVEC4D*)pRegArrayData->createRegData();
						pRegArrayData->addRegData(pRegData);

						value.x = (REAL32)Data[0];
						value.y = (REAL32)Data[1];
						value.z = (REAL32)Data[2];
						value.w = (REAL32)Data[3];

						pRegData->setValue(&value);
						if (bSpecificNation)	//국가가 있으면 루프에서 나온다.
							break;
					}
					nLocationCount++;
				}

				Index = 0;
			}
			else
				Index++;
		}

		if (Count == 4)
		{
			pRegData = pRegVEC4D;
		}
		else
		{
			pRegData = pRegArrayData;
		}
	}
	//	I3REG_TYPE_COLOR
	else if (i3::contain_string_head(pNode->Value(), _GetRegStyleName(I3REG_TYPE_COLOR)))
	{
		i3RegCOLOR * pRegCOLOR = nullptr;
		i3RegArray * pRegArrayData = nullptr;

		if (Count == 4)
		{
			pRegCOLOR = i3RegCOLOR::new_object();
			_XML_SetRegDataNameFromXML(pRegCOLOR, pNode);
		}
		else
		{
			pRegArrayData = i3RegArray::new_object();
			pRegArrayData->setElementType(I3REG_TYPE_COLOR);
			_XML_SetRegDataNameFromXML(pRegArrayData, pNode);
		}

		int Index = 0;
		I3COLOR value;
		INT32 Data[4];	//r,g,b,a
		INT32 nLocationCount = 0;

		for (; pAttribute != nullptr; pAttribute = pAttribute->Next())
		{
			const char* Str = pAttribute->Name();
			pNode->Attribute(Str, &Data[Index]);

			if (Index == 3)
			{
				if (Count == 4)
				{
					MIN(Data[0], 0);	MAX(Data[0], 255);
					MIN(Data[1], 0);	MAX(Data[1], 255);
					MIN(Data[2], 0);	MAX(Data[2], 255);
					MIN(Data[3], 0);	MAX(Data[3], 255);

					i3Color::Set(&value, (UINT8)Data[0], (UINT8)Data[1], (UINT8)Data[2], (UINT8)Data[3]);

					pRegCOLOR->setValue(&value);
				}
				else
				{
					if (nLocationCount == i3Registry::Get_LocationIndex() || !bSpecificNation)
					{
						i3RegCOLOR* pRegData = (i3RegCOLOR*)pRegArrayData->createRegData();
						pRegArrayData->addRegData(pRegData);

						MIN(Data[0], 0);	MAX(Data[0], 255);
						MIN(Data[1], 0);	MAX(Data[1], 255);
						MIN(Data[2], 0);	MAX(Data[2], 255);
						MIN(Data[3], 0);	MAX(Data[3], 255);

						i3Color::Set(&value, (UINT8)Data[0], (UINT8)Data[1], (UINT8)Data[2], (UINT8)Data[3]);

						pRegData->setValue(&value);

						if (bSpecificNation)	//국가가 있으면 루프에서 나온다.
							break;
					}
					nLocationCount++;
				}

				Index = 0;
			}
			else
				Index++;
		}

		if (Count == 4)
		{
			pRegData = pRegCOLOR;
		}
		else
		{
			pRegData = pRegArrayData;
		}
	}
	//	I3REG_TYPE_MATRIX
	else if (i3::contain_string_head(pNode->Value(), _GetRegStyleName(I3REG_TYPE_MATRIX)))
	{
		if (Count == 1)
		{
			i3RegINT32 * pRegINT32 = i3RegINT32::new_object();
			_XML_SetRegDataNameFromXML(pRegINT32, pNode);

			INT32 value = 0;
			const char* Str = pAttribute->Name();
			pNode->Attribute(Str, &value);
			pRegINT32->setValue(value);

			pRegData = pRegINT32;
		}
		else
		{
			double v = 0;
			MATRIX mtx;
			char temp[256] = "";
			i3RegMATRIX * pRegMATRIX = i3RegMATRIX::new_object();

			_XML_SetRegDataNameFromXML(pRegMATRIX, pNode);

			for (INT32 i = 0; i < 4; i++)
			{
				for (INT32 j = 0; j < 4; j++)
				{
					sprintf(temp, "m%d%d", i, j);
					pNode->Attribute(temp, &v);

					mtx.m[i][j] = (REAL32)v;
				}
			}

			i3Matrix::Copy(pRegMATRIX->getValue(), &mtx);

			pRegData = pRegMATRIX;
		}
	}
	return pRegData;
}
void i3RegistrySet::_UTF8_MultiByte(char* UTF8, size_t sizeOfUTF8, const char* MultiByte)
{
	memset(UTF8, 0, sizeOfUTF8);

	if(MultiByte != nullptr)
	{
		char szBuffer[4096] = "";

		INT32 len = i3::generic_string_size( MultiByte );

		if( len > 0)
		{
			if( m_nFileType == REGISTRY_FILE_XML_BY_UTF8)
			{
				_ConvertUTF8ToMultiByte( m_nCodePage, MultiByte, len, szBuffer, sizeof(szBuffer) );		//	UTF-8 -> MutiByte
			}
			else
			{
				i3::safe_string_copy( szBuffer, MultiByte, 4096);
			}

			strcpy_s(UTF8, sizeOfUTF8, szBuffer);
		}
	}
}

void i3RegistrySet::_MultiByte_UTF8(char* MultiByte, size_t sizeOfMultiByte, const char* UTF8)
{
	memset(MultiByte, 0, sizeOfMultiByte);

	if(UTF8 != nullptr)
	{
		char szBuffer[4096] = "";

		INT32 len = i3::generic_string_size( UTF8 );

		if( len > 0)
		{
			if( m_nFileType == REGISTRY_FILE_XML_BY_UTF8)
			{
				_ConvertMultiByteToUTF8( m_nCodePage, UTF8, len, szBuffer, sizeof(szBuffer) );		//	MultiByte -> UTF-8
			}
			else
			{
				i3::safe_string_copy( szBuffer, UTF8, 4096);
			}

			strcpy_s(MultiByte, sizeOfMultiByte, szBuffer);
		}
	}
}


void i3RegistrySet::_XML_SetRegKeyNameFromXML( i3RegKey * pDest, TiXmlElement * pSrc)
{
	I3ASSERT( pDest != nullptr && pSrc != nullptr);

	char szBuffer[2048] = "";

	_UTF8_MultiByte(szBuffer, 2048, pSrc->Attribute( "Name" ) );

	pDest->SetName( szBuffer );
}

void i3RegistrySet::_XML_SetRegDataNameFromXML( i3RegData * pDest, TiXmlElement * pSrc)
{
	I3ASSERT( pDest != nullptr && pSrc != nullptr);

	char szBuffer[2048] = "";
	
	_UTF8_MultiByte(szBuffer, 2048, pSrc->Attribute( "Name" ) );

	pDest->SetName( szBuffer );
}

void i3RegistrySet::_XML_SetRegDataNameFromXML( i3RegData * pDest, TiXmlAttribute * pSrc)
{
	I3ASSERT( pDest != nullptr && pSrc != nullptr);

	char szBuffer[2048] = "";
	
	_UTF8_MultiByte(szBuffer, 2048, pSrc->Name() );

	pDest->SetName( szBuffer );
}

void i3RegistrySet::_XML_SetRegElementStringFromXML( i3RegString * pDest, TiXmlElement * pSrc, TiXmlAttribute * pAttribute)
{
	I3ASSERT( pDest != nullptr && pSrc != nullptr && pAttribute != nullptr);

//	char szBuffer[4096] = "";
	i3::stack_wstring	wstrBuff;
	const char * pszName = pSrc->Attribute( pAttribute->Name() );
	i3::utf8_to_utf16(pszName, wstrBuff);
	i3::generic_string_replace_all(wstrBuff, L"\\n", L"\n");			
//	_UTF8_MultiByte(szBuffer, 4096, pszName );
//	i3String::Replace( szBuffer, 4096, "\\n", "\n");
	pDest->setValue( wstrBuff );

}


void i3RegistrySet::_XML_SetXMLNameFromRegKey( TiXmlElement * pDest, i3RegKey * pSrc)
{
	I3ASSERT( pDest != nullptr && pSrc != nullptr);

	char szBuffer[2048] = "";

	_MultiByte_UTF8(szBuffer, 2048, pSrc->GetName() );

	pDest->SetAttribute("Name", szBuffer);
}

void i3RegistrySet::_XML_SetXMLNameFromRegData( TiXmlElement * pDest, i3RegData * pSrc)
{
	I3ASSERT( pDest != nullptr && pSrc != nullptr);

	char szBuffer[2048] = "";

	_MultiByte_UTF8(szBuffer, 2048, pSrc->GetName() );

	pDest->SetAttribute("Name", szBuffer);
}

void i3RegistrySet::_XML_SetXMLValueFromRegData( TiXmlElement * pDest, i3RegData * pSrc, const char* szName)
{
	I3ASSERT( pDest != nullptr && pSrc != nullptr);

	i3::rc_wstring wstrVal;

	pSrc->GetValue( wstrVal );

	i3::stack_wstring wstrStackVal = wstrVal;

	i3::generic_string_replace_all(wstrStackVal, L"\n", L"\\n");
//
	i3::string utf8Val;
	
	i3::utf16_to_utf8( wstrStackVal, utf8Val);

	pDest->SetAttribute(szName, utf8Val.c_str() );
}


bool	i3RegistrySet::ExportStringFile( const char * FileName)
{
	i3FileStream file;
	if( !file.Create( FileName, STREAM_WRITE))
	{
		return false;
	}

	i3::vector<i3::rc_wstring>  StringList;

	//	저장용 문자열 프로퍼티 데이터 뽑아내기
	{
		_RecursiveChild( getRoot(), StringList);
	}

	//	파일 저장 (유니코드)

	//	ByteOrderMark
	WCHAR16 bom = 0xFEFF;
//	WCHAR16 szWriteLine[2048] = L"";

	file.Write( &bom, sizeof( WCHAR16));

	file.SetPosition( 0, STREAM_END);

	for(size_t i=0; i<StringList.size(); i++)
	{
		const wchar_t * pData = StringList[i].c_str();

	//	INT32 strLen = i3::generic_string_size(pData);
	//	INT32 wstrLen = i3String::MultiByteToWideChar(m_nCodePage, 0, (char*)pData, strLen, nullptr, 0);
	//	I3ASSERT( wstrLen < 2048);

	//	i3mem::FillZero( szWriteLine, sizeof( szWriteLine));
	//	i3String::MultiByteToWideChar( m_nCodePage, 0, pData, strLen, szWriteLine, wstrLen);

		file.WriteLineW(pData);
	}

	file.Close();

	StringList.clear();

	return true;
}


bool i3RegistrySet::_IsValidRegKey(char * pBuf)
{
	//	체크 형식: [이름1\이름2\이름3\...\이름n]

	char * pStr = pBuf;

	if( *pStr == '[')
	{
		INT32 len = i3::generic_string_size( pBuf);

		if( *(pStr + (len-1)) == ']')
			return true;
	}

	return false;
}

bool i3RegistrySet::_IsValidRegData(char * pBuf)
{
	//	체크 형식: "이름" = "값"

	INT32 nStep = 0;
	char * pStr = pBuf;
	while( *pStr)
	{
		switch( nStep)
		{
		case 0:		if( *pStr == '\"') nStep = 1;	break;//	첫번째 double quotation marks
		case 1:		if( *pStr == '\"') nStep = 2;	break;//	두번째 double quotation marks
		case 2:		if( *pStr == '=')  nStep = 3;	break;//	equal marks
		case 3:		if( *pStr == '\"') nStep = 4;	break;//	세번째 double quotation marks
		case 4:		if( *pStr == '\"') nStep = -1;	break;//	네번째 double quotation marks
		}

		pStr++;
	}

	if( nStep == -1)		//	마지막 체크 완료
		return true;

	return false;
}

char * i3RegistrySet::_Parse_FindRegKey( char * pBuf, i3RegKey* pSrc, i3RegKey ** pOut)
{
	char szKey[256] = "";
	INT32 idx = 0;
	i3RegKey * pMid = pSrc;

	while( *pBuf)
	{
		switch( *pBuf)
		{
		case '[':		//	open hierarchy node
			{
				*pOut = nullptr;
			}
			break;
		case ']':		//	close hierarchy node
			{
				if(pMid == nullptr) {
					*pOut = nullptr;
				}
				else {
					*pOut = (i3RegKey *)i3TreeNode::FindNodeByName( pMid, szKey);
				}
				return pBuf;
			}
			break;
		case '\\':		//	seperate hierarchy node
			{
				pMid = (i3RegKey *)i3TreeNode::FindNodeByName( pMid, szKey);

				idx = 0;		//	초기화
				szKey[0] = 0;
			}
			break;
		default:
			{
				szKey[idx++] = *pBuf;
				szKey[idx] = 0;

				I3ASSERT( idx < 256);
			}
			break;
		}

		pBuf++;
	}

	*pOut = nullptr;
	return pBuf;
}

char * i3RegistrySet::_Parse_FindDataName( char * pBuf, i3RegKey* pSrc, i3RegData ** pOut)
{
	INT32 idx = 0;
	bool bOpen = false;
	char szTemp[4096] = "";

	while( *pBuf)
	{
		switch( *pBuf)
		{
		case '\"':		//	open or close hierarchy node
			{
				*pOut = nullptr;
				bOpen = !bOpen;

				//	i3RegData를 찾는다.
				if( !bOpen)	//	따옴표가 닫혔다면
				{
					szTemp[idx] = 0;

					*pOut = pSrc->FindDataByName( szTemp);
					if ( *pOut == nullptr )
					{
						i3RegString * pRegStr = i3RegString::new_object();
						pRegStr->SetName(szTemp);
						pSrc->AddData(pRegStr);
						*pOut = pRegStr;
					}

					return pBuf;
				}
			}
			break;
		case '=':		//	terminate
			{
				if( !bOpen)	//	따옴표가 닫혔다면
				{
					szTemp[idx] = 0;

					return ++pBuf;
				}
			}
			break;
		default:
			{
				if( bOpen)
				{
					szTemp[idx++] = *pBuf;
					szTemp[idx] = 0;

					I3ASSERT( idx < 256);
				}
			}
		}

		pBuf++;
	}

	*pOut = nullptr;
	return pBuf;
}

char * i3RegistrySet::_Parse_FindDataString( char * pBuf, i3RegData* pSrc, char ** pOut, INT32 nLength)
{
	INT32 idx = 0;
	bool bOpen = false;
	char * pStr = *pOut;

	while( *pBuf)
	{
		switch( *pBuf)
		{
		case '\"':		//	open or close hierarchy node
			{
				if( idx > 0)
				{
					if( *(pBuf-1) == '\\')
					{
						pStr[idx++] = *pBuf;
						pStr[idx] = 0;

						I3ASSERT( idx < nLength);
					}
				}

				*pOut = nullptr;
				bOpen = !bOpen;

				if( !bOpen)	//	따옴표가 닫혔다면
				{
					pStr[idx] = 0;

					return ++pBuf;
				}
			}
			break;
		default:
			{
				if( bOpen)	//	따옴표가 열렸다면
				{
					pStr[idx++] = *pBuf;
					pStr[idx] = 0;

					I3ASSERT( idx < nLength);
				}
			}
		}

		pBuf++;
	}

	*pOut = nullptr;
	return pBuf;
}




void i3RegistrySet::_RecursiveChild(i3RegKey* pKey, i3::vector<i3::rc_wstring>& Out)
{
	wchar_t wszTemp[4096] = L"";

	i3::vector<i3RegKey*> HierarchyList;

	for( INT32 i=0; i<pKey->GetDataCount(); i++)
	{
		i3RegData * pData = pKey->GetData( i);

		//	스트링 문자열만 뽑아내도록
		if(pData->getDataType() == I3REG_TYPE_STRING)
		{
			i3RegString * pStringData = (i3RegString*) pData;

			//	가장 먼저 계층 부모 이름을 먼저 저장한다.
			if( HierarchyList.size() == 0)
			{
				wchar_t wszHierarchyName[1024] = L"";

				i3RegKey * pParent = (i3RegKey*) pKey->getParent();
				while( pParent)
				{
					if( pParent->getNodeStyle() & I3REG_STYLE_ROOT)
						break;

					HierarchyList.insert(HierarchyList.begin(), pParent);	//	상위 부모일수록 앞에 저장

					pParent = (i3RegKey*) pParent->getParent();
				}

				HierarchyList.push_back( pKey);	//	마지막은 자신

				//	부모와 연관하여 노드 이름을 만든다.
				i3::safe_string_copy(wszHierarchyName, L"[", 1024);
				for(size_t i=0; i<HierarchyList.size(); i++)
				{
					i3RegKey * pItem = HierarchyList[i];

					if( i > 0)
						i3::generic_string_cat( wszHierarchyName, L"\\");

					i3::stack_wstring wstrItemName;	i3::mb_to_utf16(pItem->GetNameString(), wstrItemName);

					i3::generic_string_cat( wszHierarchyName, wstrItemName);
				}

				i3::generic_string_cat( wszHierarchyName, L"]\r\n");

				Out.push_back( wszHierarchyName);
			}

			//	데이터 필드 이름
			i3::safe_string_copy( wszTemp, L"\"", 4096);

			i3::stack_wstring wstrDataName;		i3::mb_to_utf16(pData->GetNameString(), wstrDataName);

			i3::generic_string_cat( wszTemp, wstrDataName);
			i3::generic_string_cat( wszTemp, L"\" = ");

			//	데이터 스트링
			i3::generic_string_cat( wszTemp, L"\"");

			//	문자 데이터중에 아스키 '\n'가 있으면 문자형으로 '\\n'으로 변환해야 텍스트로 보인다.
		//	char tmp[4096] = "";
			i3::stack_wstring	tmp;

			i3::generic_string_copy( tmp, pStringData->getValue() );

			//i3String::Replace( tmp, sizeof(tmp), "\n", "\\n");
			i3::generic_string_replace_all(tmp, L"\n", L"\\n");
			// //i3String::Replace( tmp, sizeof(tmp), "\"", "\\\"");

			i3::generic_string_cat( wszTemp, tmp);
			i3::generic_string_cat( wszTemp, L"\"\r\n");

			Out.push_back( wszTemp);

			i3mem::FillZero( wszTemp, sizeof( wszTemp));
		}
	}

	if( HierarchyList.size() > 0)
	{
		Out.push_back( L"\r\n");

		HierarchyList.clear();
	}

	//	자식도 한번씩 처리
	for( INT32 i=0; i<pKey->getChildCount(); i++)
	{
		i3RegKey * pChild = (i3RegKey*) pKey->getChild(i);

		_RecursiveChild( pChild, Out);
	}
}