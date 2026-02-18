#include "pch.h"
#include "TaskProcessor.h"
#include "UserFinder.h"

BOOL CTaskProcessor::_UpdateKickList()
{
	INT32				i32Count;

	i32Count	= m_pDBKickList->GetBufferCount();

	if( i32Count <= 0)					return FALSE;
	if( i32Count > TASK_USER_KICK_MAX )		i32Count = TASK_USER_KICK_MAX;

	DB_USER_KICK_IN*	pDBUserKickIn;

	for(INT32 i = 0; i < i32Count; i++)
		{
			DB_USER_KICK_OUT	pDBUserKickOut;

			pDBUserKickIn					= m_pDBKickList->Pop();

			if( pDBUserKickIn != NULL)
			{
				if( TRUE == _AccountKick( pDBUserKickIn->i64UID, ACCOUNT_KICK_GM_TOOL ) )
				{
					pDBUserKickOut.i32Rv			= EVENT_ERROR_SUCCESS;
				}
				else
				{
					pDBUserKickOut.i32Rv			= EVENT_ERROR_FAIL;
				}

				pDBUserKickOut.i32UserKickIdx	= pDBUserKickIn->i32UserKickIdx;
				pDBUserKickOut.i64UID			= pDBUserKickIn->i64UID;

				m_pDBKickList->Push(&pDBUserKickOut);
			}

			m_pDBKickList->PopIdx();
		}

	return TRUE;
}