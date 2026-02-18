#include "pch.h"
#include "PCCafeWorker_Italy.h"

#include "CTPacket.h"

CPCCafeWorker_Italy::CPCCafeWorker_Italy()
{
}

CPCCafeWorker_Italy::~CPCCafeWorker_Italy()
{
	OnDestroy();
}

BOOL CPCCafeWorker_Italy::_WorkProcess( PCCAFE_INPUT* pInput, PCCAFE_OUTPUT* pOutput )
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

	case TYPE_PCCAFE_INPUT_LOGOUT:
		{
			pOutput->m_eOutputType = TYPE_PCCAFE_OUTPUT_LOGOUT;

			char strID[21];
			i3mem::FillZero(strID, 21);
			strncpy(strID, pInput->m_strID, 20);

			CTPacket sendPacket(1003);
			sendPacket << strID << "PBLANK";

			sendPacket.Encrypt(12280411);
			//g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L" latin log out");

			if (0 == SendMessage( (char *)sendPacket.GetBuffer(), sendPacket.GetSize() ) )  return FALSE;

			//g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L" latin log out2");
		}
		break;
	}

	return FALSE;
}