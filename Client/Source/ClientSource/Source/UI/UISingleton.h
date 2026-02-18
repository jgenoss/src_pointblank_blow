#if !defined( __UI_SINGLETON_H__)
#define __UI_SINGLETON_H__

namespace UI	{

template<class T>
class Singleton
{
private:
	static T ms_Singleton;

public:
	static T& getSingleton()	{		
		return ms_Singleton;	}
	static T* getSingletonPtr()	{
		return &ms_Singleton;	}

	static T& get()	{		
		return ms_Singleton;	}
	static T* getPtr()	{
		return &ms_Singleton;	}

};
template <class T> T Singleton<T>::ms_Singleton;

}
#endif