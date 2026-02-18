#pragma once

//
// 이건 그냥 함수로 만든다... (char문자열에 대해서만 처리되고, 유니코드는 해당사항이 없음)
// 스트링클래스 지원이 필요하다면, 모두 래핑 처리해둔다..
//

namespace i3
{
	// 인수 2개짜리 함수를 먼저 만들고..
	I3_EXPORT_BASE bool is_utf8( const char* str, std::size_t len);
		
	
	// 나머지는 템플릿/오버로딩으로 래핑..

	
	
}