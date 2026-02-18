#include "pch.h"
#include "Cheatkey_ICommand.h"
#include "Cheatkey_LimitPred.h"

namespace Cheatkey
{
	ICommand::ICommand(ILimitPred* p) : m_pred(p) {}
	ICommand::~ICommand() 
	{
		I3_SAFE_DELETE(m_pred);
	}

	bool ICommand::operator()(const i3::vector<i3::string>& params)
	{
		if ((*m_pred)() == false) return false;

		return _ParseParams(params) ? _Execute() : false;
	}

	IRunnableCommand::IRunnableCommand(ILimitPred* p) : m_pred(p), m_tm(0.0f) {}
	IRunnableCommand::~IRunnableCommand() 
	{
		I3_SAFE_DELETE(m_pred);
	}

	bool IRunnableCommand::operator()(const REAL32 tm)
	{
		if ((*m_pred)() == false) return false;

		m_tm += tm;
		_Execute(m_tm);

		return true;
	}
}