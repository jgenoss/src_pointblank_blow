#include "stdafx.h"
#include "i3aNativeFileComparator.h"
#include "fromRscTool/RSCFileMgr.h"
#include <boost/filesystem.hpp>
#include "i3Math/i3AnimationPackFile.h"

#if defined(I3_COMPILER_VC)
#pragma pack(push, 4)
#endif

struct I3_PERSIST_ANIMPACKFILE_HEADER
{
	UINT8		m_ID[4];

	UINT32		m_AnimCount;
	UINT32		m_TArrayCount;
	UINT32		m_RArrayCount;
	UINT32		m_SArrayCount;

	INT64		m_DataOffset;
	INT64		m_DataSize;

	UINT32		m_T16ArrayCount;
	UINT32		m_R16ArrayCount;
	UINT32		m_S16ArrayCount;

	UINT32		pad[29];
};

struct I3_PERSIST_ANIM_INFO
{
	UINT32		m_Type;		// 0 : i3Animation, 1:i3Animation2, 2:i3Animation2 & 16Bits 3:i3Animation2 TrackTable's 32bits
	UINT32		m_TrackCount;
	REAL32		m_Duration;
	INT64		m_Offset;
	char		m_szName[256];
	UINT32		pad[2];
};

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

namespace rsc
{
	bool i3aNativeFileComparator::IsExist(const fs::path& workdir, const fs::path& file) const
	{
		if (fs::exists(workdir / file))
			return true;

		// 관련 i3animpack 파일을 열어서 i3a 파일이 존재하는지 확인. 해당 국가 폴더의 i3animpack임을 유의.

		i3::vector<i3::rc_string> animpackPathList;
		rsctool::RSCFileMgr::i()->FindAnimPackFile(file.string(), animpackPathList);

		return !animpackPathList.empty();
	}

	bool i3aNativeFileComparator::CalcMD5(const fs::path& workdir, const fs::path& file, const Crypto::MD5Handle md5handle, Crypto::MD5Result& outMD5Value) const
	{
		return rsctool::GetMD5_MediaQA(workdir.wstring().c_str(), file.string().c_str(), md5handle, outMD5Value);
	}

	unsigned long long i3aNativeFileComparator::GetFileSize(const fs::path& workdir, const fs::path& file) const
	{
		return fs::file_size(workdir / file);
	}
}