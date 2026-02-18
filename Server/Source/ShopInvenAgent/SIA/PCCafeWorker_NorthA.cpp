#include "pch.h"
#include "PCCafeWorker_NorthA.h"

CPCCafeWorker_NorthA::CPCCafeWorker_NorthA()
{
}

CPCCafeWorker_NorthA::~CPCCafeWorker_NorthA()
{
	OnDestroy();
}

BOOL CPCCafeWorker_NorthA::_WorkProcess( PCCAFE_INPUT* pInput, PCCAFE_OUTPUT* pOutput )
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