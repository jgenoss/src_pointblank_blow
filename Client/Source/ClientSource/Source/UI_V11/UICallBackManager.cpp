#include "pch.h"

#include "UICallBackManager.h"

namespace UI
{

//마지막에 자동으로 Release 하기 때문에, 메모리 누수 걱정은 안 해도 된다.
//등록된 CallBack 함수는 반드시 Release 해야 되는건 아니지만
//그래도 사용 후에 Release 하면 좋을 것 같다.
CallBackManager::~CallBackManager()
{
	CallBackIntList::iterator int_end = mCallBackIntList.end();
	for(CallBackIntList::iterator it = mCallBackIntList.begin(); it != int_end;	++it)
	{
		I3PRINTLOG(I3LOG_NOTICE, "해재 안 된 [ %d ] IntCallBack 이다.", it->first );	
		delete it->second;
	}
	mCallBackIntList.clear();

	CallBackStringList::iterator str_end = mCallBackStringList.end();
	for(CallBackStringList::iterator it = mCallBackStringList.begin(); it != str_end;	++it)
	{
		I3PRINTLOG(I3LOG_NOTICE, "해재 안 된 [ %s ] StringCallBack 이다.", it->first.c_str() );
		delete it->second;
	}
	mCallBackStringList.clear();
}

void CallBackManager::insert(DWORD key, ICallback* pCallback)
{
	if( mCallBackIntList.find( key ) != mCallBackIntList.end() )
	{
		I3PRINTLOG(I3LOG_NOTICE, "이미 등록된 [ %d ] CallBack 이다.", key );
		return;
	}
	mCallBackIntList.insert( CallBackIntList::value_type( key, pCallback ) );
}

void CallBackManager::insert(const std::string & name, ICallback* pCallback)
{
	if( mCallBackStringList.find( name ) != mCallBackStringList.end() )
	{
		I3PRINTLOG(I3LOG_NOTICE, "이미 등록된 [ %s] CallBack 이다.", name.c_str() );
		return;
	}
	mCallBackStringList.insert( CallBackStringList::value_type( name, pCallback ) );
}

void CallBackManager::release(DWORD key)
{
	CallBackIntList::iterator it = mCallBackIntList.find( key );
	if( it != mCallBackIntList.end() )
	{
		delete it->second;
		mCallBackIntList.erase( it );
	}
}

void CallBackManager::release(const std::string & name)
{
	CallBackStringList::iterator it = mCallBackStringList.find( name );
	if( it != mCallBackStringList.end() )
	{
		delete it->second;
		mCallBackStringList.erase( it );
	}
}

bool CallBackManager::run(DWORD key, void* arg1, void* arg2)
{
	CallBackIntList::iterator it = mCallBackIntList.find( key );
	if( it == mCallBackIntList.end() )
		return false;

	it->second->run(arg1, arg2);
	return true;
}

bool CallBackManager::run(const std::string & name, void* arg1, void* arg2)
{
	CallBackStringList::iterator it = mCallBackStringList.find( name );
	if( it == mCallBackStringList.end() )
		return false;

	it->second->run(arg1, arg2);
	return true;
}

}