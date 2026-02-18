#include "stdafx.h"
#include "ShaderContentsCrawler.h"
#include "ContentsCrawlerCallback.h"

namespace rsc
{
	ShaderContentsCrawler::ShaderContentsCrawler()
	{}

	ShaderContentsCrawler::~ShaderContentsCrawler()
	{}

	bool ShaderContentsCrawler::__Load()
	{
		return true;
	}

	bool ShaderContentsCrawler::__Gather()
	{
		if (GetCallback())
			GetCallback()->OnGather(IContentsCrawler::GetCountryName(), "Shader Sync All", "Shader", "HLSL");

		return true;
	}

	bool ShaderContentsCrawler::__PreGather()
	{
		if (GetCallback())
			GetCallback()->OnPreGather(IContentsCrawler::GetCountryName(), "Shader");

		return true;
	}

	bool ShaderContentsCrawler::__PostGather()
	{
		if (GetCallback())
			GetCallback()->OnPostGather(IContentsCrawler::GetCountryName(), "Shader");

		return true;
	}
}