#pragma once

#include "Cheatkey_ICommand.h"
#include "i3Base/itl/map.h"

namespace Cheatkey
{
	bool ParseCommand(const char* cheatkey, i3::string& outKeyword, i3::vector<i3::string>& outParams);

	i3::shared_ptr<ICommand> CreateCommand(const i3::string& keyword);
	i3::shared_ptr<ICommand> CreateCommand(const char* keyword);

	class Context : public i3::shared_ginst<Context>
	{
		typedef IRunnableCommand* (*CmdCreator)();

	public:
		Context() {}
		~Context();

		void AddRunnableCommand(CmdCreator key, const i3::vector<i3::string>& params);
		void DelRunnableCommand(CmdCreator key);
		void OnUpdate(const REAL32 tm);

	private:
		typedef i3::map<CmdCreator, i3::vector<IRunnableCommand*>*> CommandMapType;
		CommandMapType m_commands;
	};
}