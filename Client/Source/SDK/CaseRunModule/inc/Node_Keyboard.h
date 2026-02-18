#pragma once

#include "CaseRunDefine.h"
#include "NodeBase.h"
#include "Job.h"

namespace CaseRun
{
	///////////////////////////////////////////////////////////////
	// KeyDown
	class Node_KeyDown : public NodeBase
	{
	public:
		Job_KeyDown			m_Job;
		
	protected:

	public:
		Node_KeyDown(void);

		const wchar_t *		getKeyName(void) const		{ return m_Job.m_KeyName.c_str(); }
		void				setKeyName( const wchar_t * pszKey)
		{
			m_Job.m_KeyName = pszKey;
		}

		float				getDuration(void) const		{ return m_Job.m_Duration; }
		void				setDuration( float secs)	{ m_Job.m_Duration = secs; }

		virtual void		getInfoString(std::wstring & str);
		virtual void		Serialize(Serializer & file, JSONNODE * pJSON);
		virtual bool		Deserialize(JSONNODE * pJSON);
		virtual TESTRESULT	DoTest( Interface * pInterface, float tm);
	};
};

