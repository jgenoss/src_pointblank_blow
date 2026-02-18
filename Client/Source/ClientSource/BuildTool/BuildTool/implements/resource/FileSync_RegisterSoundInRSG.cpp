#include "stdafx.h"
#include "FileSync_RegisterSoundInRSG.h"
#include "FileSync_Copy.h"
#include <boost/filesystem.hpp>
#include "RscTool/RSCFileMgr.h"
#include <boost/format.hpp>

namespace
{
	bool GetOrigSoundPropsInRSCGen(const Path& mediaWorkdir, const Path& i3RSCGenFileRelPath, 
		const std::string& strSoundRelPath, REAL32& outMaxDist, REAL32& outMinDist, INT32& outDefaultVolume, INT32& outListenerVolume)
	{
		outMaxDist = outMinDist = 0.0f;	outDefaultVolume = outListenerVolume = 0;

		i3RSCGen* i3RSCGenPtr =
			RSCFileMgr::i()->FindOrLoadRSCGen(mediaWorkdir.string().c_str(), i3RSCGenFileRelPath.string().c_str());

		if (i3RSCGenPtr == nullptr)
			return false;

		RSCNode* i3RSCNodeSound = i3RSCGenPtr->FindByRelPath(strSoundRelPath.c_str());
		if (i3RSCNodeSound)
		{
			RSC_DATA* data = i3RSCNodeSound->GetData();
			if (data)
			{
				CFieldPack* fp = data->m_spField.get();

				RSC_FIELD_INFO* field = fp->GetField(3);
				if (field)
					outMaxDist = i3::any_cast<REAL32>(field->m_FieldData);

				field = fp->GetField(4);
				if (field)
					outMinDist = i3::any_cast<REAL32>(field->m_FieldData);

				field = fp->GetField(5);
				if (field)
					outDefaultVolume = i3::any_cast<INT32>(field->m_FieldData);

				field = fp->GetField(6);
				if (field)
					outListenerVolume = i3::any_cast<INT32>(field->m_FieldData);
			}
		}

		return true;
	}
}

namespace rsc
{
	FileSync_RegisterSoundInRSG::FileSync_RegisterSoundInRSG(IFileSync* p) : FileSync_Wrapper(p) 
	{}

	FileSync_RegisterSoundInRSG::~FileSync_RegisterSoundInRSG() 
	{}

	bool FileSync_RegisterSoundInRSG::operator()(const Path& fileRelPath, const Path& srcWorkdir, const Path& dstWorkdir) const
	{
		if (!FileSync_Wrapper::operator()(fileRelPath, srcWorkdir, dstWorkdir))
			return false;

		RscFileInfo* info = nullptr;
		if (RSCFileMgr::i()->FindRscFilePath(fileRelPath.filename().string().c_str(), info))
		{
			const Path i3RSCRelPath(info->m_RSCFileName.c_str());		// ".RSC" 파일 상대 경로.
			Path i3RSCGenRelPath(info->m_RSCFileName.c_str());	i3RSCGenRelPath.replace_extension(".RSG");	// ".RSG" 파일 상대 경로.

			const Path i3RSCGenAbsPath(dstWorkdir/i3RSCGenRelPath);	// ".RSG" 파일 전체 경로.
			if (!fs::exists(i3RSCGenAbsPath))
			{
				const std::wstring wstrErrMessage =
					boost::io::str(boost::wformat(L"%s\\%s is not exists.") % dstWorkdir % i3RSCGenRelPath);

				GetCallback()->OnFailed(fileRelPath, srcWorkdir, dstWorkdir, wstrErrMessage.c_str());

				return false;
			}

			// RSG 파일에 등록 후 RSC 파일 익스포트.
			const std::string strSoundRelPath = fileRelPath.string();

			i3RSCGen* i3RSCGenPtr = 
				RSCFileMgr::i()->FindOrLoadRSCGen(dstWorkdir.string(), i3RSCGenRelPath.string());
			
			RSCNode* i3RSCNode = nullptr;
			if (i3RSCGenPtr)
			{
				// 기존 파일 제거(중복되는 이름을 가진 노드도 제거).
				i3::vector<RSCNode*> i3RSCNodeList;
				const std::string strFileName = fileRelPath.filename().string();

				if (i3RSCGenPtr->FindByFileName(strFileName.c_str(), i3RSCNodeList))
					i3RSCGenPtr->DeleteFiles(strFileName.c_str());

				i3RSCGenPtr->AddFile(strSoundRelPath.c_str());

				// 사운드 데이터 복사.
				RSCNode* curri3RSCNodeSound = i3RSCGenPtr->FindByRelPath(strSoundRelPath.c_str());
				if (curri3RSCNodeSound)
				{
					RSC_DATA* soundData = curri3RSCNodeSound->GetData();
					if (soundData)
					{
						// 미디어 RSG 파일 열어서 데이터 가져오기.
						REAL32 maxDist, minDist;
						INT32 defaultVolume, listenerVolume;

						GetOrigSoundPropsInRSCGen(srcWorkdir, i3RSCGenRelPath, strSoundRelPath, maxDist, minDist, defaultVolume, listenerVolume);

						// QA RSG 파일에 데이터 복사.
						CFieldPack* fp = soundData->m_spField.get();

						RSC_FIELD_INFO* field = nullptr;
						
						field = fp->GetField(3);		// "Sound_MaxDist"
						if (fp)
							field->m_FieldData = maxDist;

						field = fp->GetField(4);		// "Sound_MinDist"
						if (field)
							field->m_FieldData = minDist;

						field = fp->GetField(5);		// "DefaultVolume"
						if (field)
							field->m_FieldData = defaultVolume;

						field = fp->GetField(6);		// "ListenerVolume"
						if (field)
							field->m_FieldData = listenerVolume;
					}
				}

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