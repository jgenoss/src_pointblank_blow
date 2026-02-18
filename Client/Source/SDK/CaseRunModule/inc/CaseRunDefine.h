#pragma once

#include <string>

namespace CaseRun
{
	// 이 Enum 값은 CaseRun SDK에서 정의하고 있는 값과 동일해야 한다.
	enum TESTRESULT
	{
		TESTRESULT_NA,					// Test되지 않았음.
		TESTRESULT_PASS,				// Case 실행이 정상적으로 종료되었으며, 성공
		TESTRESULT_FAIL,				// Case 실행 결과, 실패 했음.
		TESTRESULT_STOP,				// 중지되었음.
		TESTRESULT_ERROR,				// Case 실행 조건이 맞지 않아 실행하지 못했음.
		TESTRESULT_BLOCKED,
		TESTRESULT_NO_AGENT,			// Agent 수가 부족하여 Test를 수행하지 못함.
		TESTRESULT_CRASH,

		TESTRESULT_STARTED = 100,
		TESTRESULT_FINISHED,

		TESTRESULT_CONTINUE,			// 내부적인 용도로 사용되며, DoTest() 함수가 아직 완전히 처리되지 않았음을 의미.
		TESTRESULT_CONTINUE_YEILD,		// DoTest()가 아직 완전히 처리되지 않았지만, 다음 Node를 실행할 수 있다는 의미.
	};

	enum BUTTON
	{
		BTN_L,
		BTN_M,
		BTN_R
	};

	enum VAROP
	{
		VAR_OP_PLUS,
		VAR_OP_MINUS,
		VAR_OP_MUL,
		VAR_OP_DIV
	};

	enum VARCOMPARE
	{
		VAR_COMPARE_EQUAL,
		VAR_COMPARE_NOT_EQUAL,
		VAR_COMPARE_GREATER,
		VAR_COMPARE_GEQUAL,
		VAR_COMPARE_LESS,
		VAR_COMPARE_LEQUAL
	};

	enum CTRL_STATE
	{
		CTRL_STATE_NORMAL,
		CTRL_STATE_DISABLED,
		CTRL_STATE_SELECTED,
		CTRL_STATE_FOCUSED
	};

	enum LOCTYPE
	{
		LOC_POSITION,
		LOC_CHARA,
		LOC_OBJECT
	};

	enum DIRTYPE
	{
		DIR_DIRECTION,
		DIR_CHARA,
		DIR_OBJECT
	};

	class NodeBase;

	struct TestInfo
	{
		std::wstring	m_CaseID;
		INT32			m_idxNode;
		std::wstring	m_Verb;
		INT32			m_PID;

		NodeBase *		m_pNode;
	};

	typedef void		JSONNODE;

	typedef void(*PACKETCALLBACK)(const wchar_t * packet, void * pUserData);
}
