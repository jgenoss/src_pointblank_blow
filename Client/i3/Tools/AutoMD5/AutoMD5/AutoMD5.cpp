#include "AutoMD5.h"
#include "md5.h"

int main(int argc, char *argv[])
{
	if( argv[1] == NULL )
	{
		std::cout<<"작업할 파일명은 첫번째 매계인수로 넣어주세요. \n프로그램 종료."<<std::endl;
		return NULL;
	}
	
	//====================================//
	//엠디 5 값 추출 하고 해당 값 출력( AutoMD5.bat 에서 해당 값 불러옵니다. )

	MD5 md5;
	char* md5result = NULL;

	md5result = md5.digestFile(argv[1]);
	std::strupr(md5result);
	std::cout<<md5result<<std::endl;
	
	//====================================//

	return 0;
}