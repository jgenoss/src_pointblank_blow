#include "stdafx.h"
#include "PackSync_World.h"
#include "PackRule_World.h"
#include "PackRule_World_Col.h"
#include "PackRule_World_Col_Hero.h"
#include "PackRule_World_Scene.h"
#include "PackRule_World_SubData.h"
#include "PackRule_World_TabMinimap.h"
#include "PackRule_World_Textures.h"
#include "PackRule_Sound.h"
#include "PackRule_Object.h"
#include "PackRule_World_ImagesNoText.h"

namespace rsc
{
	void PackSync_World_V1::_RegisterRuleCreators()
	{
		m_ruleCreators.push_back(std::bind(PackRule_World::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_World_Col::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_World_Col_Hero::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_World_Scene::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_World_SubData::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_World_Textures::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_Sound::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_Object::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	}

	void PackSync_World_V2::_RegisterRuleCreators()
	{
		m_ruleCreators.push_back(std::bind(PackRule_World::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_World_Col::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_World_Col_Hero::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_World_Scene::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_World_SubData::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_World_TabMinimap::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_World_Textures::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_Sound::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_Object::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	}

	void PackSync_World_V3::_RegisterRuleCreators()
	{
		m_ruleCreators.push_back(std::bind(PackRule_World::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_World_Col::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_World_Col_Hero::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_World_Scene::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_World_SubData::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_World_TabMinimap::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_World_Textures::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_Sound::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_Object::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_ruleCreators.push_back(std::bind(PackRule_World_ImagesNoText::Creator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	}
}