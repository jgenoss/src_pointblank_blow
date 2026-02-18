#if !defined( __I3_CRC32_H)
#define __I3_CRC32_H

I3_EXPORT_BASE	UINT32	CRC32( UINT32 PrevCRC, UINT8 * pBuf, UINT32 Length);
I3_EXPORT_BASE	void	BitRotateEncript( UINT8 * pBuf, UINT32 Length, UINT32 c);
I3_EXPORT_BASE	void	BitRotateDecript( UINT8 * pBuf, UINT32 Length, UINT32 c);

#endif
