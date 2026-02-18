#include "stdafx.h"
#include "IPackRule.h"
#include "FileSync_Copy(Deprecated).h"
#include "i3pack.h"
#include <boost/format.hpp>
#include "MainFrm.h"

namespace rsc
{
	void ClearPackAnalysis(PackAnalysis& src)
	{
		src.stri3PackName.clear();
		src.addList.clear();
		src.strRemoveRelPathList.clear();
		src.strEncryptRelPathList.clear();
	}


	IPackRule::IPackRule(const std::vector<PathPtr>& srcList,
						 const Path& srcWorkdir,
						 const Path& dstWorkdir)
		: m_srcWorkdir(srcWorkdir), m_dstWorkdir(dstWorkdir), m_callback(std::make_shared<PackRuleCallback>())
	{
		std::copy(srcList.begin(), srcList.end(), std::back_inserter(m_srcPackTgtList));
	}

	IPackRule::~IPackRule() 
	{}

	void IPackRule::operator()(bool bCheckDuplicatedFile)
	{
		PackAnalysis analysis;		
		std::vector<PathPtr> copyList;
		if (Analyze(analysis, copyList))
		{
			if (!copyList.empty())
				_Copy(copyList);	// 단순 카피.

			if (!analysis.targetList.empty())
				_Pack(analysis, bCheckDuplicatedFile);	// 패킹.
		}
	}

	void IPackRule::_Copy(const std::vector<PathPtr>& copyList)
	{
		std::copy(copyList.begin(), copyList.end(), std::back_inserter(m_srcCopyTgtList));

		struct fn : std::binary_function<PathPtr, PathPtr, bool>
		{
			bool operator()(const PathPtr& lhs, const PathPtr& rhs) const
			{
				return *lhs == *rhs;
			}
		};

		for (auto &copy : copyList)
		{
			auto it = std::find_if(m_srcPackTgtList.begin(), m_srcPackTgtList.end(), std::bind2nd(fn(), copy));
			if (it != m_srcPackTgtList.end())
			{
				m_srcPackTgtList.erase(it);
			}
		}

		FileSync_Copy_Deprecated copier;

		bool success = true;
		std::vector<PathPtr> copyFailedList;
		for (auto &copyTgt : m_srcCopyTgtList)
		{
			success = copier.operator()(*copyTgt, m_srcWorkdir, m_dstWorkdir);
			if (!success)
				copyFailedList.push_back(copyTgt);
		}

		if (!copyFailedList.empty())
			GetCallback()->OnCopyComplete(m_srcCopyTgtList, m_srcWorkdir, m_dstWorkdir, true, L"");
		else
			GetCallback()->OnCopyComplete(copyFailedList, m_srcWorkdir, m_dstWorkdir, false, L"");
	}

	bool IPackRule::_Pack(const PackAnalysis& analysis, bool bCheckDuplicatedFile)
	{
		bool success = true;
		std::string strErrMessage;

		i3pack pak(m_srcWorkdir.string().c_str());

		for (auto &add : analysis.addList)
		{
			success = pak.AddFileToPack(add->strRelPath.c_str(), add->includeSubFolderOK);
			if (!success)
				strErrMessage += boost::io::str(boost::format("[PACK error, ADD] %s \n") % add->strRelPath);
		}

		for (auto &remove : analysis.strRemoveRelPathList)
		{
			success = pak.RemoveFileFromPack(remove.c_str());
			if (!success)
				strErrMessage += boost::io::str(boost::format("[PACK error, REMOVE] %s \n") % remove);
		}

		for (auto &enc : analysis.strEncryptRelPathList)
		{
			pak.EncryptFileInPack(enc.c_str());
		}

		const std::string stri3PackWorkdir = boost::io::str(boost::format("%s/Pack") % m_dstWorkdir.string());

		success = pak.SavePack(stri3PackWorkdir.c_str(), analysis.stri3PackName.c_str());
		if (!success)
			strErrMessage += boost::io::str(boost::format("[PACK error, SAVE] %s \n") % analysis.stri3PackName);

		if (analysis.checksumOK)
		{
			success = pak.SetChecksumToPack(stri3PackWorkdir.c_str(), analysis.stri3PackName.c_str());
			if (!success)
				strErrMessage += boost::io::str(boost::format("[PACK error, CHECKSUM] %s \n") % analysis.stri3PackName);
		}

		std::string stri3PackAbsPath;
		stri3PackAbsPath += stri3PackWorkdir;	stri3PackAbsPath += '/';
		stri3PackAbsPath += analysis.stri3PackName;
		i3packUtil::CheckOffsetValidity(stri3PackAbsPath.c_str());

		std::wstring wstrErrMessage;
		if (!strErrMessage.empty())
			i3::mb_to_utf16(strErrMessage, wstrErrMessage);

		GetCallback()->OnPackComplete(m_srcPackTgtList, m_srcWorkdir, m_dstWorkdir, analysis, success, wstrErrMessage.c_str());

		if (true == bCheckDuplicatedFile)
		{
			std::string packName = m_dstWorkdir.string();
			packName = packName.append("\\Pack\\");
			packName = packName.append(analysis.stri3PackName);
			GetMainFrame()->GetDuplicatedPackFileReport().AddToDuplicatedPackCheckList(packName);
		}

		return true;
	}
}