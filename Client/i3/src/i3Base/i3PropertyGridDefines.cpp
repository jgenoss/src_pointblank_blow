#include "i3CommonType.h"
#include "i3PropertyGridDefines.h"
#include "i3Memory.h"
#include "itl/vector.h"

//////////////////////////////////////////////////////////////////////
void	AddMFCProperty_BeginGroup( i3::vector<PROPERTYINFO*>& List, const char* pszGroupName, const char* pszDesc)
{
	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	
	pProp->Set( EPT_GROUP, nullptr, pszGroupName, pszDesc);

	List.push_back( pProp);
}

void	AddMFCProperty_EndGroup( i3::vector<PROPERTYINFO*>& List)
{
	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));

	pProp->Set( EPT_VOID, nullptr, nullptr);

	List.push_back( pProp);
}

void	AddMFCProperty_Bool( i3::vector<PROPERTYINFO*>& List, bool* pBool, const char* pszName, const char* pszDesc, bool bNoEdit)
{
	UINT32 nFlag = PROPERTYFLAG_NONE;
	if( bNoEdit)
		nFlag |= PROPERTYFLAG_NOEDIT;
	
	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));

	pProp->Set(	EPT_BOOLEAN, pBool, pszName, pszDesc, nFlag, 0);

	List.push_back( pProp);
}

void AddMFCProperty_UINT32BitFlag( i3::vector<PROPERTYINFO*>& List, UINT32* pData, UINT32 bitMask, 
												 const char* pszName, const char* pszDesc, bool bNoEdit)
{
	UINT32 nFlag = PROPERTYFLAG_NONE;
	if (bNoEdit)
		nFlag |= PROPERTYFLAG_NOEDIT;

	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	
	pProp->Set( EPT_UINT32_BITFLAG, pData, pszName, pszDesc, nFlag, reinterpret_cast<void*>(bitMask ) );
	
	List.push_back(pProp);
}


void	AddMFCProperty_Int32( 	i3::vector<PROPERTYINFO*>& List, 
								INT32* pInt, const char* pszName, const char* pszDesc, 
								INT32 nMin, INT32 nMax, bool bNoEdit)
{
	UINT32 nFlag = PROPERTYFLAG_NONE;
	INT32* pUserData = nullptr;
	
	if( nMax > nMin)
	{
		nFlag |= PROPERTYFLAG_HASRANGE;
		pUserData = (INT32*)i3MemAlloc( sizeof(INT32) * 2);		// delete는 i3TDKPropertyGridProperty_Int32::i3TDKPropertyGridProperty_Int32() 에서 한다.
		pUserData[0] = nMin;
		pUserData[1] = nMax;
	}

	if( bNoEdit )
		nFlag |= PROPERTYFLAG_NOEDIT;

	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	pProp->Set( EPT_INT32, pInt, pszName, pszDesc, nFlag, pUserData);	// delete는 i3TDKPropertyGridCtrl::AddProp() 에서 한다.

	List.push_back( pProp);
}

void	AddMFCProperty_Real32(	i3::vector<PROPERTYINFO*>& List, 
								REAL32* pReal, const char* pszName, const char* pszDesc, 
								REAL32 fMin, REAL32 fMax, bool bNoEdit)
{
	UINT32 nFlag = PROPERTYFLAG_NONE;
	REAL32* pUserData = nullptr;

	if( fMax > fMin)
	{
		nFlag |= PROPERTYFLAG_HASRANGE;
		pUserData = (REAL32*) i3MemAlloc( sizeof( REAL32) * 2);
		pUserData[0] = fMin;
		pUserData[1] = fMax;
	}

	if( bNoEdit)
		nFlag |= PROPERTYFLAG_NOEDIT;

	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	pProp->Set( EPT_REAL32, pReal, pszName, pszDesc, nFlag, pUserData);
	
	List.push_back( pProp);
}

void	AddMFCProperty_String(	i3::vector<PROPERTYINFO*>& List, 
								char* pString, UINT32 nBufSize, const char* pszName, const char* pszDesc, 
								bool bNoEdit)
{
	INT32* pBufSize = (INT32*)i3MemAlloc( sizeof( INT32));
	*pBufSize = nBufSize;

	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	pProp->Set( EPT_STRING, pString, pszName, pszDesc, 0, pBufSize);

	if( bNoEdit)
		pProp->m_nFlag |= PROPERTYFLAG_NOEDIT;
	
	List.push_back( pProp);
}

