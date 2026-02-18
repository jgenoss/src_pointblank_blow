#include "pch.h"
#include "PCCafeWorker_Korea.h"
#include "WorkerStreamBuffer_PCCafe.h"

#define KOREA_LOGIN_BUFFER_IDX -1 // 한국 UID GET 방식이 TYPE_UID_CREATE_EXTER 가 아니면, WORKER_STREAM_BUFFER_PCCAFE 에서 받아와야 합니다.

CPCCafeWorker_Korea::CPCCafeWorker_Korea()
{
}

CPCCafeWorker_Korea::~CPCCafeWorker_Korea()
{
	OnDestroy();
}

BOOL CPCCafeWorker_Korea::_WorkProcess(PCCAFE_INPUT* pInput, PCCAFE_OUTPUT* pOutput)
{
	switch (pInput->m_eInputType)
	{
	case TYPE_PCCAFE_INPUT_FIRSTLOGIN:
		{
			pOutput->m_TUID = pInput->m_TUID;
			pOutput->m_eOutputType = TYPE_PCCAFE_OUTPUT_FIRSTLOGIN;
			pOutput->m_ui8PCCafe = PC_NOT_CAFE;
		}
		return TRUE;
	}

	return FALSE;
}
