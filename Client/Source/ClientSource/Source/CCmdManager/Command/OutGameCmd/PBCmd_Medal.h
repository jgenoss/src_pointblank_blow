#pragma once

#include "Cheatkey_ICommand.h"

namespace Cheatkey
{
	class ChangeMedalCount : public ICommand
	{
	public:
		explicit ChangeMedalCount(ILimitPred* p) : ICommand(p), m_type(0), m_idx(0), m_count(0) {}
		virtual ~ChangeMedalCount() {}

		static ICommand* Creator();

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;

	private:
		UINT8 m_type;
		UINT16 m_idx;
		UINT16 m_count;
	};

	class ChangeMedalSet : public ICommand
	{
	public:
		explicit ChangeMedalSet(ILimitPred* p) : ICommand(p), m_type(0), m_idx(0) {}
		virtual ~ChangeMedalSet() {}

		static ICommand* Creator();

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;

	private:
		UINT8 m_type; 
		UINT16 m_idx;
	};
}