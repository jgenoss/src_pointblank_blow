#include "stdafx.h"
#include "ContentsSynchronizerFactory.h"
#include "Settings.h"

namespace rsc
{
	ContentsSynchronizerPtr ContentsSynchronizerFactory::Request(ContentsSyncDataPtr data) const
	{
		switch (data->type)
		{
		case SYNCHRONIZER_TYPE_CONTENTS:
		{
			data->fileRelPathList.clear();	// 파일 경로들을 ContentsSynchronizer_CopyPack 객체 내부에서 생성하므로 삭제 처리함.
			return std::make_shared<ContentsSynchronizer_CopyPack>(data->contents, data->country);
		}break;

		case SYNCHRONIZER_TYPE_FILES:
		{
			return std::make_shared<FileSynchronizer_CopyPack>(data->contents, data->country, data->fileRelPathList);
		}break;

		case SYNCHRONIZER_TYPE_SHADER:
		{
			const VerDataPtr versionData = VersionSettings::i()->FindData(data->country->GetVersion());
			const Path i3EngineAbsPath(versionData->Geti3EngineAbsPath());
			const Path HLSLFolderAbsPath(i3EngineAbsPath / "Shader");

			return std::make_shared<ShaderSynchronizer>(HLSLFolderAbsPath, i3EngineAbsPath);
		}break;

		default:
		{
			return std::make_shared<Synchronizer_Null>();
		}break;
		}
	}
}