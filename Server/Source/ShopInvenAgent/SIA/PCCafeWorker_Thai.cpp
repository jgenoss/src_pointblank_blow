#include "pch.h"
#include "PCCafeWorker_Thai.h"

CPCCafeWorker_Thai::CPCCafeWorker_Thai()
{
}

CPCCafeWorker_Thai::~CPCCafeWorker_Thai()
{
	OnDestroy();
}

BOOL CPCCafeWorker_Thai::_WorkProcess( PCCAFE_INPUT* pInput, PCCAFE_OUTPUT* pOutput )
{
	switch( pInput->m_eInputType )
	{
	case TYPE_PCCAFE_INPUT_FIRSTLOGIN		:
		{
			pOutput->m_TUID			= pInput->m_TUID;
			pOutput->m_eOutputType	= TYPE_PCCAFE_OUTPUT_FIRSTLOGIN;

			// GCA PLUS üũ
			if ( pInput->m_ui16GcaType & GCAPLUS_MASK_THAI )
			{
				pOutput->m_ui8PCCafe = PC_GARENA_GCA_PLUS;
			}
			// GCA üũ
			else if ( pInput->m_ui16GcaType & GCA_MASK_THAI )
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