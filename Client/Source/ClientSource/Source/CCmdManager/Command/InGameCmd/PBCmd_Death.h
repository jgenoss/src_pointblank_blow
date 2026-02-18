#pragma once

#include "Cheatkey_ICommand.h"

namespace Cheatkey
{
	class Suicide : public ICommand
	{
	public:
		explicit Suicide(ILimitPred* p) : ICommand(p) {}
		virtual ~Suicide() {}

		static ICommand* Creator();

	private:
		virtual bool _Execute() override;

	};

	class KillemAll : public ICommand
	{
	public:
		explicit KillemAll(ILimitPred* p) : ICommand(p) {}
		virtual ~KillemAll() {}

		static ICommand* Creator();

	private:
		virtual bool _Execute() override;
	};
}