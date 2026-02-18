#pragma once

#include "Cheatkey_ICommand.h"

namespace Cheatkey
{
	class RespawnWaitTimeCtrl : public ICommand
	{
	public:
		explicit RespawnWaitTimeCtrl(ILimitPred* p);
		virtual ~RespawnWaitTimeCtrl() {}

		static ICommand* Creator();

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;

	private:
		REAL32 m_waittime, m_numOfAvail;
	};

	class RespawnPosCtrl : public ICommand
	{
	public:
		explicit RespawnPosCtrl(ILimitPred* p) : ICommand(p) {}
		virtual ~RespawnPosCtrl() {}

		static ICommand* Creator();

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;

	private:
		VEC3D m_pos;
	};

	
}