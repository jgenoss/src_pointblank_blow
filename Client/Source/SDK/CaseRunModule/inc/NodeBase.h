#pragma once

#include "CaseRunDefine.h"
#include "Interface.h"

namespace CaseRun
{
	class Serializer;

	class NodeBase
	{
		
	protected:
		const wchar_t *	m_pszVerb;
		std::wstring	m_Name;
		NodeBase *		m_pNext;
		float			m_Time;

		float			m_tmLocal;

	public:
		NodeBase(const wchar_t * pszVerb);

		const wchar_t *	getVerb(void) const		{ return m_pszVerb; }

		const wstring & getName(void) const		{ return m_Name; }
		void			setName(const wchar_t * pszName)
		{
			m_Name = pszName;
		}

		NodeBase *		getNext(void);
		void			setNext( NodeBase * pNext);

		float			getTime(void) const;
		void			setTime(float tm);

		void			resetLocalTime(void);
		void			addLocalTime(float tm);

		virtual void	getInfoString(std::wstring & str);
		virtual void	Serialize( Serializer & file, JSONNODE * pJSON);
		virtual bool	Deserialize(JSONNODE * pJSON);
		virtual TESTRESULT DoTest( Interface * pInterface, float tm);
	};
};

