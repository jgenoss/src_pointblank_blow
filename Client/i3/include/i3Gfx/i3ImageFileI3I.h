#ifndef __I3_IMAGE_FILE_I3I_H
#define __I3_IMAGE_FILE_I3I_H

#define I3I_SIGNATURE_SIZE			4
#define I3I_SIGNATURE_BINARY		"I3IB"
#define I3I_SIGNATURE_EXTERNAL		"I3IE"

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace i3
{
	namespace pack
	{
		struct PACKED IMAGE_FILE_HEADER
		{
			INT8				m_Signature[I3I_SIGNATURE_SIZE] = { 0, 0, 0, 0 };
			UINT8				m_Version[2] = { 0, 0 };
			UINT16				m_Width = 0;
			UINT16				m_Height = 0;
			I3G_IMAGE_FORMAT	m_Format = I3G_IMAGE_FORMAT::I3G_IMAGE_FORMAT_NONE;

			UINT16				m_ClutCount = 0;
		};

		struct PACKED IMAGE_FILE_HEADER2
		{
			IMAGE_FILE_HEADER		m_OldVersionHeader;

			UINT32				m_Flags = 0;
			UINT32				m_FileSize = 0;
			UINT16				m_MipMapCount = 0;
			UINT16				m_NameLength = 0;
		};

		struct PACKED IMAGE_FILE_HEADER3
		{
			IMAGE_FILE_HEADER		m_OldVersionHeader;

			UINT32				m_TexFlag = 0;						// Texture Usage Flag°¡ »ç¿ëµÊ.
			UINT32				m_FileSize = 0;
			UINT16				m_MipMapCount = 0;
			UINT16				m_NameLength = 0;

			UINT32				m_Usage = 0;
			UINT8				m_MipmapGen = 0;
			UINT8				m_MipmapGenFilter = 0;
			UINT8				pad[2] = { 0 };
			UINT32				m_DataSize[6] = { 0 };
		};
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

#endif // __I3_IMAGE_FILE_I3I_H
