#ifndef __GAMESERVER_PCH_H__
#define __GAMESERVER_PCH_H__

#pragma once

// Windows
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

// Standard C++
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cmath>

// i3 Engine
#include "i3Base.h"

// ServerCommon
#include "IServer.h"
#include "ServerInfo.h"
#include "BaseServer.h"
#include "InterServerProtocol.h"

#endif // __GAMESERVER_PCH_H__
