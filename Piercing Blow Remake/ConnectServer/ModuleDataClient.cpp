#include "pch.h"
#include "ModuleDataClient.h"
#include "ConnectSessionManager.h"
#include "ConnectSession.h"
#include <cstdio>
#include <cstring>

ModuleDataClient::ModuleDataClient()
	: m_pSessionManager(nullptr)
{
}

ModuleDataClient::~ModuleDataClient()
{
}

// -- ModuleBase overrides --

bool ModuleDataClient::OnConnect()
{
	printf("[ModuleDataClient] Connected to DataServer\n");
	return true;
}

void ModuleDataClient::OnDisconnect()
{
	printf("[ModuleDataClient] Disconnected from DataServer\n");
}

void ModuleDataClient::OnProcessPacket(char* pData, int i32Size)
{
	if (i32Size < 4)
		return;

	uint16_t packetSize = *(uint16_t*)pData & 0x7FFF;	// data field size only
	uint16_t protocolId = *(uint16_t*)(pData + 2);
	char* pPayload = pData + 4;
	int payloadSize = (int)packetSize;

	switch (protocolId)
	{
	case PROTOCOL_IS_AUTH_LOGIN_ACK:
		OnLoginAck(pPayload, payloadSize);
		break;

	default:
		printf("[ModuleDataClient] Unknown protocol: 0x%04X\n", protocolId);
		break;
	}
}

void ModuleDataClient::OnHeartbeat()
{
	// Simple heartbeat - send a small packet to keep connection alive
	i3NetworkPacket packet(0);
	SendPacket(&packet);
}

// -- Request methods --

void ModuleDataClient::RequestLogin(int i32SessionIdx, const char* pszUsername, const char* pszPassword, uint32_t ui32ClientIP)
{
	IS_AUTH_LOGIN_REQ req;
	memset(&req, 0, sizeof(req));
	req.i32SessionIdx = i32SessionIdx;
	strncpy(req.szUsername, pszUsername, sizeof(req.szUsername) - 1);
	strncpy(req.szPassword, pszPassword, sizeof(req.szPassword) - 1);
	req.ui32ClientIP = ui32ClientIP;

	SendRequest(PROTOCOL_IS_AUTH_LOGIN_REQ, &req, sizeof(req));
	printf("[ModuleDataClient] RequestLogin: session=%d user='%s'\n", i32SessionIdx, pszUsername);
}

// -- Response handlers --

void ModuleDataClient::OnLoginAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_AUTH_LOGIN_ACK))
		return;

	IS_AUTH_LOGIN_ACK* pAck = (IS_AUTH_LOGIN_ACK*)pData;

	printf("[ModuleDataClient] LoginAck: session=%d result=%d uid=%lld authLevel=%d\n",
		pAck->i32SessionIdx, pAck->i32Result, (long long)pAck->i64UID, pAck->ui8AuthLevel);

	if (!m_pSessionManager)
		return;

	ConnectSession* pSession = m_pSessionManager->GetSession(pAck->i32SessionIdx);
	if (!pSession)
		return;

	// Forward result to session
	pSession->OnLoginResult(pAck->i32Result, pAck->i64UID, pAck->ui8AuthLevel);
}

// -- Helpers --

void ModuleDataClient::SendRequest(uint16_t ui16Protocol, const void* pStruct, int i32StructSize)
{
	if (!IsConnected())
		return;

	i3NetworkPacket packet(ui16Protocol);
	packet.WriteData(pStruct, i32StructSize);
	SendPacket(&packet);
}
