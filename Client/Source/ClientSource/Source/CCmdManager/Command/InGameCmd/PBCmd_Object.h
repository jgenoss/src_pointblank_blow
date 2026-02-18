#pragma once

#include "Cheatkey_ICommand.h"

namespace Cheatkey
{
	class RespawnObj : public ICommand
	{
	public:
		explicit RespawnObj(ILimitPred* p) : ICommand(p) {}
		virtual ~RespawnObj() {}

		static ICommand* Creator();

	private:
		virtual bool _Execute() override;
	};

	class SetObjDamage : public ICommand
	{
		enum 
		{
			TEAMRED_OBJ,
			TEAMBLUE_OBJ,
			HELICOPTER,
			OBJ_MAX_COUNT,
		};

	public:
		explicit SetObjDamage(ILimitPred* p) : ICommand(p) {}
		virtual ~SetObjDamage() {}

		static ICommand* Creator();

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;

	private:
		INT32 m_idx, m_damage;
	};
}