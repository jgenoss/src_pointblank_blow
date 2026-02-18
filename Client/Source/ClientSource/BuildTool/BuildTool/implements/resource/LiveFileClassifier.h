#pragma once

#include <vector>
#include <functional>
#include <map>
#include "util/ThreadPolicy.h"

namespace rsc
{
	class LiveFileClassifiers :  public i3::shared_ginst<LiveFileClassifiers>
	{
	public:
		LiveFileClassifiers();
		~LiveFileClassifiers();

		void Classify(const std::string& category, std::vector<PathPtr>& out);

	private:
		using ClassifierMapType = std::map<std::string, std::function<void(std::vector<PathPtr>&)>>;
		ClassifierMapType m_classifyFuncs;
	};
}