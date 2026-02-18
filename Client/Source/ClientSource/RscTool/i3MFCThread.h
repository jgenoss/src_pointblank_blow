#pragma once

//
// CWinThread 파생클래스 따위를 만들지 않고 대충 AfxBeginThread 함수만 쓴다.
//

//
// .. 해당 콜백에 상태멤버가 들어간다면, 이것들은 공유변수처럼 작동되므로..
// .. 보호처리 혹은 락프리 처리를 고려해야한다.. (예측동작: 메인쓰레드에서 생성, 부쓰레드에서 접근 )
// 

//
// callback인수 i3MFCThread포인터가 유지되려면, 콜백함수호출동안 i3MFCThread객체가
// 지워지면 안된다... 해당 부분 처리가 미흡하므로, 나중에 필요하면 넣어줘야한다..
//

template<class Derived>
class i3MFCThreadCallback : i3::class_common_pool<i3MFCThreadCallback<Derived> >
{
protected:
	void operator()(class i3MFCThread* pthread) {}			// 일단은 함수객체화한다..
private:
	void Run(i3MFCThread* pthread) { (*static_cast<Derived*>(this))(pthread); }
	template<class MFCThreadCallback>
	friend UINT __cdecl i3MFCThreadFunc( LPVOID pParam);
};

//
// 파생클래스 따위 만들지 말것....컨트롤러의 역할만 갖는다..
// 적당한 함수로 생성시키는 것 이외 필요한 기능은 그때그때 추가한다...
// 캐시라인유지를 위해 64바이트크기를 맞추는게 좋다.
// 그리고 이클래스는 가상함수를 가지지 말아야한다.

#include "i3Base/smart_ptr/enable_shared_from_this.h"
#include "i3Base/smart_ptr/weak_ptr.h"


typedef i3::shared_ptr<i3MFCThread>		i3MFCThreadPtr;

i3MFCThreadPtr Createi3MFCThread();

__declspec( align(64) )
class i3MFCThread : public i3::class_common_pool<i3MFCThread, i3::enable_shared_from_this<i3MFCThread> >
{
public:
	i3MFCThread() { }

	template<class MFCThreadCallback>
	void Call(i3::identity<MFCThreadCallback> );
	
private:
	struct Data 
	{
		Data() : m_mfcThread(NULL) {}
		CWinThread*	m_mfcThread;						
	};
	Data		m_Data;
	size_t		m_aligned_dummy[ (64 - sizeof(Data)) / sizeof(size_t) ];
};


// 이 함수는 한번만 실행되는 함수이다.
template<class MFCThreadCallback>
UINT __cdecl i3MFCThreadFunc( LPVOID pParam) 
{	
	i3MFCThread* owner = static_cast<i3MFCThread*>(pParam);

	i3MFCThreadPtr owner_ptr = owner->shared_from_this();	// 이 시점에서 owner는 유효해야한다..

	// 쓰레드 내부에서 콜백 객체를 스택으로 생성...
	MFCThreadCallback callback;
	callback.Run(owner);

	return 0;
	// 일단 이렇게 처리..
}


template<class MFCThreadCallback>
void i3MFCThread::Call(i3::identity<MFCThreadCallback> )
{
	m_Data.m_mfcThread = ::AfxBeginThread(&i3MFCThreadFunc<MFCThreadCallback>, this);
	m_Data.m_mfcThread->m_bAutoDelete = TRUE;
}

inline
i3MFCThreadPtr Createi3MFCThread() 
{
	return i3::make_shared<i3MFCThread>();
}
