#if !defined( __RADIOCHATDEF_H__ )
#define __RADIOCHATDEF_H__


////////////////////////////////////////////////////////////////////////////////////////////////////
// 네트워크에서 idx는 각 그룹에 대해 10을 더한 값이 됩니다.

#define RADIOCHAT_MSG_SIZE	128

typedef struct _tagRadiochat
{
	UINT8	idx;
	char	szMessageUS[ RADIOCHAT_MSG_SIZE ];
	char	szMessageKR[ RADIOCHAT_MSG_SIZE ];
} RADIOCHAT;


extern RADIOCHAT g_RadioChat[3][10];

#endif