void	AddMFCProperty_StringW( i3::vector<PROPERTYINFO*>& List, 
								WCHAR16* pStringW, UINT32 nBufSize, const char* pszName, const char* pszDesc,
								bool bNoEdit)
{
	
	INT32* pBufSize = (INT32*)i3MemAlloc( sizeof( INT32));
	*pBufSize = nBufSize;

	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	pProp->Set( EPT_STRINGW, pStringW, pszName, pszDesc, 0, pBufSize);

	if( bNoEdit)
		pProp->m_nFlag |= PROPERTYFLAG_NOEDIT;
	
	List.push_back( pProp);
}


I3_EXPORT_BASE void	AddMFCProperty_RcString( i3::vector<PROPERTYINFO*>& List, 
											i3::rc_string& inout_data, const char* pszName, const char* pszDesc , 
											bool bNoEdit )
{
	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	pProp->Set( EPT_RCSTRING, &inout_data, pszName, pszDesc, 0);

	if( bNoEdit)
		pProp->m_nFlag |= PROPERTYFLAG_NOEDIT;

	List.push_back( pProp);
}

I3_EXPORT_BASE void	AddMFCProperty_RcStringW( i3::vector<PROPERTYINFO*>& List, 
											 i3::rc_wstring& inout_data, const char* pszName, const char* pszDesc ,
											 bool bNoEdit )
{
	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	pProp->Set( EPT_RCSTRINGW, &inout_data, pszName, pszDesc, 0);

	if( bNoEdit)
		pProp->m_nFlag |= PROPERTYFLAG_NOEDIT;

	List.push_back( pProp);
}


void	AddMFCProperty_Combo(	i3::vector<PROPERTYINFO*>& List, 
								const i3::vector<i3::rc_string>& StrList, INT32* pIdx, const char* pszName, const char* pszDesc, 
								bool bNoEdit)
{
	
	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	pProp->Set( EPT_COMBO, pIdx, pszName, pszDesc, PROPERTYFLAG_NONE, const_cast< i3::vector<i3::rc_string>*>(&StrList) );

	if( bNoEdit)
		pProp->m_nFlag |= PROPERTYFLAG_NOEDIT;

	List.push_back( pProp);
}

void	AddMFCProperty_File(	i3::vector<PROPERTYINFO*>& List, 
								char* pFileName, UINT32 nBufSize, const char* pszName, const char* pszDesc)
{
	
	void* pUserData = nullptr;	//can be used as a filter

	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	pProp->Set( EPT_FILE, pFileName, pszName, pszDesc, nBufSize, pUserData);

	List.push_back( pProp);
}

void	AddMFCProperty_File_RcString(i3::vector<PROPERTYINFO*>& List, 
									 i3::rc_string& inoutFileName, const char* pszName, const char* pszDesc)
{

	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	pProp->Set( EPT_FILE_RCSTRING, &inoutFileName, pszName, pszDesc, 0, nullptr);
	List.push_back( pProp);
}

void	AddMFCProperty_Button(	i3::vector<PROPERTYINFO*>& List, 
								PROC_BUTTONPROPERTY pProc, const char* pszName, char* pBtnName, const char* pszDesc, void * pUserData)
{
	
	UINT32 nFlag = PROPERTYFLAG_NONE;
	//void* pUserData = pProc;	//a function pointer

	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	pProp->Set( EPT_BUTTON, pBtnName, pszName, pszDesc, nFlag, pUserData, pProc);
	List.push_back( pProp);
}

void	AddMFCProperty_Image(	i3::vector<PROPERTYINFO*>& List, 
								char* pFileName, UINT32 nBufSize, const char* pszName, const char* pszDesc)
{
	
	void* pUserData = nullptr;		//can be used as a filter

	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	pProp->Set( EPT_IMAGE, pFileName, pszName, pszDesc, nBufSize, pUserData);

	List.push_back( pProp);
}

