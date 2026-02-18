#pragma once

#include "Cheatkey_ICommand.h"

namespace Cheatkey
{
	class Toggle3pCamera : public ICommand
	{
	public:
		explicit Toggle3pCamera(ILimitPred* p) : ICommand(p) {}
		virtual ~Toggle3pCamera() {}

		static ICommand* Creator();

	private:
		virtual bool _Execute() override;
	};

	class ToggleFlyCamera : public ICommand
	{
	public:
		explicit ToggleFlyCamera(ILimitPred* p) : ICommand(p) {}
		virtual ~ToggleFlyCamera() {}

		static ICommand * Creator();

	private:
		virtual bool _Execute() override;
	};

	class TopViewCamera : public ICommand
	{
	public:
		explicit TopViewCamera(ILimitPred* p) : ICommand(p) {}
		virtual ~TopViewCamera() {}

		static ICommand * Creator();

	private:
		virtual bool _Execute() override;
	};
}