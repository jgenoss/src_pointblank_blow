#ifndef		__USING_BOOST_H__
#define		__USING_BOOST_H__

// ver boost_1_45_0
// boost library (추가로 사용할 boost 라이브러리가 있을 경우 헤더만를 include하면 해당 boost 함수를 사용할 수 있다. (komet)
//#include "boost/format.hpp"		// format



//-------------------------------------------------------------------------------
// boost::format()
//
// web: http://www.boost.org/doc/libs/1_45_0/libs/format/index.html
// syntax: http://www.boost.org/doc/libs/1_45_0/libs/format/doc/format.html
//-------------------------------------------------------------------------------
// boost 라이브러리의 format 함수는 type safe string format 이며 순서에 관게없이 인자를 넣어도 안전하다. 
// sprintf 계열은 인자의 타입이 다르면 문제가 되며 예를 들어 "%s" 스트링에 <- int형 인자 값을 넣으면 API 내부에서 크래시하게된다.
// 각나라별 스트링을 대입할때 각국 언어의 어순에 따라 번역의 순서가 반대로 오는 경우가 생길 경우 심각한 크래시를 유발하여 이 문제를 해결하기 위해 boost 라이브러리를 사용한다.
// 함수를 사용하려면 #include "boost.h" 를 선언하세요.
//
// 스트링 포멧 순서에 따라 인자 값이 차례로 대입되며 인자 타입은 자유롭다.
// 기존 스트링의 %d %s 도 사용가능하나 안전하지 못해서 인자가 틀리면 크래시하므로 주의해서 사용할 것.
//
//	형식:
//
//	%번호%		: 양쪽으로 %오퍼레이터를 사용하며 스트링 포멧에서 인자의 순서를 정한다.
//	%번호$03d	: %03d랑 같으며 000 형식으로 수치 값을 나타낸다. 문자형이 들어가도 0으로 나머지 자리는 채워지므로 안전하다.
//
//  사용:	아래 사용시 반환값은 std::string 이므로 char * 로 사용하려면 한번 변환이 필요하다.
//
//	예) std::string str = BOOST_FORMAT3( "%3% %2% %1%", "1번", "2번", "3번");		//	std::string으로 받아온다.
//	    const char * temp = str.c_str();		// std::string에서 변환
//
//	결과)	s == "3번 2번 1번";
//
//#define BOOST_FORMAT1( fmt, arg1)						((boost::format(fmt) % (arg1)).str())
//#define BOOST_FORMAT2( fmt, arg1, arg2)					((boost::format(fmt) % (arg1) % (arg2)).str())
//#define BOOST_FORMAT3( fmt, arg1, arg2, arg3)			((boost::format(fmt) % (arg1) % (arg2) % (arg3)).str())
//#define BOOST_FORMAT4( fmt, arg1, arg2, arg3, arg4)		((boost::format(fmt) % (arg1) % (arg2) % (arg3) % (arg4)).str())


#endif // __USING_BOOST_H__