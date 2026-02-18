#include "stdafx.h"
#include "PackScriptParser.h"
#include "PackCommands.h"
#include "i3Base/string/ext/remove_blank_from_head.h"
#include "i3Base/string/ext/remove_all_char.h"
#include <boost/tokenizer.hpp>

namespace rsc
{
	PackScriptParser::PackScriptParser(const Path& i3PackScriptAbsPath) 
		: IFilePathable(i3PackScriptAbsPath)
	{
		if (i3::generic_is_iequal(i3PackScriptAbsPath.extension().string(), ".i3PackScript"))
		{
			m_openOK =
				m_fstream.Open(i3PackScriptAbsPath.string().c_str(), STREAM_READ | STREAM_SHAREREAD) ? true : false;
		}
	}

	PackScriptParser::PackScriptParser(const Path& i3PackScriptWorkdir, const Path& i3PackScriptRelPath)
		: PackScriptParser(i3PackScriptWorkdir / i3PackScriptRelPath)
	{
		IFilePathable::SetWorkdir(i3PackScriptWorkdir);
		IFilePathable::SetWorkdir(i3PackScriptRelPath);
	}

	PackScriptParser::~PackScriptParser()
	{
		m_fstream.Close();
	}

	void PackScriptParser::operator()(std::vector<std::vector<std::string>>& outCmdParams)
	{
		if (!m_openOK)
			return;

		char szCommandLine[512] = { 0, };
		std::string strCommandLine;

		using tok = boost::tokenizer < boost::char_separator<char> >;

		boost::char_separator<char> sep(" \t");	// white space, Tab.

		while (m_fstream.ReadLine(szCommandLine, sizeof(szCommandLine) - 1) > 0)
		{
			i3::remove_blank_from_head(szCommandLine);

			if ((szCommandLine[0] == 0) || (szCommandLine[0] == '#'))
				continue;

			strCommandLine = szCommandLine;

			std::vector<std::string> params;

			tok tokens(strCommandLine, sep);
			tok::const_iterator it = tokens.begin();
			for (; it != tokens.end(); it++)
				params.push_back(*it);

			if (!params.empty())
				outCmdParams.push_back(params);
		}
	}
}