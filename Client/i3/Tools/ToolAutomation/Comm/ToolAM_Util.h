
#ifndef _TOOLAM_UTIL_H_
#define _TOOLAM_UTIL_H_

#include "ToolAM_Def.h"

namespace ToolAM
{
	namespace Util
	{
		Str GeneratePath(const char* workpath, const char* subpath, const char* ext=NULL);
		Str GenerateLODFileNameWithHero(const char* prefix, const char* heroName, DWORD lodFlag);
		StrVec GenerateLODFileNamesWithHero(const char* prefix, DWORD lodFlag);

		Str NormalizePath(const Str& in, char c='/');
		void DoNormalizePath(char* in, char c='/');

	} // end of namespace util
}


#endif
