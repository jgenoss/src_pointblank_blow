#ifndef __I3_IMAGE_FILE_I3I_H
#define __I3_IMAGE_FILE_I3I_H

#define I3I_SIGNATURE_SIZE			4
#define I3I_SIGNATURE_BINARY		"I3IB"
#define I3I_SIGNATURE_EXTERNAL		"I3IE"

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

typedef struct PACKED _tagI3I_HEADER
{
	INT8				m_Signature[I3I_SIGNATURE_SIZE];
	UINT8				m_Version[2];
	UINT16				m_Width;
	UINT16				m_Height;
	I3G_IMAGE_FORMAT	m_Format;

	UINT16				m_ClutCount;
} I3I_FILE_HEADER;

typedef struct PACKED _tagI3I_HEADER2
{
	I3I_FILE_HEADER		m_OldVersionHeader;

	UINT32				m_Flags;
	UINT32				m_FileSize;
	UINT16				m_MipMapCount;
	UINT16				m_NameLength;
} I3I_FILE_HEADER2;

typedef struct PACKED _tagI3I_HEADER3
{
	I3I_FILE_HEADER		m_OldVersionHeader;

	UINT32				m_TexFlag;						// Texture Usage Flag°¡ »ç¿ëµÊ.
	UINT32				m_FileSize;
	UINT16				m_MipMapCount;
	UINT16				m_NameLength;
	UINT32				m_DataSize[8];
} I3I_FILE_HEADER3;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

#endif // __I3_IMAGE_FILE_I3I_H
