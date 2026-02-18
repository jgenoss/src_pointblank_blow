#include "i3CommonType.h"
#include "i3PackFileSystem.h"
#include "i3PackFile.h"
#include "i3String.h"
#include "i3Timer.h"

#include "itl/vector.h"
#include "itl/algorithm/sort.h"

#include "string/ext/remove_end_slash.h"
#include "string/ext/make_unix_path.h"
#include "i3Base/string/ext/safe_string_copy.h"

#include "i3PackFileFindMgr.h"

namespace i3Pack
{
	i3PackVolume *	isFile_Legacy(const char * pszPath);
	i3PackFile *	Open_Legacy(const char * pszPath);
}


static CALLBACK_UPDATEFUNC sPackUpdateFunc = nullptr;

namespace
{
	i3::vector<i3PackVolume*>	s_VolList;
}

namespace i3Pack
{
	bool	comparebylatest( i3PackVolume* p1, i3PackVolume* p2);
}

bool i3Pack::comparebylatest( i3PackVolume* p1, i3PackVolume* p2)
{
	i3PackVolume* pvolum1 = p1;
	i3PackVolume* pvolum2 = p2;

	FILETIME filetime1, filetime2;

	i3mem::FillZero( &filetime1, sizeof(FILETIME));
	i3mem::FillZero( &filetime2, sizeof(FILETIME));

	i3mem::Copy( &filetime1, &pvolum1->getPreview()->header.pad[i3ResourceFile::LOW_DATA_TIME], sizeof(FILETIME));
	i3mem::Copy( &filetime2, &pvolum2->getPreview()->header.pad[i3ResourceFile::LOW_DATA_TIME], sizeof(FILETIME));

	LONG result = CompareFileTime( &filetime1, &filetime2);

//	if( result > 0) return -1;

//	if( result < 0) return 1;

	return (result > 0);
}

I3_EXPORT_BASE	
void i3Pack::Sort(void)
{     
//	s_VolList->Sort( comparebylatest);
	i3::sort(s_VolList.begin(), s_VolList.end(), comparebylatest);
}

I3_EXPORT_BASE
bool i3Pack::Init(void)
{
	return true;
}

I3_EXPORT_BASE
bool i3Pack::Finish(void)
{
	i3PackFileFindMgr::i()->RemoveAll();

	for(size_t i = 0; i < s_VolList.size(); i++)
	{
		i3PackVolume * pVol = s_VolList[i]; 
		I3_MUST_RELEASE( pVol);
	}
	s_VolList.clear();

	return true;
}

I3_EXPORT_BASE	
bool i3Pack::Preview( const char * pszPath)
{
	i3PackVolume * pVol = i3PackVolume::new_object();

	if( pVol->Preview( pszPath) == STREAM_ERR)
	{
		I3PRINTF( "[i3Pack] Could not Preview %s!!!!!!!!!!!!!\n", pszPath);
		I3_MUST_RELEASE( pVol);

		return false;
	}

	s_VolList.push_back(pVol); 
	i3PackFileFindMgr::i()->AddPackVolume(pVol);

	return true;
}

I3_EXPORT_BASE
bool i3Pack::Bind( const char * pszPath)
{
	i3PackVolume * pVol = i3PackVolume::new_object();

	if( pVol->LoadFromFile( pszPath) == false)
	{
		I3PRINTF( "[i3Pack] Could not load %s!!!!!!!!!!!!!\n", pszPath);
		I3_MUST_RELEASE( pVol);

		return false;
	}

	s_VolList.push_back(pVol);
	i3PackFileFindMgr::i()->AddPackVolume(pVol);

	return true;
}

I3_EXPORT_BASE  
void	i3Pack::BindVolume( i3PackVolume* vol)
{
	if (!vol) return;
	vol->AddRef();
	s_VolList.push_back(vol);
	i3PackFileFindMgr::i()->AddPackVolume(vol);

}



I3_EXPORT_BASE
bool i3Pack::Bind()
{

	const char* pszPath = nullptr;

	for(size_t i = 0; i < s_VolList.size(); i++)
	{
		i3PackVolume * pVol = s_VolList[i]; 
		pszPath = pVol->getPreview()->filename.c_str();

		if( pVol->LoadFromFile( pszPath) == false)
		{
			I3PRINTF( "[i3Pack] Could not load %s!!!!!!!!!!!!!(Bind(void))\n", pszPath);

			return false;
		}	
	}

	return true;
}

