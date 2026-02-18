#pragma once

#include "util/ObjectFactory.h"
#include "i3PackMappedFileTable.h"

namespace rsc
{
	class i3PackMappedFileTableFactory final
		: public FlyweightFactory<Ii3PackMappedFileTable, std::string, i3PackMappedFileTable_Null>
		, public i3::shared_ginst<i3PackMappedFileTableFactory>
	{
	public:
		i3PackMappedFileTableFactory() = default;
		~i3PackMappedFileTableFactory() = default;

		void Buildup();
	};
}