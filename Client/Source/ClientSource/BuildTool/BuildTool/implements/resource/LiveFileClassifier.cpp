#include "stdafx.h"
#include "LiveFileClassifier.h"
#include "RscTool/RSCFileMgr.h"

namespace rsc
{
	void ClassifyNull(std::vector<PathPtr>& fileRelPathList) {}

	void ClassifyWeapon(std::vector<PathPtr>& fileRelPathList)
	{
		// live 또는 qa 폴더 파일 비교 시 애니메이션 파일들은 i3AnimPack 파일로 비교하므로,
		// 관련 i3a 파일들을 목록에서 제거한다.
		std::vector<PathPtr> eraseList;

		for (auto &fileRelPath : fileRelPathList)
		{
			if (i3::generic_is_iequal(fileRelPath->extension().string(), ".i3a"))
			{
				i3::vector<i3::rc_string> animpackRelPathList;
				RSCFileMgr::i()->FindAnimPackFile(fileRelPath->string(), animpackRelPathList);

				for (auto &currAnimpackRelPath : animpackRelPathList)
				{
					struct fn : std::binary_function<PathPtr, i3::rc_string, bool>
					{
						bool operator()(const PathPtr& srcRelPathPtr, const i3::rc_string& strAnimpackRelPath) const
						{
							return i3::generic_is_iequal(srcRelPathPtr->string(), strAnimpackRelPath.c_str());
							return true;
						}
					};

					auto it = std::find_if(fileRelPathList.begin(), fileRelPathList.end(), std::bind2nd(fn(), currAnimpackRelPath));
					if (it != fileRelPathList.end())
						eraseList.push_back(fileRelPath);
				}
			}
		}

		for (auto &currErased : eraseList)
		{
			auto it = std::find(fileRelPathList.begin(), fileRelPathList.end(), currErased);
			
			if (it != fileRelPathList.end())
				fileRelPathList.erase(it);
		}
	}

	LiveFileClassifiers::LiveFileClassifiers()
	{
		m_classifyFuncs.insert(ClassifierMapType::value_type("Weapon", std::bind(ClassifyWeapon, std::placeholders::_1)));
	}

	LiveFileClassifiers::~LiveFileClassifiers()
	{
	}

	void LiveFileClassifiers::Classify(const std::string& category, std::vector<PathPtr>& out)
	{
		auto it = m_classifyFuncs.find(category);

		if (it != m_classifyFuncs.end())
			(it->second)(out);
		else
			return ClassifyNull(out);
	}
}