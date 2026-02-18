#pragma once

#include "Cheatkey_ICommand.h"

namespace Cheatkey
{
	class Null : public ICommand
	{
	public:
		explicit Null(ILimitPred* p) : ICommand(p) {}
		virtual ~Null() {}

	private:
		virtual bool _Execute()  override { return false; }
	};
}