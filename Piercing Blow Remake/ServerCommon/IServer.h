#ifndef __ISERVER_H__
#define __ISERVER_H__

#pragma once

enum class ServerType
{
	Connect = 0,
	Game,
	Battle,
	Data,

	Count
};

enum class ServerState
{
	None = 0,
	Loading,
	Init,
	Initing,
	Start,
};

class IServer
{
public:
	virtual ~IServer() = default;

	virtual bool		Initialize(const char* pszConfigPath) = 0;
	virtual bool		Start() = 0;
	virtual void		Update() = 0;
	virtual void		Shutdown(int i32Reason) = 0;
	virtual ServerType	GetType() const = 0;
	virtual const char*	GetName() const = 0;
};

#endif // __ISERVER_H__
