#ifndef __CONNECTSERVER_PCH_H__
#define __CONNECTSERVER_PCH_H__

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

// i3 Engine platform defines (required before i3Base.h)
#ifndef _WINDOWS
#define _WINDOWS
#endif
#ifndef I3_COMPILER_VC
#define I3_COMPILER_VC
#endif
#ifndef I3_DLL
#define I3_DLL
#endif

#include "i3Base.h"

// ServerCommon
#include "IServer.h"
#include "ServerInfo.h"
#include "BaseServer.h"
#include "InterServerProtocol.h"

#endif // __CONNECTSERVER_PCH_H__
