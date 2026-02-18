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

	if (argc < 2 )		// 인수가 모자람..
	{
		printf_s("명령행 첫 인수로 조회해야할 로그키 디비 경로가 필요합니다.\n");
		printf_s("명령행 둘째 인수로 저장해야할 공개키 배포 파일의 경로가 필요합니다.\n");
		printf_s("[엔터키를 눌러서 종료합니다]\n");
		scanf_s("%c", &input_char);
		return 0;
	}
	
	const char* db_path = argv[0];
	const char* save_path = argv[1];


	printf_s("%s 경로에서 로그키 디비를 참고하여\n", db_path);
	printf_s("%s 경로로 공개키 배포파일을 생성합니다.\n", save_path);
	printf_s("공개키 배포 파일을 생성하려면 Y/y키와 엔터키를 누르세요.\n");

	scanf_s("%c", &input_char);
	char k = input_char;

	if ( k != 0x0d && k!= 0x0a)
		scanf_s("%c", &input_char);
		
	if ( k == 'Y' || k == 'y')
	{
		bool res = i3::save_public_key_from_db(db_path, save_path);

		if (res)
		{
			printf_s("배포 파일을 생성하였습니다.\n");
			scanf_s("%c", &input_char);
		}
		else
		{
			printf_s("배포 파일 생성에 실패했습니다.\n");
			scanf_s("%c", &input_char);
		}
	}
	
	return 0;
}

