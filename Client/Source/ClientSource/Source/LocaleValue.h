#ifndef _LocaleValue_H__
#define _LocaleValue_H__

/*	국가 설정 값 얻는 곳
설명	:	국가에 따라 설정 된 값이 다르기 때문에 Pef 에 저장된 속성 값을
			스트링 인덱스로 검색해 값을 얻어 온다.
			스트링 인덱스 구성 방식은 부모 스트링/자식 스트링 으로 구성된다. (구분자 '/')
*/
namespace LocaleValue
{
	/*	국가 설정 값을 초기화 한다.
	return	:	초기화 성공 true, 실패시 false를 반환한다.
	*/
	bool Init();

	/*	국가 설정 값을 해제한다.
	*/
	void Release();

	/*	국가별 속성값을 읽어 온다.
	메모	:	해당 스트링 인덱스에 속성 값이 없는 경우, 기본값(false 반환)
	ValueID	:	읽어 올 스트링 인덱스
	return	:	bool 형식으로 true, false 를 반환한다. 
	*/
	bool Enable(const i3::string & ValueID);

	/*	국가별 속성값을 읽어 온다.
	메모	:	해당 스트링 인덱스에 속성 값이 없는 경우, 기본값(false 반환)
	ValueID	:	읽어 올 스트링 인덱스
	return	:	bool 형식으로 true, false 를 반환한다.
	*/
	bool GetBOOL(const i3::string & ValueID);

	/*	국가별 속성값을 읽어 온다.
	메모	:	해당 스트링 인덱스에 속성 값이 없는 경우, 기본값( 0 반환)
	ValueID	:	읽어 올 스트링 인덱스
	return	:	INT32 형식으로 반환한다.
	*/
	INT32 GetInt32(const i3::string & ValueID);

	/*	국가별 속성값을 읽어 온다.
	메모	:	해당 스트링 인덱스에 속성 값이 없는 경우, 기본값(빈 스트링 반환)
	ValueID	:	읽어 올 스트링 인덱스
	return	:	std::string 형식으로 반환한다.
	*/
	i3::rc_wstring GetString(const i3::string & ValueID);

	/*	국가별 속성값을 읽어 온다.
	메모	:	해당 스트링 인덱스에 속성 값이 없는 경우, 기본값(0.0f 반환)
	ValueID	:	읽어 올 스트링 인덱스
	return	:	REAL32 형식으로 반환한다
	*/
	REAL32 GetReal(const i3::string & ValueID);

	/*	국가별 속성값을 읽어 온다.
	메모	:	해당 스트링 인덱스에 속성 값이 없는 경우, 기본값(x = 0.0f, y = 0.0f 반환)
	ValueID	:	읽어 올 스트링 인덱스
	return	:	VEC2D 형식으로 반환한다
	*/
	VEC2D GetVec2d(const i3::string & ValueID);

	/*	국가별 속성값을 읽어 온다.
	메모	:	해당 스트링 인덱스에 속성 값이 없는 경우, 기본값(x = 0.0f, y = 0.0f, z = 0.0f 반환)
	ValueID	:	읽어 올 스트링 인덱스
	return	:	VEC3D 형식으로 반환한다
	*/
	VEC3D GetVec3d(const i3::string & ValueID);

	/*	국가별 속성값을 읽어 온다.
	메모	:	해당 스트링 인덱스에 속성 값이 없는 경우, 기본값(x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f 반환)
	ValueID	:	읽어 올 스트링 인덱스
	return	:	VEC4D 형식으로 반환한다
	*/
	VEC4D GetVec4d(const i3::string & ValueID);

	/*	국가별 속성값을 읽어 온다.
	메모	:	해당 스트링 인덱스에 속성 값이 없는 경우, 기본값(r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f 반환)
	ValueID	:	읽어 올 스트링 인덱스
	return	:	I3COLOR 형식으로 반환한다
	*/
	I3COLOR GetColor(const i3::string & ValueID);

	/*	국가별 속성값을 읽어 온다.
	메모	:	해당 스트링 인덱스에 속성 값이 없는 경우, 기본값(모두 반환)
	ValueID	:	읽어 올 스트링 인덱스
	return	:	MATRIX 형식으로 반환한다
	*/
	MATRIX GetMatrix(const i3::string & ValueID);
};

//#if defined( 
#define __L_DEFINED__( arg )	if( LocaleValue::Enable( arg ) )\
{
#define __L_ELSE_IF__ }\
else if( LocaleValue::Enable( arg ) )\
{
#define __L_ELSE__ }\
else\
{
#define __L_END__	}

#endif //_CONFIG_H__

