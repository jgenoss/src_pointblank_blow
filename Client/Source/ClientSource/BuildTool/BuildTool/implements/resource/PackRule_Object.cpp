#include "stdafx.h"
#include "PackRule_Object.h"
#include <boost/filesystem.hpp>
#include "i3pack.h"
#include "util/PathTokenizer.h"
#include "../util/CustomUtil.h"
#include <boost/format.hpp>

namespace rsc
{
	PackRule_Object::PackRule_Object(const std::string& strObjFolderRelPath, 
									 const std::vector<PathPtr>& srcList,
									 const Path& srcWorkdir, 
									 const Path& dstWorkdir)
		: IPackRule(srcList, srcWorkdir, dstWorkdir), m_strObjFolderRelPath(strObjFolderRelPath)
	{}

	PackRule_Object::~PackRule_Object() 
	{}

	void PackRule_Object::Creator(std::vector<PathPtr>& inout,
								  const Path& srcWorkdir,
								  const Path& dstWorkdir,
								  std::vector<PackRulePtr>& out)
	{
		struct PickoutObjPred : std::unary_function<PathPtr, bool>
		{
			bool operator()(const PathPtr& src) const
			{
				// ---------원본 팩스크립트 규칙---------------
				//	add	Object\cgame\Tank
				//	save Object_cgame_Tank.i3Pack
				//	checksum Object_cgame_Tank.i3Pack
				// --------------------------------------------

				if (!src->has_extension())	// 폴더 대상 X.
					return true;

				std::vector<std::string> strTokenList;
				GetPathTokenList(*(src.get()), strTokenList);

				if (strTokenList.size() < 3)	// Object/.../ 하위 폴더 대상.
					return true;

				const std::string& strRootFolderName = strTokenList[0];	// "Object/..." 대상으로만.
				if (!i3::generic_is_iequal(strRootFolderName, "Object"))
					return true;

				return false;
			}
		};

		std::vector<PathPtr> pickoutList;
		std::remove_copy_if(inout.begin(),inout.end(), std::back_inserter(pickoutList), PickoutObjPred());

		using PathListPtr = std::shared_ptr<std::vector<PathPtr>>;
		using ObjClassifiedMap = std::map<std::string, PathListPtr>;
		ObjClassifiedMap tbl;

		std::string strObjFolderRelPath;
		for (auto &pickout : pickoutList)
		{
			strObjFolderRelPath = pickout->parent_path().string();

			auto it = tbl.find(strObjFolderRelPath);
			if (it == tbl.end())
			{
				auto pr = tbl.insert(ObjClassifiedMap::value_type(
					strObjFolderRelPath, std::make_shared<std::vector<PathPtr>>()));
				
				if (pr.second == false)
					continue;

				it = pr.first;
			}

			(it->second)->push_back(pickout);
		}

		PathListPtr classifiedList;
		for (auto it = tbl.begin(); it != tbl.end(); ++it)
		{
			strObjFolderRelPath = it->first;
			classifiedList = it->second;

			out.push_back(std::make_shared<PackRule_Object>(strObjFolderRelPath, *classifiedList, srcWorkdir, dstWorkdir));
		}
	}

	bool PackRule_Object::Analyze(PackAnalysis& outAnalysis, std::vector<PathPtr>& outCopyList)
	{
		if (!fs::is_directory(m_srcWorkdir))
			return false;

		// pack target paths.
		std::copy(m_srcPackTgtList.begin(), m_srcPackTgtList.end(), std::back_inserter(outAnalysis.targetList));

		// name of pack.
		std::string strObji3PackName;

		std::vector<std::string> tokenList;
		GetPathTokenList(m_strObjFolderRelPath, tokenList);
		for (auto &token : tokenList)
		{
			strObji3PackName += token;
			strObji3PackName += "_";
		}

		if (!strObji3PackName.empty())
			strObji3PackName.erase(strObji3PackName.end() - 1);	// 마지막 '_' 제거.
		
		strObji3PackName += ".i3Pack";

		outAnalysis.stri3PackName = strObji3PackName;

		// add.
		auto add = std::make_shared<PackAddTarget>();

		add->strRelPath = m_strObjFolderRelPath;
		add->includeSubFolderOK = true;

		outAnalysis.addList.push_back(add);

		// checksum.
		outAnalysis.checksumOK = true;

		return true;
	}
}