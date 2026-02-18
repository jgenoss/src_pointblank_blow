#pragma once

#include "Cheatkey_ICommand.h"

namespace Cheatkey
{
	class UIToggle : public ICommand
	{
	public:
		explicit UIToggle(ILimitPred* p) : ICommand(p) {}
		virtual ~UIToggle() {}

		static ICommand* Creator();

	private:
		virtual bool _Execute() override;
	};

	class SlotInfo : public ICommand
	{
	public:
		explicit SlotInfo(ILimitPred* p) : ICommand(p) {}
		virtual ~SlotInfo() {}

		static ICommand* Creator();

		static const wchar_t* SlotStateToStr(SLOT_STATE state);
		static const wchar_t* SlotNickNameToStr(i3::rc_wstring& nickStr);

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;
	};

	class ViewPosition : public IRunnableCommand
	{
	public: 
		ViewPosition(ILimitPred* p) : IRunnableCommand(p) {}
		virtual ~ViewPosition() {}

		static IRunnableCommand* Creator();

	private:
		virtual void _Execute(const REAL32 tm) override;
	};
}

class PBCmd_ViewPosition : public PBConsoleCmd
{
public:
	virtual CMD_RESULT_FLAG ExecuteCommand( i3::vector<i3::string> &params) override;
	virtual CMD_RESULT_FLAG RunCommand(REAL32 DeltaSec) override;
};