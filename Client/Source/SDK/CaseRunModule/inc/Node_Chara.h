#pragma once

#include "CaseRunDefine.h"

#include "NodeBase.h"

namespace CaseRun
{
	///////////////////////////////////////////////////////
	// MoveTo
	class Node_MoveTo : public NodeBase
	{
	public:
		LOCTYPE				m_LocType;
		std::wstring		m_Location;

	public:
		Node_MoveTo(void);

		virtual void		getInfoString(std::wstring & str);
		virtual void		Serialize(Serializer & file, JSONNODE * pJSON);
		virtual bool		Deserialize(JSONNODE * pJSON);
		virtual TESTRESULT	DoTest( Interface * pInterface, float tm);
	};

	///////////////////////////////////////////////////////
	// LookAt
	class Node_LookAt : public NodeBase
	{
	public:
		DIRTYPE				m_DirType;
		std::wstring		m_Direction;

	public:
		Node_LookAt(void);

		virtual void		getInfoString(std::wstring & str);
		virtual void		Serialize(Serializer & file, JSONNODE * pJSON);
		virtual bool		Deserialize(JSONNODE * pJSON);
		virtual TESTRESULT	DoTest( Interface * pInterface, float tm);
	};
};
