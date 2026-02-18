#include "stdafx.h"
#include "FileSync_RegisterInRSG.h"
#include "FileSync_Copy.h"
#include <boost/filesystem.hpp>
#include "RscTool/RSCFileMgr.h"
#include <boost/format.hpp>

namespace rsc
{
	FileSync_RegisterInRSG::FileSync_RegisterInRSG(IFileSync* p) : FileSync_Wrapper(p) {}

	FileSync_RegisterInRSG::~FileSync_RegisterInRSG() {}

	bool FileSync_RegisterInRSG::operator()(const Path& fileRelPath, const Path& srcWorkdir, const Path& dstWorkdir) const
	{
		if (!FileSync_Wrapper::operator()(fileRelPath, srcWorkdir, dstWorkdir))
			return false;

		// I3CHR 파일 등록 검사 및 추가.
		RscFileInfo* info = nullptr;
		if (RSCFileMgr::i()->FindRscFilePath(fileRelPath.filename().string().c_str(), info))	// 미디어 기준.
		{
			const Path i3RSCRelPath(info->m_RSCFileName.c_str());		// ".RSC" 파일 상대 경로.
			Path i3RSCGenRelPath(info->m_RSCFileName.c_str());	i3RSCGenRelPath.replace_extension(".RSG");	// ".RSG" 파일 상대 경로.

			const Path i3RSCGenAbsPath(dstWorkdir / i3RSCGenRelPath);	// ".RSG" 파일 전체 경로.
			if (!fs::exists(i3RSCGenAbsPath))
			{
				const std::wstring wstrErrMessage =
					boost::io::str(boost::wformat(L"%s\\%s is not exists.") % dstWorkdir % i3RSCGenRelPath);

				GetCallback()->OnFailed(fileRelPath, srcWorkdir, dstWorkdir, wstrErrMessage.c_str());
				return false;
			}
			
			// RSG 파일에 등록 후 RSC 파일 익스포트.
			const std::string strFileRelPath = fileRelPath.string();

			i3RSCGen* i3RSCGenPtr = 
				RSCFileMgr::i()->FindOrLoadRSCGen(dstWorkdir.string(), i3RSCGenRelPath.string());

			if (i3RSCGenPtr && !i3RSCGenPtr->FindByRelPath(strFileRelPath.c_str()))
			{
				// 중복 파일 제거. Weapon/Weapon 폴더에 있는 I3CHR 파일과 Weapon/WeaponAnimpack 폴더에 있는 I3ANIMPACK 파일들이
				// 중복으로 등록 되는 상황을 방지하기 위해.
				if (i3::generic_is_iequal(i3RSCGenRelPath.filename().string(), "Weapon.RSG") ||
					i3::generic_is_iequal(i3RSCGenRelPath.filename().string(), "WeaponAnimPack.RSG"))
				{
					i3::vector<RSCNode*> i3RSCNodeList;
					const std::string strFileName = fileRelPath.filename().string();

					if (i3RSCGenPtr->FindByFileName(strFileName.c_str(), i3RSCNodeList))
						i3RSCGenPtr->DeleteFiles(strFileName.c_str());
				}

				i3RSCGenPtr->AddFile(strFileRelPath.c_str());
				i3RSCGenPtr->BuildRSCFile(i3RSCGenAbsPath.string().c_str());

				char szi3RSCGenAbsPath[MAX_PATH] = { 0, };	
				strcpy(szi3RSCGenAbsPath, i3RSCGenAbsPath.string().c_str());
				i3RSCGenPtr->ExportRSCBin(szi3RSCGenAbsPath);

				GetCallback()->OnSucceeded(i3RSCRelPath, srcWorkdir, dstWorkdir);
				GetCallback()->OnSucceeded(i3RSCGenRelPath, srcWorkdir, dstWorkdir);
			}
		}

		return true;
	}
}