#pragma once

#include "CaseRunDefine.h"

#include "NodeBase.h"

namespace CaseRun
{
	///////////////////////////////////////////////////////
	//
	class Node_Custom : public NodeBase
	{
	public:
		std::wstring		m_Function;
		std::wstring		m_Param1;
		std::wstring		m_Param2;

	public:
		Node_Custom(void);

		virtual void		getInfoString(std::wstring & str);
		virtual void		Serialize(Serializer & file, JSONNODE * pJSON);
		virtual bool		Deserialize(JSONNODE * pJSON);
		virtual TESTRESULT	DoTest( Interface * pInterface, float tm);
	};
};
