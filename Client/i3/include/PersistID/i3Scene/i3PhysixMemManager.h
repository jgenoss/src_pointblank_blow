#if !defined( __I3_PHYSIX_MEM_MANAGER_H)
#define __I3_PHYSIX_MEM_MANAGER_H

void		_PHS_Init(void);
void		_PHS_Close(void);
void *		_PHS_Alloc( UINT32 sz);
void *		_PHS_Realloc( void * p, UINT32 sz);
void		_PHS_Free( void * p);

#endif

