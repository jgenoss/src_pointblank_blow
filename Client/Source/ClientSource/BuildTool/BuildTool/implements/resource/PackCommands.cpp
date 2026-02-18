#include "stdafx.h"
#include "PackCommands.h"
#include "i3pack.h"
#include <boost/filesystem.hpp>

namespace rsc
{
	void PackCommand_AddFileToPack::operator()(std::shared_ptr<i3pack> i3PackPtr, const std::vector<std::string>& tokenList)
	{
		if (i3PackPtr.get() == nullptr)
			return;

		if (tokenList.size() < 2)
			return;

		const std::string& cmd = tokenList[0];
		if (!i3::generic_is_iequal(cmd, "ADD"))
			return;

		const Path fileRelpath = tokenList[1];

		bool includeSubdirOK = fileRelpath.has_extension() ? false : true;
		if (tokenList.size() > 2)
		{
			if (i3::generic_is_iequal(tokenList[2], "-nosub"))
				includeSubdirOK = false;
		}

		i3PackPtr->AddFileToPack(fileRelpath.string().c_str(), includeSubdirOK);
	}

	IPackCommand* PackCommand_AddFileToPack::Creator()
	{
		return new PackCommand_AddFileToPack;
	}

	void PackCommand_RemoveFileFromPack::operator()(std::shared_ptr<i3pack> i3PackPtr, const std::vector<std::string>& tokenList)
	{
		if (i3PackPtr.get() == nullptr)
			return;

		if (tokenList.size() < 2)
			return;

		const std::string& strCmd = tokenList[0];
		if (!i3::generic_is_iequal(strCmd, "REMOVE"))
			return;

		const std::string& strFileRelpath = tokenList[1];

		i3PackPtr->RemoveFileFromPack(strFileRelpath.c_str());
	}

	IPackCommand* PackCommand_RemoveFileFromPack::Creator()
	{
		return new PackCommand_RemoveFileFromPack;
	}

	void PackCommand_SavePack::operator()(std::shared_ptr<i3pack> i3PackPtr, const std::vector<std::string>& tokenList)
	{
		if (i3PackPtr.get() == nullptr)
			return;

		if (tokenList.size() < 2)
			return;

		const std::string& strCmd = tokenList[0];
		if (!i3::generic_is_iequal(strCmd, "SAVE"))
			return;

		const std::string& stri3PackName = tokenList[1];

		i3PackPtr->SavePack(stri3PackName.c_str());

		i3packUtil::CheckOffsetValidity(stri3PackName.c_str());
	}

	IPackCommand* PackCommand_SavePack::Creator()
	{
		return new PackCommand_SavePack;
	}

	void PackCommand_EncryptFileInPack::operator()(std::shared_ptr<i3pack> i3PackPtr, const std::vector<std::string>& tokenList)
	{
		if (i3PackPtr.get() == nullptr)
			return;

		if (tokenList.size() < 2)
			return;

		const std::string& strCmd = tokenList[0];
		if (!i3::generic_is_iequal(strCmd, "ENC"))
			return;

		const std::string& strFileRelpath = tokenList[1];

		i3PackPtr->EncryptFileInPack(strFileRelpath.c_str());
	}

	IPackCommand* PackCommand_EncryptFileInPack::Creator()
	{
		return new PackCommand_EncryptFileInPack;
	}

	void PackCommand_SetChecksumToPack::operator()(std::shared_ptr<i3pack> i3PackPtr, const std::vector<std::string>& tokenList)
	{
		if (i3PackPtr.get() == nullptr)
			return;

		if (tokenList.size() < 2)
			return;

		const std::string& strCmd = tokenList[0];
		if (!i3::generic_is_iequal(strCmd, "CHECKSUM"))
			return;

		const std::string& stri3PackName = tokenList[1];

		i3PackPtr->SetChecksumToPack(stri3PackName.c_str());
	}

	IPackCommand* PackCommand_SetChecksumToPack::Creator()
	{
		return new PackCommand_SetChecksumToPack;
	}
}