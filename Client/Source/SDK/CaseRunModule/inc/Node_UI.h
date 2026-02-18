#pragma once

#include "CaseRunDefine.h"
#include "Node_UIBase.h"

namespace CaseRun
{
	///////////////////////////////////////////////////////////////
	// Input
	class Node_Input : public Node_UIBase
	{
	public:
		std::wstring		m_Input;

	public:
		Node_Input(void);

		virtual void		getInfoString(std::wstring & str);
		virtual void		Serialize(Serializer & file, JSONNODE * pJSON);
		virtual bool		Deserialize(JSONNODE * pJSON);
		virtual TESTRESULT	OnWork(Interface * pInterface, float tm);
	};

	///////////////////////////////////////////////////////////////
	// Select
	class Node_Select : public Node_UIBase
	{
	public:
		wstring				m_Item;
		bool				m_bMustBeSame;
		bool				m_bDblClick;

	protected:


	public:
		Node_Select(void);

		virtual void		getInfoString(std::wstring & str);
		virtual void		Serialize(Serializer & file, JSONNODE * pJSON);
		virtual bool		Deserialize(JSONNODE * pJSON);
		virtual TESTRESULT	OnWork( Interface * pInterface, float tm);
	};

	///////////////////////////////////////////////////////////////
	// CheckText
	class Node_CheckText : public Node_UIBase
	{
	protected:
		wstring				m_Text;
		bool				m_bMustBeSame;
		bool				m_bSubControl;

	protected:


	public:
		Node_CheckText(void);

		virtual void		getInfoString(std::wstring & str);
		virtual void		Serialize(Serializer & file, JSONNODE * pJSON);
		virtual bool		Deserialize(JSONNODE * pJSON);
		virtual TESTRESULT	OnWork( Interface * pInterface, float tm);
	};

	///////////////////////////////////////////////////////////////
	// CheckState
	class Node_CheckState : public Node_UIBase
	{
	protected:
		CTRL_STATE			m_State;

	protected:


	public:
		Node_CheckState(void);

		virtual void		getInfoString(std::wstring & str);
		virtual void		Serialize(Serializer & file, JSONNODE * pJSON);
		virtual bool		Deserialize(JSONNODE * pJSON);
		virtual TESTRESULT	OnWork(  Interface * pInterface, float tm);
	};

	///////////////////////////////////////////////////////////////
	// CheckTopWindow
	class Node_CheckTopWindow : public Node_UIBase
	{
	protected:
	protected:

	public:
		Node_CheckTopWindow(void);

		virtual TESTRESULT	OnWork(  Interface * pInterface, float tm);
	};
}
