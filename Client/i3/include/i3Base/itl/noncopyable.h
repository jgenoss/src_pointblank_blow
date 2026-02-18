#pragma once


// boost / stl / modern c++ design 등을 기준으로 네임스페이스를 사용하고, 공용 자료구조는 대부분 소문자와 언더바를 써서 표기법을 맞춤.

namespace i3
{
	

	class I3_EXPORT_BASE noncopyable
	{
	protected:
		noncopyable() {}
		~noncopyable() {}
	private:  // emphasize the following members are private
		noncopyable( const noncopyable& );
		const noncopyable& operator=( const noncopyable& );
	};
	
	



}