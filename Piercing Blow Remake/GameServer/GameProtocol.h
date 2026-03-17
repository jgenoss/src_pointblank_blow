#ifndef __GAMEPROTOCOL_H__
#define __GAMEPROTOCOL_H__

#pragma once

// Enable cross-server find protocols (originally in Trans server, handled locally in Remake)
#define USE_TRANS_FIND

// Use the original ProtocolDef.h from S2MO/Common
// Available via include path: Common/Library/include/ProtocolDef.h
#include "ProtocolDef.h"

// Packet header size: 2 bytes size + 2 bytes protocol ID
#define SOCKET_HEAD_SIZE	4

#endif // __GAMEPROTOCOL_H__
