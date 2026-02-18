#pragma once
#include "LogBase.h"

class CLocalLogNet : public CLogBase
{
public:
	CLocalLogNet();
	~CLocalLogNet();

	HANDLE m_hTread;
	UINT m_dwThreadID;
	bool m_State;
	

	HANDLE	m_hPipe;
	void CreatePipeProcess(i3::string command);
	void WatchPipeProcess(i3::string &inout);

	void ReadPipeData();

	virtual void Init(i3::string sWorkBase, i3::string sLogName);


};

