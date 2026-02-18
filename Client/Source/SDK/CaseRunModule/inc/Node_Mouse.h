#pragma once

#include "CaseRunDefine.h"
#include "Node_UIBase.h"
#include "Job.h"

namespace CaseRun
{
	///////////////////////////////////////////////////////////////
	// Click
	class Node_Click : public Node_UIBase
	{
	protected:
		Job_Click 			m_Job;
		wstring				m_SearchText;
		bool				m_bMustBeSame;

	public:
		Node_Click(void);
		
		void		set( const wstring & path, const wstring & search_txt, bool must_be_same, BUTTON btn, bool bDblClick);

		virtual void		getInfoString(std::wstring & str);
		virtual void		Serialize(Serializer & file, JSONNODE * pJSON);
		virtual bool		Deserialize(JSONNODE * pJSON);
		
		virtual TESTRESULT	OnWork(  Interface * pInterface, float tm);
	};

	///////////////////////////////////////////////////////////////
	// MouseDown
	class Node_MouseDown : public NodeBase
	{
	public:
		Job_MouseDown			m_Job;
	protected:


	public:
		Node_MouseDown(void);
		
		void		set( BUTTON btn, float duration);

		virtual void		getInfoString(std::wstring & str);
		virtual void		Serialize(Serializer & file, JSONNODE * pJSON);
		virtual bool		Deserialize(JSONNODE * pJSON);
		virtual TESTRESULT	DoTest(Interface * pInterface, float tm);
	};
};

