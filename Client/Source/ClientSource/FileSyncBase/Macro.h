#ifndef		__MACRO_H__
#define		__MACRO_H__




#define			TEST_CODE			// 테스트 코드
#define			NEEDED_IMPROVE		// 개선이 필요한 코드
#define			NO_IMP				// 구현이 안된 코드



#define		SAFE_CLOSE_HANDLE(h)	\
	if (h != INVALID_HANDLE_VALUE)	{CloseHandle(h);h = INVALID_HANDLE_VALUE;}

#define		SAFE_CLOSE_INTERNET_HANDLE(h)	\
	if (h != nullptr)	{InternetCloseHandle(h);h = nullptr;}

#define		SAFE_CLOSE(p)	\
	if (p)	{p->Close();}

template<typename T >
inline	void	DELETE_ALL(T& container)
{
	for (T::iterator iter = container.begin() ;
		iter != container.end() ; ++iter)
	{
		delete (*iter);
	}
	container.clear();
}

template<typename T>
inline	void	SAFE_DELETE(T& p)
{
	if (p)
	{
		delete p;
		p = nullptr;
	}
}

template<typename T>
inline	void	SAFE_DELETE_ARRAY(T& p)
{
	if (p)
	{
		delete [] p;
		p = nullptr;
	}
}



#endif