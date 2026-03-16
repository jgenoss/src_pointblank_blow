#ifndef __S2MO_STRUCT_H__
#define __S2MO_STRUCT_H__

typedef		INT16		T_PACKET_SIZE;
typedef		INT16		T_PACKET_PROTOCOL;
typedef		INT32		T_RESULT;

#define S2MO_PACKET_CONTENT_SIZE			8192
#define S2MO_IS_SUCCESS(result)				((0<=result)?TRUE:FALSE)

#endif