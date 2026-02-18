#include "stdafx.h"
#include "FileSync_Factory.h"
#include "FileSync_RegisterInRSG.h"
#include "FileSync_RegisterSoundInRSG.h"
#include "FileSync_Copy_FailIfExists.h"
#include "util/Creator.h"
#include "util/PathTokenizer.h"

namespace rsc
{
	FileSync_Factory::FileSync_Factory()
	{
		// 기본 동기화 실행은 파일 복사/붙여넣기(SyncQA_CopyFile) 이고,
		// 다른 방법의 리소스 동기화 실행이 필요하다면 아래에 등록. 
		// 등록되는 객체는 공유되기 때문에 스레드 세이프한 클래스만 등록해야 함에 유의할 것.
		
		struct Creator_Copy_RegisterInRSG
		{
			static IFileSync* Call(const Path& fileRelPath)
			{
				return new FileSync_RegisterInRSG(new FileSync_Copy_Deprecated);
			}
		};

		struct Creator_Copy_RegisterSoundInRSG
		{
			static IFileSync* Call(const Path& fileRelPath)
			{
				return new FileSync_RegisterSoundInRSG(new FileSync_Copy_Deprecated);
			}
		};

		struct Creator_Copy
		{
			static IFileSync* Call(const Path& fileRelPath)
			{
				std::vector<std::wstring> wstrTokenList;
				GetPathTokenList(fileRelPath, wstrTokenList);
				if (!wstrTokenList.empty())
				{
					if (i3::generic_is_iequal(wstrTokenList[0], L"Scene"))	// 맵 Scene 폴더의 RSC 파일은 강제 복사 대상이다.
						return new FileSync_Copy;
				}

				return new FileSync_Copy_FailIfExists;
			}
		};

		Regist(".I3CHR",		Creator_Copy_RegisterInRSG::Call);
		Regist(".I3ANIMPACK",	Creator_Copy_RegisterInRSG::Call);
		Regist(".WAV",			Creator_Copy_RegisterSoundInRSG::Call);
		Regist(".RSG",			Creator_Copy::Call);
		Regist(".RSC",			Creator_Copy::Call);
		Regist(".I3I",			Creator_Copy_RegisterInRSG::Call);
		Regist(".I3S",			Creator_Copy_RegisterInRSG::Call);
		Regist(".I3PACK",		Creator_Copy::Call);
	}

	FileSync_Factory::~FileSync_Factory() {}
}