#ifndef __DATASERVER_PCH_H__
#define __DATASERVER_PCH_H__

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

// i3 Engine
#include "i3Base.h"

// ServerCommon
#include "IServer.h"
#include "ServerInfo.h"
#include "BaseServer.h"
#include "InterServerProtocol.h"

#endif // __DATASERVER_PCH_H__