I3_EXPORT_BASE
i3PackVolume * i3Pack::isFile(const i3::literal_range& strResPath)
{
	const i3PackFileFindEntity* pEntity = i3PackFileFindMgr::i()->FindEntity(strResPath);
	if (pEntity == NULL)
		return NULL;
	return pEntity->pPackVolume;
}

// 이 함수의 기존 구 코드를 남겨둡니다...(향후 쓰지는 않겠지만 참고는 할만하다...)
i3PackVolume * i3Pack::isFile_Legacy(const char * pszPath)
{
	I3_PACK_FILE_REF * pFileInfo = nullptr;

	for(size_t i = 0; i < s_VolList.size(); i++)
	{
		i3PackVolume * pVol = s_VolList[i];

		i3PackNode * pNode = pVol->FindPath((char *)pszPath, &pFileInfo);

		if( pNode != nullptr && (pFileInfo != nullptr))
		{
			return pVol;
		}
	}

	return nullptr;
}



I3_EXPORT_BASE
i3PackFile * i3Pack::Open(const i3::literal_range& strResPath)
{

	const i3PackFileFindEntity* pEntity = i3PackFileFindMgr::i()->FindEntity(strResPath);

	if (pEntity == nullptr)
		return nullptr;

	i3PackFile * pFile = i3PackFile::new_object_ref();

	pFile->Open(pEntity->pPackVolume, pEntity->pPackNode, pEntity->pPackFileRef);

	return pFile;
}

//  이 함수의 기존 구현을 레거시로 남겨둡니다...(남길만한 가치는 있다..)
i3PackFile * i3Pack::Open_Legacy(const char * pszPath)
{
	i3PackVolume * pVol = nullptr;
	i3PackNode * pPackNode = nullptr;
	I3_PACK_FILE_REF * pFileInfo = nullptr;

	for (size_t i = 0; i < s_VolList.size(); i++)
	{
		pVol = s_VolList[i];

		i3PackNode * pNode = pVol->FindPath((char *)pszPath, &pFileInfo);

		if( pNode != nullptr && (pFileInfo != nullptr))
		{
			pPackNode = pNode;
			break;
		}
	}

	if( pPackNode == nullptr)
		return nullptr;

	i3PackFile * pFile = i3PackFile::new_object_ref();

	pFile->Open(pVol, pPackNode, pFileInfo);

	return pFile;
}

I3_EXPORT_BASE
void i3Pack::Close( i3PackFile * pFile)
{
	I3_MUST_RELEASE( pFile);
}

I3_EXPORT_BASE  
void i3Pack::DumpList()
{


	for(size_t i = 0; i < s_VolList.size(); ++i )
	{
		i3PackVolume*	pVol = s_VolList[i];

		I3TRACE( "Dump Pack File Name(%d) : %s  \n", i, pVol->getPreview()->filename);	

		pVol->DumpVolume();

		I3TRACE( "\n");
	}
}


// 이제사 보니, FindDirectoryPackNode의 함수 기능은 제한적인듯 함... ( 동일 디렉토리라도, pVolume마다 제각각으로 여러개일수 있는 약점이 존재합니다... 이 함수를 쓸때는 pVolume이 1개라는 가정이 필요함..)

// 일단 디렉토리 찾는 기능은 구버전 그대로 남겨둡니다....(필요하면 i3PackFileFindMgr으로 구현을 변경하고 옮겨야 하나, 그전에 제한적 사항을 해결할 필요가 있어 미룸)

I3_EXPORT_BASE  
i3PackNode*		i3Pack::FindDirectoryPackNode(const char* pszDir)
{

	char szPath[MAX_PATH];
	i3::safe_string_copy(szPath, pszDir, MAX_PATH);

	i3::remove_end_slash(szPath);		// 마지막 슬래시 제거..
	i3::make_unix_path(szPath);			// '/' 치환..

	const INT32  nVol = s_VolList.size();

	i3PackNode * pPackNode = nullptr;
	I3_PACK_FILE_REF * pFileInfo = nullptr;

	for(INT32 i = 0; i < nVol; ++i)
	{
		i3PackVolume* pVol = s_VolList[i];

		i3PackNode * pNode = pVol->FindPath( (char *) szPath, &pFileInfo);

		if( pNode != nullptr && (pFileInfo == nullptr))		// 여기서 pFileInfo는 오히려 nullptr값이어야 Directory를 확신할수 있음..
		{
			pPackNode = pNode;
			break;
		}
	}

	return pPackNode;
}

I3_EXPORT_BASE
void i3Pack::BindCallbackUpdateFunc( CALLBACK_UPDATEFUNC func)
{
	sPackUpdateFunc = func;
}