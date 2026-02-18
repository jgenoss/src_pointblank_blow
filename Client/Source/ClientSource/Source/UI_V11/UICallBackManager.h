#if !defined( __UI_CALLBACK_MANAGER_H__)
#define __UI_CALLBACK_MANAGER_H__

#include "UI/UISingleton.h"

namespace UI
{
class ICallback
{
public:
    virtual void run(void* arg1, void* arg2) {};
};

template<class T>
class Callback : public ICallback
{
private:
	typedef void (T::*Func)(void* arg1, void* arg2);

	T* m_pOwner;
	Func m_pfFunc;

public:
    Callback(T* pOwner, Func pfFunc) : m_pOwner(pOwner), m_pfFunc(pfFunc) {}

    virtual void run(void* arg1, void* arg2) {	(m_pOwner->*m_pfFunc)(arg1, arg2);    }
};

class CallBackManager : public Singleton< CallBackManager >
{
	typedef std::map< UINT32, ICallback* > CallBackIntList;
	CallBackIntList mCallBackIntList;

	typedef std::map< std::string, ICallback* > CallBackStringList;
	CallBackStringList mCallBackStringList;

public:
	CallBackManager() {}
	~CallBackManager();

public:
    void insert( DWORD key, ICallback* pCallback );
	void insert( const std::string & name, ICallback* pCallback );
	void release(DWORD key);
	void release(const std::string & name);

	bool run(DWORD key, void* arg1 = 0, void* arg2 = 0);
	bool run(const std::string & name, void* arg1 = 0, void* arg2 = 0);
};

#define CREATE_CALLBACK( k, c, f ) \
	UI::CallBackManager::getSingleton().insert( k, new UI::Callback< c >( this, &c::f ) )

#define RELEASE_CALLBACK( k ) \
	UI::CallBackManager::getSingleton().release( k )

}	//end of GameUI namespace
#endif
