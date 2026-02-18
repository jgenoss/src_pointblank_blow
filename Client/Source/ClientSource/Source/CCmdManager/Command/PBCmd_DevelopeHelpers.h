#pragma once

#include "Cheatkey_ICommand.h"

namespace Cheatkey
{
	class Popup : public ICommand
	{
	public:
		explicit Popup(ILimitPred* p) : ICommand(p), m_popupIdx(0), m_open(false) {}
		virtual ~Popup() {}

		static ICommand* Creator();

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;

	private:
		UINT32 m_popupIdx;
		bool m_open;
	};

	class MessageBox : public ICommand
	{
	public:
		explicit MessageBox(ILimitPred* p) : ICommand(p), m_open(false) {}
		virtual ~MessageBox() {}

		static ICommand* Creator();

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;

	private:
		struct Message
		{
			i3::wstring message;
			i3::wstring caption;
			INT32 messageType = 1;	// MSG_TYPE_GAME_QUERY
			INT32 fontSize = 10;
		};

		Message m_msg;
		bool m_open = false;
	};

	class ShowTabminimapProps : public ICommand
	{
	public:
		explicit ShowTabminimapProps(ILimitPred* p) : ICommand(p), m_show(false) {}
		virtual ~ShowTabminimapProps() {}

		static ICommand* Creator();

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;

	private:
		bool m_show;
	};

	class ShowProfile : public ICommand
	{
	public:
		explicit ShowProfile(ILimitPred* p) : ICommand(p), m_show(false) {}
		virtual ~ShowProfile() {}

		static ICommand* Creator();

	private:
		virtual bool _Execute() override;

	private:
		bool m_show;
	};
}