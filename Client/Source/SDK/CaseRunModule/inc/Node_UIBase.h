#pragma once

#include "CaseRunDefine.h"
#include "NodeBase.h"

namespace CaseRun
{
	class Node_UIBase : public NodeBase
	{
	private:
		enum STEP
		{
			STEP_FINDUI,
			STEP_WAIT,
			STEP_WORK,
		};

		// Volatile
		
		STEP				m_Step;
		RECT				m_OldRect;
		bool				m_bFirstTime;

	public:
		std::wstring		m_CtrlPath;

	
	private:
		void			SetStep(STEP step, float tm);

	public:
		Node_UIBase( const wchar_t * pszVerb);

		virtual void		getInfoString(std::wstring & str);
		virtual void		Serialize(Serializer & file, JSONNODE * pJSON);
		virtual bool		Deserialize(JSONNODE * pJSON);
		virtual TESTRESULT	DoTest( Interface * pInterface, float tm);

		virtual TESTRESULT	OnWork( Interface * pInterface, float tm) = 0;
	};
}
