#pragma once
class CLogBase
{
protected:
	i3::string m_name;
	i3::deque<i3::string> m_inout;

public:
	CLogBase();
	~CLogBase();

	virtual void Init(i3::string sWorkBase, i3::string sLogName){}

	i3::string GetInOut();
	i3::string GetName() { return m_name; }
};

