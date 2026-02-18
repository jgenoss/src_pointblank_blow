#pragma once

#include "util/ObjectFactory.h"
#include "FileSync_Copy.h"
#include "FileSync_Copy(Deprecated).h"

namespace rsc
{
	class FileSync_Factory final : 
		
		public FlyweightFactory<IFileSync, std::string, FileSync_Copy_Deprecated, 
								IFileSync* (*)(const Path&)>,

		public i3::shared_ginst<FileSync_Factory>
	{

	public:
		FileSync_Factory();
		~FileSync_Factory();

	};
}