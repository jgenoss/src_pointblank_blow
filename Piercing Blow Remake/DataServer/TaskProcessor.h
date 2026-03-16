#ifndef __TASKPROCESSOR_H__
#define __TASKPROCESSOR_H__

#pragma once

class DataServerContext;
class DataSession;
struct IS_AUTH_LOGIN_REQ;
struct IS_PLAYER_LOAD_REQ;
struct IS_PLAYER_SAVE_REQ;
struct IS_PLAYER_CREATE_NICK_REQ;
struct IS_PLAYER_CHECK_NICK_REQ;
struct IS_STATS_SAVE_REQ;

// Dispatcher central del DataServer
// Recibe requests desde DataSession y los despacha a los modulos de DB
// Patron portado de CTaskProcessor (Server/Source/Trans/Trans/TaskProcessor.h)
// Version simplificada: sincronico (sin i3Thread), ejecuta queries directamente
class TaskProcessor
{
public:
	TaskProcessor(DataServerContext* pContext);
	~TaskProcessor();

	// Dispatch methods - llamados desde DataSession::PacketParsing
	void ProcessAuthLogin(DataSession* pSession, IS_AUTH_LOGIN_REQ* pReq);
	void ProcessPlayerLoad(DataSession* pSession, IS_PLAYER_LOAD_REQ* pReq);
	void ProcessPlayerSave(DataSession* pSession, IS_PLAYER_SAVE_REQ* pReq);
	void ProcessPlayerCreateNick(DataSession* pSession, IS_PLAYER_CREATE_NICK_REQ* pReq);
	void ProcessPlayerCheckNick(DataSession* pSession, IS_PLAYER_CHECK_NICK_REQ* pReq);
	void ProcessStatsSave(DataSession* pSession, IS_STATS_SAVE_REQ* pReq);

private:
	DataServerContext*	m_pContext;
};

#endif // __TASKPROCESSOR_H__
