
#include "ToolAMPCH.h"
#include "ToolAM_Util.h"

#include <iostream>   // std::cout, std::ios
#include <sstream>    // std::ostringstream


namespace ToolAM
{
	namespace Util
	{
		
		Str GeneratePath(const char* workpath, const char* subpath, const char* ext)
		{
			Str final = Str(workpath) + '/' + Str(subpath);

			if(ext)
				final = final + "/" + ext;

			final = NormalizePath(final, '/');

			return final;
		}

		Str GenerateLODFileNameWithHero(const char* prefix, const char* heroName, DWORD lod)
		{
			std::stringstream os;

			os << prefix << '_' << heroName << "_LOD";
			os << lod;
			os << ".i3s";

			return os.str();
		}

		StrVec GenerateLODFileNamesWithHero(const char* prefix, DWORD lodFlag)
		{
			return StrVec();
		}

		Str NormalizePath(const Str& in, char c)
		{
			char findChar = '\\';
			if(c == '\\') findChar = '/';

			Str rst = in;
			INT32 numChar = (INT32)rst.size();
			for(INT32 i=0; i < numChar; i++)
			{
				if(rst[i] == findChar) rst[i] = c;
			}

			return rst;
		}

		void DoNormalizePath(char* in, char c)
		{
			if(!in)
				return;

			char findChar = '\\';
			if(c == '\\') findChar = '/';

			char* cur = in;

			while(cur && *cur != NULL)
			{
				if(*cur == findChar) *cur = c;
				cur++;
			}
		}

	} // end of namespace util


//////////////////////////////////////////////////////////////////////////
} // end of ToolAM
//////////////////////////////////////////////////////////////////////////
