#include "TaskProcessor.h"
#include "DataServerContext.h"
#include "DataSession.h"
#include "ModuleDBAuth.h"
#include "ModuleDBUserLoad.h"
#include "ModuleDBUserSave.h"
#include "InterServerProtocol.h"
#include "i3NetworkPacket.h"
#include <cstdio>
#include <cstring>

TaskProcessor::TaskProcessor(DataServerContext* pContext)
	: m_pContext(pContext)
{
}

TaskProcessor::~TaskProcessor()
{
}

void TaskProcessor::ProcessAuthLogin(DataSession* pSession, IS_AUTH_LOGIN_REQ* pReq)
{
	if (!m_pContext->GetModuleDBAuth())
		return;

	IS_AUTH_LOGIN_ACK ack;
	memset(&ack, 0, sizeof(ack));
	ack.i32SessionIdx = pReq->i32SessionIdx;

	bool bResult = m_pContext->GetModuleDBAuth()->AuthenticateUser(
		pReq->szUsername, pReq->szPassword, &ack);

	printf("[TaskProcessor] AUTH_LOGIN: user='%s' result=%d uid=%lld\n",
		pReq->szUsername, ack.i32Result, ack.i64UID);

	// Enviar respuesta
	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_AUTH_LOGIN_ACK);
	packet.WriteData(&ack, sizeof(ack));
	pSession->SendMessage(&packet);
}

void TaskProcessor::ProcessPlayerLoad(DataSession* pSession, IS_PLAYER_LOAD_REQ* pReq)
{
	if (!m_pContext->GetModuleDBUserLoad())
		return;

	IS_PLAYER_LOAD_ACK ack;
	memset(&ack, 0, sizeof(ack));
	ack.i64UID = pReq->i64UID;
	ack.i32SessionIdx = pReq->i32SessionIdx;

	// Buffer para payload de datos del jugador
	char szPayload[8192];
	int i32PayloadSize = 0;

	bool bResult = m_pContext->GetModuleDBUserLoad()->LoadPlayerData(
		pReq->i64UID, &ack, szPayload, &i32PayloadSize);

	printf("[TaskProcessor] PLAYER_LOAD: uid=%lld result=%d payloadSize=%d\n",
		pReq->i64UID, ack.i32Result, i32PayloadSize);

	// Enviar respuesta con header + payload
	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_PLAYER_LOAD_ACK);
	packet.WriteData(&ack, sizeof(ack));
	if (bResult && i32PayloadSize > 0)
		packet.WriteData(szPayload, i32PayloadSize);
	pSession->SendMessage(&packet);
}

void TaskProcessor::ProcessPlayerSave(DataSession* pSession, IS_PLAYER_SAVE_REQ* pReq)
{
	if (!m_pContext->GetModuleDBUserSave())
		return;

	bool bResult = m_pContext->GetModuleDBUserSave()->SavePlayerData(
		pReq->i64UID, pReq->i32Level, pReq->i64Exp, pReq->i32Cash, pReq->i32GP);

	printf("[TaskProcessor] PLAYER_SAVE: uid=%lld result=%d\n",
		pReq->i64UID, bResult ? 0 : 1);

	// Fire-and-forget en v1, pero enviamos ACK para confirmacion
	IS_PLAYER_SAVE_ACK ack;
	memset(&ack, 0, sizeof(ack));
	ack.i64UID = pReq->i64UID;
	ack.i32Result = bResult ? 0 : 1;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_PLAYER_SAVE_ACK);
	packet.WriteData(&ack, sizeof(ack));
	pSession->SendMessage(&packet);
}

void TaskProcessor::ProcessPlayerCreateNick(DataSession* pSession, IS_PLAYER_CREATE_NICK_REQ* pReq)
{
	if (!m_pContext->GetModuleDBAuth())
		return;

	int i32Result = m_pContext->GetModuleDBAuth()->CreateNickname(
		pReq->i64UID, pReq->szNickname);

	printf("[TaskProcessor] CREATE_NICK: uid=%lld nick='%s' result=%d\n",
		pReq->i64UID, pReq->szNickname, i32Result);

	IS_PLAYER_CREATE_NICK_ACK ack;
	memset(&ack, 0, sizeof(ack));
	ack.i64UID = pReq->i64UID;
	ack.i32SessionIdx = pReq->i32SessionIdx;
	ack.i32Result = i32Result;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_PLAYER_CREATE_NICK_ACK);
	packet.WriteData(&ack, sizeof(ack));
	pSession->SendMessage(&packet);
}

void TaskProcessor::ProcessPlayerCheckNick(DataSession* pSession, IS_PLAYER_CHECK_NICK_REQ* pReq)
{
	if (!m_pContext->GetModuleDBAuth())
		return;

	bool bExists = m_pContext->GetModuleDBAuth()->CheckNicknameExists(pReq->szNickname);

	printf("[TaskProcessor] CHECK_NICK: nick='%s' exists=%d\n",
		pReq->szNickname, bExists);

	IS_PLAYER_CHECK_NICK_ACK ack;
	memset(&ack, 0, sizeof(ack));
	ack.i32SessionIdx = pReq->i32SessionIdx;
	ack.i32Result = bExists ? 1 : 0;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_PLAYER_CHECK_NICK_ACK);
	packet.WriteData(&ack, sizeof(ack));
	pSession->SendMessage(&packet);
}

void TaskProcessor::ProcessStatsSave(DataSession* pSession, IS_STATS_SAVE_REQ* pReq)
{
	if (!m_pContext->GetModuleDBUserSave())
		return;

	bool bResult = m_pContext->GetModuleDBUserSave()->SaveBattleStats(
		pReq->i64UID, pReq->i32Kills, pReq->i32Deaths,
		pReq->i32Headshots, pReq->i32Wins, pReq->i32Losses);

	printf("[TaskProcessor] STATS_SAVE: uid=%lld result=%d\n",
		pReq->i64UID, bResult ? 0 : 1);

	IS_STATS_SAVE_ACK ack;
	memset(&ack, 0, sizeof(ack));
	ack.i64UID = pReq->i64UID;
	ack.i32Result = bResult ? 0 : 1;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_STATS_SAVE_ACK);
	packet.WriteData(&ack, sizeof(ack));
	pSession->SendMessage(&packet);
}
