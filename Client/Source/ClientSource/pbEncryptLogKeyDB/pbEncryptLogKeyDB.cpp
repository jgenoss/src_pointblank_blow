// i3EncryptLogKeyDB.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "i3Base/i3RSABF.h"

//
//  콘솔창에서 응용프로그램을 버튼으로 종료시에는 전역변수 내부소멸자와 doexit함수호출 순서가 바뀐다..
//  이로 인해서, 라이브러리에서 정해진 소거규칙이 깨지게 되며, 또한 메인함수의 리턴도 실행되지 않기 때문에
//  말그대로 비정상 종료로 간주해야될것으로 보인다..
//

BOOL WINAPI ConsoleHandler(DWORD ctrl_type) { return TRUE;  }		// 크래시를 막기 위한 장치..


const size_t default_key_length = 128;
const size_t min_key_length = 1;
const size_t max_key_length = 512;

int _tmain(int argc, _TCHAR* argv[])
{
	::SetConsoleCtrlHandler(ConsoleHandler, TRUE);

	--argc;
	++argv;

	TCHAR  input_char;

	if (argc == 0)		// 인수 없음..
	{
		printf_s("명령행 첫 인수로 로그키 디비가 저장될 파일경로가 필요합니다.\n");
		printf_s("[엔터키를 눌러서 종료합니다]\n");
		scanf_s("%c", &input_char);
		return 0;
	}

	const TCHAR* tstr = argv[0];
	printf_s("RSA 공개/비공개키 db파일을 \n");
	printf_s("%s 경로로 둡니다.\n", tstr);
	printf_s("명령행 둘째 인수는 옵션으로 키의 길이를 저장할 수 있습니다.\n");
	printf_s("Default = %d, 범위 = %d ~ %d \n", default_key_length, min_key_length, max_key_length);
	printf_s("db파일에 공개/비공개키를 추가하고 싶으면 Y/y키와 엔터키를 누르세요.\n");

	size_t key_length = default_key_length;		// 디폴트..

	if ( argc > 1)
	{
		int num = atoi(argv[1]);
		if (num < min_key_length) num = min_key_length;
		else if (num > max_key_length) num = max_key_length;
		key_length = num;
	}

	scanf_s("%c", &input_char);
	char k = input_char;

	if ( k != 0x0d && k!= 0x0a)
		scanf_s("%c", &input_char);
		
	if ( k == 'Y' || k == 'y')
	{
		i3::save_public_private_key_to_db(tstr, key_length);			// 128이란 값은 언제든 변경 가능..
	}
	
	return 0;
}

