#pragma once

#include <atlstdthunk.h>				// ATL 구현을 쓴다...  (MFC를 쓰지 않는다면..참고해서 별도 구현한다..)

class WndProcThunk
{
public:
	void		Init(WNDPROC proc, void* pThis) { m_thunk.Init(reinterpret_cast<DWORD_PTR>(proc), pThis); }
	WNDPROC		GetWNDPROC() { return reinterpret_cast<WNDPROC>(m_thunk.GetCodeAddress()); }
private:
	CStdCallThunk		m_thunk;
};
