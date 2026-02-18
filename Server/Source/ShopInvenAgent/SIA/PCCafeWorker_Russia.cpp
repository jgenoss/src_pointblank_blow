#include "pch.h"
#include "PCCafeWorker_Russia.h"

CPCCafeWorker_Russia::CPCCafeWorker_Russia()
{
}

CPCCafeWorker_Russia::~CPCCafeWorker_Russia()
{
	OnDestroy();
}

BOOL CPCCafeWorker_Russia::_WorkProcess( PCCAFE_INPUT* pInput, PCCAFE_OUTPUT* pOutput )
{
	switch( pInput->m_eInputType )
	{
	case TYPE_PCCAFE_INPUT_FIRSTLOGIN		:
		{
			pOutput->m_TUID			= pInput->m_TUID;
			pOutput->m_eOutputType	= TYPE_PCCAFE_OUTPUT_FIRSTLOGIN;
			pOutput->m_ui8PCCafe	= PC_NOT_CAFE;
		}
		return TRUE;
	}

	return FALSE;
}