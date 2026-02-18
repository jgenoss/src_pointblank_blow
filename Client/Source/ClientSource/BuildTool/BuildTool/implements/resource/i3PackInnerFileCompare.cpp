#include "stdafx.h"
#include "i3PackInnerFileCompare.h"
#include <boost/filesystem.hpp>
#include "i3PackMappedFileTableFactory.h"
#include "i3PackMappedFileTable.h"
#include "RscTool/FindPefDiff.h"

namespace
{
	bool FindFileSize(i3PackVolume* vol, const i3::string& file, unsigned long long& outSize)
	{
		I3_PACK_FILE_REF* info = nullptr;

		i3PackNode* root = vol->getRoot();
		if (root == nullptr) return false;

		i3PackNode* node = root->FindPath(file.c_str(), &info);
		if (node == nullptr) return false;

		i3FileStream* stream = vol->getCurrThreadStream();
		stream->SetPosition(info->m_Offset, STREAM_BEGIN);

		outSize = info->m_Size;
		return true;
	}

	struct dtor
	{
		i3PackVolume* vol = nullptr;

		dtor(i3PackVolume* v) 
		{ 
			vol = v; 
		}

		~dtor() 
		{ 
			vol->Release(); 
		}
	};
}

namespace rsc
{
	bool i3PackInnerFileCompare::IsExist(const Path& workdir, const Path& fileRelPath)
	{
		auto table = i3PackMappedFileTableFactory::i()->RequestObj(workdir.string());
		
		if (table->IsExist(fileRelPath))
			return true;

		return false;
	}

	bool i3PackInnerFileCompare::CalcMD5(const Path& workdir, const Path& fileRelPath, const Crypto::MD5Handle md5handle, Crypto::MD5Result& outMD5Value)
	{
		auto table = i3PackMappedFileTableFactory::i()->RequestObj(workdir.string());

		Path i3PackAbsPath;
		if (table->Find(fileRelPath, i3PackAbsPath))
		{
			i3PackVolume* vol = LoadPackVolume(i3::string(i3PackAbsPath.string().c_str()));
			if (vol == nullptr) 
				return false;

			dtor vol_dtor(vol);

			bool enc = false;
			GetMD5_Live(vol, fileRelPath.string().c_str(), md5handle, outMD5Value, enc);

			return true;
		}

		return false;
	}

	unsigned long long i3PackInnerFileCompare::GetFileSize(const Path& workdir, const Path& fileRelPath)
	{
		auto table = i3PackMappedFileTableFactory::i()->RequestObj(workdir.string());
		
		Path i3PackAbsPath;
		if (table->Find(fileRelPath, i3PackAbsPath))
		{
			i3PackVolume* vol = LoadPackVolume(i3::string(i3PackAbsPath.string().c_str()));
			if (vol == nullptr) 
				return 0;

			dtor vol_dtor(vol);

			unsigned long long size = 0;
			if (FindFileSize(vol, fileRelPath.string().c_str(), size))
				return size;
		}

		return 0;
	}
}