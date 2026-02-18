#pragma once

#include <memory>
#include <string>
#include <map>
#include "util/ObjectFactory.h"
#include "NullCompare.h"
#include "NativeFileCompare.h"
#include "i3PackInnerFileCompare.h"

namespace rsc
{
	class NativeFileCompareFactory final: 
		public FlyweightFactory<IFileCompare, std::string, NativeFileCompare>, // NativeFileCompare 객체는 공유 변수 없으므로 스레드 세이프하다.
		public i3::shared_ginst<NativeFileCompareFactory>
	{
	public:
		NativeFileCompareFactory()
		{
			//regist(".i3a", );
		}
	};

	class i3PackCompareFactory final: 
		public FlyweightFactory<IFileCompare, std::string, i3PackInnerFileCompare>, // PackedFileCompare 객체는 공유 변수 없으므로 스레드 세이프하다.
		public i3::shared_ginst<i3PackCompareFactory>
	{
	public:
		i3PackCompareFactory()
		{
			//regist(".i3a", );
		}
	};
}