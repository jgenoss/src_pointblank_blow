#pragma once

#include "WndProcThunk.h"


bool GetPathFromAbsPidl(PCIDLIST_ABSOLUTE pidl, wchar_t* outstrPath_MAXPATH);
bool GetPathFromAbsPidl(PCIDLIST_ABSOLUTE pidl, char* outstrPath_MAXPATH);

bool GetDisplayNameFromAbsPidl(PCIDLIST_ABSOLUTE pidl, wchar_t* outstrPath_MAXPATH);

//
// 경로문자열이 아닌 PIDL을 콜백인수로 지정했다...
// 경로문자열을 얻기 위해서... 획득함수를 만들어야한다...
//
//
// 주목할 만한 사항으로 상위 디렉토리 삭제가 있는 경우, 하위 내용들의 갱신 콜백까지 처리하지 않는다는 특징이 있고 미리 숙지해둬야 함. (셸 기준으로만 따지는듯)
// ( 만약 그러한 재귀적 처리가 필요한 경우엔 이 모듈이 아닌 별도의 외부 처리가 요구된다.)
// 재귀처리를 안하는 점은  다소간 속도저하 예방이라는 장점이 될수도 있다.
//
//  아이콘 오버레이의 업데이트 감시의 경우, 해당 어플리케이션이 Notify처리를 해줬느냐 아니냐에 따라 실행 여부가 결정된다...
//
// 추상화까지는 필요가 없어서, =0 처리는 하지 않았다.

#pragma warning(push)
#pragma warning(disable : 4100)

struct ShellChangeCallback
{
	virtual void OnGeneral(long SHCNE_event, PIDLIST_ABSOLUTE pidl1, PIDLIST_ABSOLUTE pidl2 ) {}
	virtual void OnCreate_Directory( PIDLIST_ABSOLUTE pidl) {}	
	virtual void OnDelete_Directory( PIDLIST_ABSOLUTE pidl) {}	
	virtual void OnRename_Directory( PIDLIST_ABSOLUTE from_pidl, PIDLIST_ABSOLUTE to_pidl ) {}	

	virtual void OnCreate_File( PIDLIST_ABSOLUTE pidl) {}	
	virtual void OnDelete_File( PIDLIST_ABSOLUTE pidl) {}	
	virtual void OnRename_File( PIDLIST_ABSOLUTE from_pidl, PIDLIST_ABSOLUTE to_pidl ) {}	

	virtual void OnUpdate_Directory(PIDLIST_ABSOLUTE pidl) {} 
	virtual void OnUpdate_File( PIDLIST_ABSOLUTE pidl) {}
};


#pragma  warning(pop)

class ShellChangeWatcher
{
public:
	ShellChangeWatcher();
	~ShellChangeWatcher();

	HRESULT Start(HWND hwnd, const wchar_t* wstrPath);
	HRESULT Start(HWND hwnd, PIDLIST_ABSOLUTE pidl);
	void	Stop();

	void	SetCallback(ShellChangeCallback* cb) { m_callback = cb; }
	ShellChangeCallback*	GetCallback() const { return m_callback; }
private:
	static LRESULT CALLBACK StaticWndProc(HWND, UINT, WPARAM, LPARAM);
	LRESULT MemberWndProc(UINT, WPARAM, LPARAM);

	ULONG	m_ulRegister;
	long	m_SHCNE_event;
	BOOL	m_recursive;			// ShellAPI에서 요구하는 타입에 기인한 대문자bool.
	HWND			m_hwnd;
	WNDPROC			m_oldProc;
	WndProcThunk	m_thunk;
	ShellChangeCallback*	m_callback;	
};




