#pragma once

#include "Cheatkey_ICommand.h"

namespace Cheatkey
{
	class Collision : public ICommand
	{
	public:
		explicit Collision(ILimitPred* p) : ICommand(p) {}
		virtual ~Collision() {}

		static ICommand* Creator();

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;

	private:
		INT32 m_iValue[2];
	};

	class ThrowCollisionBox : public ICommand
	{
	public:
		explicit ThrowCollisionBox(ILimitPred* p) : ICommand(p) {}
		virtual ~ThrowCollisionBox() {}

		static ICommand* Creator();

	private:
		virtual bool _Execute() override;
	};
}

