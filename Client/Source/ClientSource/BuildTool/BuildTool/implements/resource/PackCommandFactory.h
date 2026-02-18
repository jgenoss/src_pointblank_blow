#pragma once

#include "util/ObjectFactory.h"
#include "PackCommands.h"

namespace rsc
{
	class PackCommandFactory final : 
		public FlyweightFactory<IPackCommand, std::string, IPackCommand, IPackCommand* (*)()>,
		public i3::shared_ginst < PackCommandFactory >
	{
	public:
		PackCommandFactory()
		{
			Regist("ADD",		PackCommand_AddFileToPack::Creator);
			Regist("REMOVE",	PackCommand_RemoveFileFromPack::Creator);
			Regist("ENC",		PackCommand_EncryptFileInPack::Creator);
			Regist("SAVE",		PackCommand_SavePack::Creator);
			Regist("CHECKSUM",	PackCommand_SetChecksumToPack::Creator);
		}
	};
}