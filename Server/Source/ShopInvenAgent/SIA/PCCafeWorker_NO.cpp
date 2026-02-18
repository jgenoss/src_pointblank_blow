#include "pch.h"
#include "PCCafeWorker_NO.h"

CPCCafeWorker_NO::CPCCafeWorker_NO()
{
}

CPCCafeWorker_NO::~CPCCafeWorker_NO()
{
	OnDestroy();
}

BOOL CPCCafeWorker_NO::_WorkProcess( PCCAFE_INPUT* pInput, PCCAFE_OUTPUT* pOutput )
{
	switch( pInput->m_eInputType )
	{
	case TYPE_PCCAFE_INPUT_FIRSTLOGIN		:
		{
			pOutput->m_TUID			= pInput->m_TUID;
			pOutput->m_eOutputType	= TYPE_PCCAFE_OUTPUT_FIRSTLOGIN;
			pOutput->m_ui8PCCafe	= (UINT8)(rand()%3);
		}
		return TRUE;
	}

	return FALSE;
}