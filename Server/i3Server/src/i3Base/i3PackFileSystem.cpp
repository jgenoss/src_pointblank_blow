#include "i3CommonType.h"
#include "i3PackFileSystem.h"
#include "i3String.h"

static i3List			* s_VolList = NULL;

INT32 i3Pack::comparebylatest( void * p1, void *p2)
{
	i3PackVolume* pvolum1 = (i3PackVolume*)p1;
	i3PackVolume* pvolum2 = (i3PackVolume*)p2;

	FILETIME filetime1, filetime2;

	i3mem::FillZero( &filetime1, sizeof(FILETIME));
	i3mem::FillZero( &filetime2, sizeof(FILETIME));

	i3mem::Copy( &filetime1, &pvolum1->getPreview()->header.pad[i3ResourceFile::LOW_DATA_TIME], sizeof(FILETIME));
	i3mem::Copy( &filetime2, &pvolum2->getPreview()->header.pad[i3ResourceFile::LOW_DATA_TIME], sizeof(FILETIME));
	
	LONG result = CompareFileTime( &filetime1, &filetime2);
	
	if( result > 0) return -1;
	
	if( result < 0) return 1;
	
	return 0;
}

I3_EXPORT_BASE	
void i3Pack::Sort(void)
{     
	s_VolList->Sort( comparebylatest);
}

I3_EXPORT_BASE
bool i3Pack::Init(void)
{
	if( s_VolList == NULL)
	{
		s_VolList = i3List::NewObject();
	}

	return true;
}

I3_EXPORT_BASE
bool i3Pack::Finish(void)
{
	if( s_VolList == NULL)
		return false;

	INT32 i;
	i3PackVolume * pVol;

	for( i = 0; i < s_VolList->GetCount(); i++)
	{
		pVol = (i3PackVolume *) s_VolList->Items[i];

		pVol->Release();
	}

	s_VolList->Clear();

	I3_SAFE_RELEASE( s_VolList);

	return true;
}

I3_EXPORT_BASE	
bool i3Pack::Preview( const char * pszPath)
{
	if( s_VolList == NULL)
		return false;

	i3PackVolume * pVol = i3PackVolume::NewObject();

	if( pVol->Preview( pszPath) == STREAM_ERR)
	{
		I3PRINTF( "[i3Pack] Could not Preview %s!!!!!!!!!!!!!\n", pszPath);
		pVol->Release();

		return false;
	}

	s_VolList->Add( pVol);

	return true;
}

I3_EXPORT_BASE
bool i3Pack::Bind( const char * pszPath)
{
	if( s_VolList == NULL)
		return false;

	i3PackVolume * pVol = i3PackVolume::NewObject();

	if( pVol->LoadFromFile( pszPath) == false)
	{
		I3PRINTF( "[i3Pack] Could not load %s!!!!!!!!!!!!!\n", pszPath);
		pVol->Release();

		return false;
	}

	s_VolList->Add( pVol);

	return true;
}

I3_EXPORT_BASE
bool i3Pack::Bind()
{
	if( s_VolList == NULL)
		return false;

	INT32 i;
	i3PackVolume * pVol;
	const char* pszPath = NULL;

	for( i = 0; i < s_VolList->GetCount(); i++)
	{
		pVol = (i3PackVolume *) s_VolList->Items[i];
		pszPath = pVol->getPreview()->filename;

		if( pVol->LoadFromFile( pszPath) == false)
		{
			I3PRINTF( "[i3Pack] Could not load %s!!!!!!!!!!!!!(Bind(void))\n", pszPath);

			return false;
		}	
	}

	return true;
}

I3_EXPORT_BASE
i3PackVolume * i3Pack::isFile( const char * pszPath)
{
	if( s_VolList == NULL)
		return NULL;

	INT32 i;
	i3PackVolume * pVol = NULL;
	I3_PACK_FILE_REF * pFileInfo = NULL;

	for( i = 0; i < s_VolList->GetCount(); i++)
	{
		pVol = (i3PackVolume *) s_VolList->Items[i];

		i3PackNode * pNode = pVol->FindPath( (char *) pszPath, &pFileInfo);

		if( pNode != NULL && (pFileInfo != NULL))
		{
			return pVol;
		}
	}

	return NULL;
}

I3_EXPORT_BASE
i3PackFile * i3Pack::Open( const char * pszPath)
{
	if( s_VolList == NULL)
		return NULL;

	INT32 i;
	i3PackVolume * pVol = NULL;
	i3PackNode * pPackNode = NULL;
	I3_PACK_FILE_REF * pFileInfo = NULL;

	for( i = 0; i < s_VolList->GetCount(); i++)
	{
		pVol = (i3PackVolume *) s_VolList->Items[i];

		i3PackNode * pNode = pVol->FindPath( (char *) pszPath, &pFileInfo);

		if( pNode != NULL && (pFileInfo != NULL))
		{
			pPackNode = pNode;
			break;
		}
	}

	if( pPackNode == NULL)
		return NULL;

	i3PackFile * pFile = i3PackFile::NewObjectRef();

	pFile->Open( pVol, pPackNode, pFileInfo);

	return pFile;
}

I3_EXPORT_BASE
void i3Pack::Close( i3PackFile * pFile)
{
	pFile->Release();
}

I3_EXPORT_BASE  
void i3Pack::DumpList()
{
	INT32 i;
	i3PackVolume* pVol;

	for( i = 0; i < s_VolList->GetCount(); ++i )
	{
		pVol = (i3PackVolume*)s_VolList->GetItem(i);

		I3TRACE( "Dump Pack File Name(%d) : %s  \n", i, pVol->getPreview()->filename);	

		pVol->DumpVolume();

		I3TRACE( "\n");
	}
}

I3_EXPORT_BASE  
i3PackNode*		i3Pack::FindDirectoryPackNode(const char* pszDir)
{
	if( s_VolList == NULL)
		return NULL;

	char szPath[MAX_PATH];
	i3String::Copy(szPath, pszDir, MAX_PATH);
	i3String::NormalizePath(szPath);	// 마지막 슬래시 제거..
	i3String::MakeUnixPath(szPath);		// '/' 치환..

	const INT32  nVol = s_VolList->GetCount();
	
	i3PackNode * pPackNode = NULL;
	I3_PACK_FILE_REF * pFileInfo = NULL;

	for(INT32 i = 0; i < nVol; ++i)
	{
		i3PackVolume* pVol = (i3PackVolume *) s_VolList->Items[i];

		i3PackNode * pNode = pVol->FindPath( (char *) szPath, &pFileInfo);

		if( pNode != NULL && (pFileInfo == NULL))		// 여기서 pFileInfo는 오히려 NULL값이어야 Directory를 확신할수 있음..
		{
			pPackNode = pNode;
			break;
		}
	}

	return pPackNode;
}