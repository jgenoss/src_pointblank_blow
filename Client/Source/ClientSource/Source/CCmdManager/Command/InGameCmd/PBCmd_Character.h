#pragma once

#include "Cheatkey_ICommand.h"

namespace Cheatkey
{
	class SwitchCharacter : public ICommand
	{
	public:
		explicit SwitchCharacter(ILimitPred* p);
		virtual ~SwitchCharacter() {}

		static ICommand* Creator();

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;

		CHARA_RES_ID m_charaType;		// [initialize at constructor]
	};

	class Skill : public ICommand
	{
	public:
		explicit Skill(ILimitPred* p) : ICommand(p), m_category(0), m_idx(0), m_level(0) {}
		virtual ~Skill() {}

		static ICommand* Creator();

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;

		INT32 m_category, m_idx, m_level;	// [initialize at constructor]
	};

	class Invincible : public ICommand
	{
	public:
		explicit Invincible(ILimitPred* p) : ICommand(p) {}
		virtual ~Invincible() {}

		static ICommand* Creator();

	private:
		virtual bool _Execute() override;
	};
}