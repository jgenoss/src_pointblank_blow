#pragma once

#include "CaseRunDefine.h"
#include "NodeBase.h"

namespace CaseRun
{
	///////////////////////////////////////////////////////////////
	// Target Run
	class Node_TargetRun : public NodeBase
	{
	public:
		Node_TargetRun(void);

		virtual void		getInfoString(std::wstring & str);
	};

	///////////////////////////////////////////////////////////////
	// Target Exit
	class Node_TargetExit : public NodeBase
	{
	public:
		Node_TargetExit(void);

		virtual void		getInfoString(std::wstring & str);
		virtual TESTRESULT	DoTest( Interface * pInterface, float tm);
	};

	///////////////////////////////////////////////////////////////
	// Agent
	class Node_Agent : public NodeBase
	{
	protected:
		wstring			m_UserID;
		wstring			m_Password;
	public:
		Node_Agent(void);

		virtual void		getInfoString(std::wstring & str);
		virtual void		Serialize(Serializer & file, JSONNODE * pJSON);
		virtual bool		Deserialize(JSONNODE * pJSON);
		virtual TESTRESULT	DoTest( Interface * pInterface, float tm);
	};

	///////////////////////////////////////////////////////////////
	// Wait
	class Node_Wait : public NodeBase
	{
	public:
		float		m_Elapse;

	public:
		Node_Wait(void);

		virtual void		getInfoString(std::wstring & str);
		virtual void		Serialize(Serializer & file, JSONNODE * pJSON);
		virtual bool		Deserialize(JSONNODE * pJSON);
		virtual TESTRESULT	DoTest( Interface * pInterface, float tm);
	};
};

