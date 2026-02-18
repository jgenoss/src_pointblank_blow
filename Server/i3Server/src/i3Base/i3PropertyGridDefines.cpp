#include "i3CommonType.h"
#include "i3PropertyGridDefines.h"
#include "i3List.h"

//////////////////////////////////////////////////////////////////////
void	AddMFCProperty_BeginGroup( i3List* pList, const char* pszGroupName, const char* pszDesc)
{
	I3ASSERT( pList != NULL);

	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	
	pProp->Set( EPT_GROUP, NULL, pszGroupName, pszDesc);

	pList->Add( pProp);
}

void	AddMFCProperty_EndGroup( i3List* pList)
{
	I3ASSERT( pList != NULL);

	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));

	pProp->Set( EPT_VOID, NULL, NULL);

	pList->Add( pProp);
}

void	AddMFCProperty_Bool( i3List* pList, bool* pBool, const char* pszName, const char* pszDesc, bool bNoEdit)
{
	I3ASSERT( pList != NULL);

	UINT32 nFlag = PROPERTYFLAG_NONE;
	if( bNoEdit)
		nFlag |= PROPERTYFLAG_NOEDIT;
	
	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));

	pProp->Set(	EPT_BOOLEAN, pBool, pszName, pszDesc, nFlag, 0);

	pList->Add( pProp);
}

void	AddMFCProperty_Int32( 	i3List* pList, 
								INT32* pInt, const char* pszName, const char* pszDesc, 
								INT32 nMin, INT32 nMax, bool bNoEdit)
{
	I3ASSERT( pList != NULL);

	UINT32 nFlag = PROPERTYFLAG_NONE;
	INT32* pUserData = NULL;
	
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

	pList->Add( pProp);
}

void	AddMFCProperty_Real32(	i3List* pList, 
								REAL32* pReal, const char* pszName, const char* pszDesc, 
								REAL32 fMin, REAL32 fMax, bool bNoEdit)
{
	I3ASSERT( pList != NULL);

	UINT32 nFlag = PROPERTYFLAG_NONE;
	REAL32* pUserData = NULL;

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
	
	pList->Add( pProp);
}

void	AddMFCProperty_String(	i3List* pList, 
								char* pString, UINT32 nBufSize, const char* pszName, const char* pszDesc, 
								bool bNoEdit)
{
	I3ASSERT( pList != NULL);

	INT32* pBufSize = (INT32*)i3MemAlloc( sizeof( INT32));
	*pBufSize = nBufSize;

	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	pProp->Set( EPT_STRING, pString, pszName, pszDesc, 0, pBufSize);

	if( bNoEdit)
		pProp->m_nFlag |= PROPERTYFLAG_NOEDIT;
	
	pList->Add( pProp);
}

void	AddMFCProperty_StringW( i3List* pList, 
								WCHAR16* pStringW, UINT32 nBufSize, const char* pszName, const char* pszDesc,
								bool bNoEdit)
{
	I3ASSERT( pList != NULL);

	INT32* pBufSize = (INT32*)i3MemAlloc( sizeof( INT32));
	*pBufSize = nBufSize;

	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	pProp->Set( EPT_STRINGW, pStringW, pszName, pszDesc, 0, pBufSize);

	if( bNoEdit)
		pProp->m_nFlag |= PROPERTYFLAG_NOEDIT;
	
	pList->Add( pProp);
}


void	AddMFCProperty_Combo(	i3List* pList, 
								i3StringList* pStrList, INT32* pIdx, const char* pszName, const char* pszDesc, 
								bool bNoEdit)
{
	I3ASSERT( pList != NULL);

	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	pProp->Set( EPT_COMBO, pIdx, pszName, pszDesc, PROPERTYFLAG_NONE, pStrList);

	if( bNoEdit)
		pProp->m_nFlag |= PROPERTYFLAG_NOEDIT;

	pList->Add( pProp);
}

void	AddMFCProperty_File(	i3List* pList, 
								char* pFileName, UINT32 nBufSize, const char* pszName, const char* pszDesc)
{
	I3ASSERT( pList != NULL);

	void* pUserData = NULL;	//can be used as a filter

	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	pProp->Set( EPT_FILE, pFileName, pszName, pszDesc, nBufSize, pUserData);

	pList->Add( pProp);
}

void	AddMFCProperty_Button(	i3List* pList, 
								PROC_BUTTONPROPERTY pProc, const char* pszName, char* pBtnName, const char* pszDesc)
{
	I3ASSERT( pList != NULL);

	UINT32 nFlag = PROPERTYFLAG_NONE;
	void* pUserData = pProc;	//a function pointer

	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	pProp->Set( EPT_BUTTON, pBtnName, pszName, pszDesc, nFlag, pUserData);

	pList->Add( pProp);
}

void	AddMFCProperty_Image(	i3List* pList, 
								char* pFileName, UINT32 nBufSize, const char* pszName, const char* pszDesc)
{
	I3ASSERT( pList != NULL);

	void* pUserData = NULL;		//can be used as a filter

	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	pProp->Set( EPT_IMAGE, pFileName, pszName, pszDesc, nBufSize, pUserData);

	pList->Add( pProp);
}

