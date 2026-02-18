#pragma once

#include "CaseRunDefine.h"
#include "NodeBase.h"

namespace CaseRun
{
	///////////////////////////////////////////////////////
	//
	class Node_Store : public NodeBase
	{
	public:
		std::wstring		m_Variable;
		std::wstring		m_Value;

	public:
		Node_Store(void);

		virtual void		getInfoString(std::wstring & str);
		virtual void		Serialize(Serializer & file, JSONNODE * pJSON);
		virtual bool		Deserialize(JSONNODE * pJSON);
		virtual TESTRESULT	DoTest( Interface * pInterface, float tm);
	};

	///////////////////////////////////////////////////////
	//
	class Node_Load : public NodeBase
	{
	public:
		std::wstring		m_Variable;
		std::wstring		m_CtrlID;

	public:
		Node_Load(void);

		virtual void		getInfoString(std::wstring & str);
		virtual void		Serialize(Serializer & file, JSONNODE * pJSON);
		virtual bool		Deserialize(JSONNODE * pJSON);
		virtual TESTRESULT	DoTest( Interface * pInterface, float tm);
	};

	///////////////////////////////////////////////////////
	//
	class Node_Calc : public NodeBase
	{
	public:
		std::wstring		m_V1;
		std::wstring		m_V2;
		std::wstring		m_V3;
		VAROP				m_Op;

	public:
		Node_Calc(void);

		virtual void		getInfoString(std::wstring & str);
		virtual void		Serialize(Serializer & file, JSONNODE * pJSON);
		virtual bool		Deserialize(JSONNODE * pJSON);
		virtual TESTRESULT	DoTest( Interface * pInterface, float tm);
	};

	///////////////////////////////////////////////////////
	//
	class Node_Compare : public NodeBase
	{
	public:
		std::wstring		m_V1;
		std::wstring		m_V2;
		VARCOMPARE			m_Func;

	public:
		Node_Compare(void);

		virtual void		getInfoString(std::wstring & str);
		virtual void		Serialize(Serializer & file, JSONNODE * pJSON);
		virtual bool		Deserialize(JSONNODE * pJSON);
		virtual TESTRESULT	DoTest( Interface * pInterface, float tm);
	};
};
