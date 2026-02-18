#include "stdafx.h"
#include "LogBase.h"


CLogBase::CLogBase()
{
}


CLogBase::~CLogBase()
{
}

i3::string CLogBase::GetInOut()
{
	i3::string temp = "";

	if (m_inout.size() > 0)
	{
		temp = m_inout[0];
		m_inout.pop_front();
	}

	return temp;
}