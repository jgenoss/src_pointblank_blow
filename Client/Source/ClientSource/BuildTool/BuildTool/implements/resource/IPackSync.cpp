#include "stdafx.h"
#include "IPackSync.h"
#include "PackScriptMappedTable.h"
#include "PackScriptParser.h"
#include "PackCommandFactory.h"
#include "i3pack.h"
#include "../util/CustomUtil.h"
#include <boost/format.hpp>
#include "MainFrm.h"

namespace rsc
{
	IPackSync::IPackSync() : m_callback(std::make_shared<PackScriptCallback>())
	{}

	IPackSync::~IPackSync() 
	{}

	bool IPackSync::operator()(const std::vector<PathPtr>& srcList, const Path& srcWorkdir, const Path& dstWorkdir)
	{
		std::vector<PathPtr> outUnperformedList;
		std::set<std::string> outPackedFileNameList;
		bool bCheckDuplicatedFile = GetMainFrame()->GetDuplicatedPackFileReport().UseDuplicatedFileCheck();

		// 기존 팩스크립트를 이용 패킹 수행.
		_PackingByPackScript(srcList, srcWorkdir, dstWorkdir, outUnperformedList, bCheckDuplicatedFile);

		// 소스 코드 이용 패킹.
		if (!outUnperformedList.empty())
		{
			_RegisterRuleCreators();
			_PackingByRule(outUnperformedList, srcWorkdir, dstWorkdir, bCheckDuplicatedFile);
		}

		return true;
	}

	void IPackSync::_PackingByPackScript(const std::vector<PathPtr>& srcList,
										const Path& srcWorkdir,
										const Path& dstWorkdir,
										std::vector<PathPtr>& outUnperformedList,
										bool bCheckDuplicatedFile)
	{
		// 긁어 모은 팩스크립트 정보들 활용하여
		// 개별 파일 단위로 패킹 수행.
		// 수행된 개별 파일은 fileList에서 삭제.

		using SrcRelPathList = std::vector<PathPtr>;
		using SrcRelPathListPtr = std::unique_ptr<SrcRelPathList>;
		using i3PackScriptMappedTbl = std::map<std::string, SrcRelPathListPtr>;
		i3PackScriptMappedTbl tbl;

		Path i3PackScriptAbsPath;
		std::string str;

		for (auto &currRelPathPtr : srcList)
		{
			if (PackScriptMappedTable::i()->FindPackScript(*(currRelPathPtr.get()), i3PackScriptAbsPath))
			{
				str = i3PackScriptAbsPath.string();
				auto it = tbl.find(str);
				if (it == tbl.end())
				{
					auto pr = tbl.insert(i3PackScriptMappedTbl::value_type(str, std::make_unique<SrcRelPathList>()));
					if (pr.second == true)
					{
						((pr.first)->second)->push_back(currRelPathPtr);
						continue;
					}
				}

				(it->second)->push_back(currRelPathPtr);
				continue;
			}

			outUnperformedList.push_back(currRelPathPtr);
		}

		// 팩스크립트 이용 패킹.
		std::vector<std::vector<std::string>> outCmdParams;
		std::string stri3PackName;
		std::string stri3PackSaveDir(dstWorkdir.string()); stri3PackSaveDir += "/Pack";
		std::string stri3PackRelPath;
		std::string strCommandKey;

		for (auto it = tbl.begin(); it != tbl.end(); ++it)
		{
			const std::string& stri3PackScriptAbsPath = it->first;
			
			outCmdParams.clear();

			PackScriptParser parser(stri3PackScriptAbsPath);
			parser.operator()(outCmdParams);

			auto i3PackPtr = std::make_shared<i3pack>(srcWorkdir.string().c_str(), stri3PackSaveDir.c_str());

			for (auto &params : outCmdParams)
			{
				if (params.empty())
					continue;

				strCommandKey = params[0];
				i3::to_upper(strCommandKey);

				auto command = PackCommandFactory::i()->RequestObj(strCommandKey);

				command->operator()(i3PackPtr, params);

				if (i3::generic_is_iequal(strCommandKey, "SAVE"))
				{
					if (params.size() > 1)
						stri3PackName = params[1];
				}
			}

			SrcRelPathList* srcPackTgtList = (it->second).get();
			stri3PackRelPath = boost::io::str(boost::format("Pack/%s") % stri3PackName);
			m_callback->OnPackComplete(*srcPackTgtList, srcWorkdir, dstWorkdir, stri3PackRelPath, i3PackScriptAbsPath);

			if (true == bCheckDuplicatedFile)
			{
				std::string packName = dstWorkdir.string();
				packName = packName.append("\\Pack\\");
				packName = packName.append(stri3PackName);
				GetMainFrame()->GetDuplicatedPackFileReport().AddToDuplicatedPackCheckList(packName);
			}
		}
	}

	void IPackSync::_PackingByRule(std::vector<PathPtr>& inout, const Path& srcWorkdir, const Path& dstWorkdir,
									bool bCheckDuplicatedFile)
	{
		std::vector<PackRulePtr> outRuleList;

		for (auto &creator : m_ruleCreators)
			creator.operator()(inout, srcWorkdir, dstWorkdir, outRuleList);

		for (auto &rule : outRuleList)
		{
			//currPackRulePtr->SetCallback();
			rule->operator()(bCheckDuplicatedFile);
		}

		m_ruleCreators.clear();
	}
}