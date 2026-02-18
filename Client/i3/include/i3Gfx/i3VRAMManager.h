#ifndef __I3_VRAMMANAGER_H
#define __I3_VRAMMANAGER_H

#ifdef I3_PSP

#include "i3Base.h"

//DrawBffer 
#define I3VRAM_DRAW_16BIT		1
#define I3VRAM_DRAW_32BIT		2

//Debug 
#define I3VRAM_DEBUG_FILE			0X00000001
#define I3VRAM_DEBUG_TEXT			0X00000002
#define I3VRAM_DEBUG_SUM			0X00000004
#define I3VRAM_DEBUG_EACH			0X00000008

namespace i3VRAMManager
{
	INT32	Create(INT32 BlockCount = 300, INT32 DrawBuffer = I3VRAM_DRAW_16BIT); 
	void	Destroy(void);

	INT32	Alloc(UINT32 nBytes, INT32 align = I3MEMORY_MINIMUM_ALLOCSIZE);	
	void	Free(INT32 idx); 

	void *	Lock(INT32 idx);
	void *	UnLock(INT32 idx);	
	
	void	Dump(INT32 Type = I3VRAM_DEBUG_TEXT | I3VRAM_DEBUG_SUM); 

	void	Reset(void);
	INT32	FindEmptyBlock(void);
}
#endif

#endif
