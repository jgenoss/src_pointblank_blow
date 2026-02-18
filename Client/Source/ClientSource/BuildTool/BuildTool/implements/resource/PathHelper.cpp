#include "stdafx.h"
#include "PathHelper.h"
#include "util/Creator.h"

namespace rsc
{
	PathHelperFactory::PathHelperFactory()
	{
		Regist("v1", Creator<IPathHelper, PathHelper_V1>::Call);
		Regist("v2", Creator<IPathHelper, PathHelper_V2>::Call);
	}

	const Path GetExeDir()
	{
		CString path;
		GetModuleFileName(NULL, path.GetBuffer(_MAX_PATH), _MAX_PATH);
		path.ReleaseBuffer();
		path = path.Left(path.ReverseFind(L'\\'));

		return Path(std::wstring(path));
	}

	const Path GetSettingsDir()
	{
		return Path(GetExeDir() / "Settings");
	}
}