#include "stdafx.h"
#include "i3UIeCrawler.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace rsc
{
	i3UIeCrawler::i3UIeCrawler(const Path& workdir, const Path& i3UIeRelPath)
		: IFilePathable(workdir, i3UIeRelPath)
	{}

	i3UIeCrawler::~i3UIeCrawler() {}

	bool i3UIeCrawler::_Load()
	{
		if (!fs::exists(IFilePathable::GetAbsolutePath()))
			return false;

		const std::string& strExt = IFilePathable::GetAbsolutePath().extension().string();
		
		if (!i3::generic_is_iequal(strExt, ".i3UIe"))
			return false;

		return true;
	}

	bool i3UIeCrawler::_Gather(std::vector<PathPtr>& out)
	{
		// i3UIe 파일은 XML 파일이다.

		const std::string& stri3UIeAbsPath = IFilePathable::GetAbsolutePath().string();
		
		PathPtr i3UIsAbsPath = std::make_shared<Path>(stri3UIeAbsPath);	
		i3UIsAbsPath->replace_extension(".i3UIs");
		out.push_back(i3UIsAbsPath);

		try
		{
			boost::property_tree::ptree pt;
			boost::property_tree::read_xml(stri3UIeAbsPath, pt);

			// lua, luab.
			if (boost::optional<std::string> luaScriptRelPath =
				pt.get_optional<std::string>("i3EngineResourceFile.Object0.<xmlattr>.Script"))
			{
				const std::string& strLuaScriptRelPath = luaScriptRelPath.get();
				out.push_back(std::make_shared<Path>(strLuaScriptRelPath));

				PathPtr luabScriptRelPath = std::make_shared<Path>(strLuaScriptRelPath);
				luabScriptRelPath->replace_extension(".luab");
				out.push_back(luabScriptRelPath);
			}
		}
		catch (const boost::property_tree::ptree_error &err)
		{
			err.what();
			return true;
		}
		
		return true;
	}
}