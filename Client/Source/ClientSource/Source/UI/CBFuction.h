#if !defined( __UI_CBFuction_H__)
#define __UI_CBFuction_H__

#include "i3Base/itl/map.h"

#include "UIDefine.h"

/*	
 *	CBFuction 은 콜백함수와 같은 기능에 클래스이다.(http://blog.naver.com/yiyousung?Redirect=Log&logNo=80113313235)
	
	UI::insert_cb("test", Ctest, &Ctest::cb_test); //등록 이름, 콜백 함수를 가지고 있는 클래스, 콜백 함수
	UI::call_cb("test"); //test 이름으로 등록된 함수를 호출한다.
	UI::remove_cb("test"); //등록된 'test' 콜백함수 제거
	UI::remove_cb(this); //해당 클래스 포인터로 등록된 모든 콜백함수 제거
*/

namespace UI
{
struct VBase { virtual ~VBase() {}  };
struct ICallback : VBase {     virtual void operator()(void* arg1, void* arg2) = 0;   };

template<class T>
class Callback : public ICallback
{
private:
	typedef void (T::*Func)(void* arg1, void* arg2);

	T* m_pOwner;
	Func m_pfFunc;

public:
    Callback(T* pOwner, Func pfFunc) : m_pOwner(pOwner), m_pfFunc(pfFunc) {}
	T* get_owner() const {	return m_pOwner; }

    virtual void operator()(void* arg1, void* arg2) {	(m_pOwner->*m_pfFunc)(arg1, arg2);    }
};

class Callback_fn : public ICallback
{
private:
	typedef void (*fn_type)(void*, void*);
	fn_type	m_fn;
public:
	Callback_fn(fn_type fn) : m_fn(fn) {}

	virtual void operator()(void* arg1, void* arg2) { m_fn(arg1, arg2); }
};



/*******************************************/
template<class K>
class CBFuction : public i3::shared_ginst< CBFuction< K > >
{
	typedef i3::pair< ICallback*, void* > CallBackValue;
	typedef i3::multimap< K, CallBackValue > CallBackList;

	CallBackList mCallBackList;

public:
	CBFuction() {}
	~CBFuction();

public:
	void insert( const K& key, ICallback* pCallback, void * p); //콜백함수 등록
	void insert(const K& Key, ICallback* pCallback);
	void remove(const K& key);	//동일 키 값 제거
	void remove(void* owner);	//같은 클래스에 있는 callback 함수 일괄 제거
	void remove(const K& key, void* owner);	//한개만 제거한다.

	void call(const K& key, void* arg1 = nullptr, void* arg2 = nullptr); //등록된 모든 콜백함수 호출
	void call_target(const K& key, void* owner, void* arg1 = nullptr, void* arg2 = nullptr); //중복 등록이 가능하기 때문에 특정 클래스 콜백 함수 호출하기 위한 함수
};


template<class K>
CBFuction< K >::~CBFuction()
{
	CallBackList::iterator end = mCallBackList.end();
	for(CallBackList::iterator it = mCallBackList.begin(); it != end;	++it)
	{
		//I3PRINTLOG(I3LOG_NOTICE,"해재 안 된 [%s]CallBack 이다.", it->first.c_str()); //Key 가 std::string 이 아니면 망함
		CallBackValue & value = it->second;
		I3_SAFE_DELETE( value.first);
	}
	mCallBackList.clear();
}

template<class K> inline
void CBFuction< K >::insert(const K& key, ICallback* pCallback, void * p)
{
	mCallBackList.insert( CallBackList::value_type( key, CallBackValue(pCallback,p) ) );
}

template<class K> inline
void CBFuction<K>::insert(const K& Key, ICallback* pCallback)
{
	mCallBackList.insert( CallBackList::value_type(Key, CallBackValue(pCallback, nullptr)));
}

template<class K>
void CBFuction< K >::remove(const K& key)
{
	CallBackList::iterator it = mCallBackList.lower_bound( key );
	CallBackList::iterator end = mCallBackList.upper_bound( key );

	while( it != end )
	{
		CallBackValue & value = it->second;
		I3_SAFE_DELETE( value.first);
		++it;
	}

	mCallBackList.erase( key );
}

template<class K>
void CBFuction< K >::remove(void* owner)
{
	CallBackList::iterator it = mCallBackList.begin();

	while( it != mCallBackList.end() )
	{
		CallBackValue & value = it->second;
		if(value.second == owner)
		{
			I3_SAFE_DELETE( value.first);
			it = mCallBackList.erase( it );
		}
		else
			++it;
	}
}

template<class K>
void CBFuction< K >::remove(const K& key, void* owner)
{
	CallBackList::iterator it = mCallBackList.lower_bound( key );
	CallBackList::iterator end = mCallBackList.upper_bound( key );

	while( it != end )
	{
		CallBackValue & value = it->second;
		if(value.second == owner)
		{
			I3_SAFE_DELETE( value.first);
			it = mCallBackList.erase( it );
		}
		else
			++it;
	}
}

template<class K>
void CBFuction< K >::call(const K& key, void* arg1, void* arg2)
{
	CallBackList::iterator it = mCallBackList.lower_bound( key );
	CallBackList::iterator end = mCallBackList.upper_bound( key );
	while( it != end )
	{
		CallBackValue & value = it->second;
		(*value.first)(arg1, arg2);
		++it;
	}
}

template<class K>
void CBFuction< K >::call_target(const K& key, void* owner, void* arg1, void* arg2)
{
	CallBackList::iterator it = mCallBackList.lower_bound( key );
	CallBackList::iterator end = mCallBackList.upper_bound( key );
	while( it != end )
	{
		CallBackValue & value = it->second;
		if(value.second == owner)
		{
			(*value.first)(arg1, arg2);
		}
		++it;
	}
}


//- 인터페이스 쉽게 한 것 --//
template< class T >
inline void insert_cb(const std::string & k,  T* p, void (T::*f)(void*,void*)  )
{
	Callback< T > * pCb = new Callback< T >(p, f );
	MEMDUMP_NEW( pCb, sizeof( Callback< T >));
	CBFuction< std::string >::i()->insert(k, pCb, p );
}

inline void insert_cb(const std::string& key, void (*fn)(void*,void*))
{
	Callback_fn* pCb = new Callback_fn(fn);
	MEMDUMP_NEW( pCb, sizeof( Callback_fn));
	CBFuction<std::string>::i()->insert(key, pCb);

}


inline void remove_cb( const std::string & k )
{
	CBFuction< std::string >::i()->remove(k);
}

inline void remove_cb( void* p )
{
	CBFuction< std::string >::i()->remove(p);
}

inline void remove_cb( const std::string & k, void* p )
{
	CBFuction< std::string >::i()->remove(k, p);
}

inline void call_cb( const std::string& k, void * arg1 = nullptr, void * arg2 = nullptr)
{
	CBFuction< std::string >::i()->call(k, arg1, arg2);
}

inline void call_cb_target( const std::string& k, void* p, void* arg1 = nullptr, void* arg2 = nullptr)
{
	CBFuction< std::string >::i()->call_target(k, p, arg1, arg2);
}

}	//end of GameUI namespace
#endif
