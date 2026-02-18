#pragma once

//
//		암시적 operator bool()의 전형적 문제점은 
//      1. bool 아닌 정수형타입에 대한 모든 변환이 이루어진다는 점..
//		2. 상등/대소비교시
//      3. 완전이 다른 타입인데도 비명시적 정수형 생성자쪽으로 대입되는 점
//      의도치 않은 bool변환이 이루어질수 있는데..이런걸 막아주는 방식이 요구됨..
//      부스트쪽 구현버전은 3종류나 되는데 모두 인터페이스적으로 아쉬워서 이것저것 참고해서 그냥 수정..

//
//		연산자 구현에 대해서는 operator typename safe_bool<T>::type () const { return safe_bool<T>::call(cond); }
//

namespace i3
{
	template<class Target>
	class safe_bool
	{
	public:
		typedef safe_bool	this_type;
		typedef void (safe_bool::*unspecified_bool_type)() const;
		typedef unspecified_bool_type	type;
		static type call(const bool x) { return x ? &safe_bool<Target>::safe_bool_true : 0; }
	private:
		safe_bool(); safe_bool(const safe_bool&); void operator=(const safe_bool&);	~safe_bool();
		void	safe_bool_true() const {}
	};
}
