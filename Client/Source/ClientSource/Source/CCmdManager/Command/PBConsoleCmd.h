#pragma once

#include "i3Framework/i3UI/i3UIConsoleCmd.h"

enum CMD_RESULT_FLAG
{
	CMD_RESULT_FAIL = 0,
	CMD_RESULT_OK,
	CMD_RESULT_LOOP
};

class PBConsoleCmd
{
public:
	UINT32 m_LoopCount = 0;
	string m_LogContext;
public:
	PBConsoleCmd(void) {}
	~PBConsoleCmd(void) {}
public:
	
	/* SAMPLE - .\\OutGameCmd\\PBCMD_UseItem
	--
		처음 명령어를 받으면 ExecuteCommand() 에서 먼저 처리를 수행하게 되는데 여기서 필요한 인자들을
		얻어와서 각객체내의 멤버 변수들에 저장합니다.
	--
	*/
	virtual CMD_RESULT_FLAG ExecuteCommand( i3::vector<i3::string> &params) { return CMD_RESULT_FAIL; }

	/*
	--
		RunCommand() 가 수행 되는경우 따로 커맨드입력시 반복횟수를 인자로 주지않으면 기본 LoopCount가 1로 설정됩니다
		return CMD_RESULT_OK; 를 반환 하게 되면 PBConsoleManager 객체에서 LoopCount 를 자동으로 1씩 소모시켜주며
		LoopCount 가 0 이 되면 현재 명령어가 PBConsoleManager 객체내의 명령수행리스트 에서 제거됩니다
		return CMD_RESULT_LOOP; 를 반환 하게되면 LoopCount 를 자동으로 소모시켜주지 않으니 상황에따라 처리하시면 됩니다.
	--
	*/
	virtual CMD_RESULT_FLAG RunCommand(REAL32 DeltaSec) = 0;

public:
	void setLoopCount(UINT32 Count);
	UINT32 getLoopCount();

	void setLogContext(string str);
	void addLogContext(string str);
	string getLogContext( );
	void clearLogContext( );
	bool SaveLogFile( char Path[] , bool AddWrite = false );

	void PrintLog(string str) { }
	void PrintLog(wstring wstr) { }

};
