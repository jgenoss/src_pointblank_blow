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


int _tmain(int argc, _TCHAR* argv[])
{
	::SetConsoleCtrlHandler(ConsoleHandler, TRUE);

	--argc;
	++argv;

	TCHAR  input_char;

	if (argc < 3 )		// 인수가 모자람..
	{
		printf_s("명령행 첫째 인수는 로그 파일 DB 경로가 와야 합니다.\n");
		printf_s("명령행 둘째 인수로 암호화된 로그 파일 경로가 필요합니다.\n");
		printf_s("명령행 셋째 인수로 해독되어 저장될 파일 경로가 필요합니다\n");
		printf_s("[엔터키를 눌러서 종료합니다]\n");
		scanf_s("%c", &input_char);
		return 0;
	}
	
	const char* db_path  = argv[0];
	const char* src_path = argv[1];
	const char* dest_path = argv[2];
	
	printf_s("%s 경로의 로그 디비 파일을 참조하여\n", db_path);
	printf_s("암호화된 %s 경로의 로그 파일을\n", src_path);
	printf_s("%s 경로로 해독합니다.\n", dest_path);

	bool res = i3::decrypt_log_enc(db_path, src_path, dest_path);

	if (res)
	{
		printf_s("파일 해독에 성공하였습니다.\n");
		scanf_s("%c", &input_char);
	}
	else
	{
		printf_s("파일 해독에 실패했습니다.\n");
		scanf_s("%c", &input_char);
	}
		
	return 0;
}

