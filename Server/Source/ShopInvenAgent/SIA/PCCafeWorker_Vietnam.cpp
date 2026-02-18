#include "pch.h"
#include "PCCafeWorker_Vietnam.h"

CPCCafeWorker_Vietnam::CPCCafeWorker_Vietnam()
{
}

CPCCafeWorker_Vietnam::~CPCCafeWorker_Vietnam()
{
	OnDestroy();
}

BOOL CPCCafeWorker_Vietnam::_WorkProcess( PCCAFE_INPUT* pInput, PCCAFE_OUTPUT* pOutput )
{
	switch( pInput->m_eInputType )
	{
	case TYPE_PCCAFE_INPUT_FIRSTLOGIN		:
		{
			pOutput->m_TUID			= pInput->m_TUID;
			pOutput->m_eOutputType	= TYPE_PCCAFE_OUTPUT_FIRSTLOGIN;

			// GCA üũ
			if( pInput->m_ui16GcaType & GCA_MASK_VIETNAM )
			{
				pOutput->m_ui8PCCafe = PC_GARENA_GCA;
			}
			else
			{
				pOutput->m_ui8PCCafe = PC_NOT_CAFE;
			}
		}
		return TRUE;
	}

	return FALSE;
}