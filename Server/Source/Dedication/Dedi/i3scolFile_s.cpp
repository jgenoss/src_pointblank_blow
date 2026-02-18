#include "pch.h"
#include "NxShapeSet.h"
#include "i3scolFile_s.h"


i3scolFile::i3scolFile(void)
{
	m_hFile = INVALID_HANDLE_VALUE;
	i3mem::FillZero(&m_head, sizeof(m_head));
	
	m_head.size			= sizeof(m_head);
	m_head.version		= I3SCOL_VERSION1;

	m_bSave				= FALSE;
	m_index				= 0;
	m_dwWritePos		= 0;
}

i3scolFile::~i3scolFile(void)
{
}

// 파일 오픈 (read mode는 dedication server에서 사용된다)
HANDLE i3scolFile::Open(const char* szFilePath, BOOL bSave)
{
	// file mode
	m_bSave = bSave;
	m_index = 0;

	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle( m_hFile);
	}

	if(bSave) // write mode
	{
		// open file for writing
		m_hFile = CreateFile(szFilePath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
		if(INVALID_HANDLE_VALUE == m_hFile ) return m_hFile;
		
		// skip header
		if(INVALID_SET_FILE_POINTER != ::SetFilePointer(m_hFile, m_head.size, 0, FILE_BEGIN))
		{
			return m_hFile; // success
		}
	}
	else
	{
		// open file for reading
		m_hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if (INVALID_HANDLE_VALUE == m_hFile) return m_hFile;
		
		// read header
		DWORD dwBytesRead = 0;
		if( FALSE == ReadFile(m_hFile, &m_head, sizeof(DS_I3SCOL_HEADER), &dwBytesRead, NULL) )
		{
			CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
			return m_hFile;
		}
		if( I3SCOL_VERSION2 == m_head.version )
		{
			if( FALSE == ReadFile(m_hFile, m_head.m_ChecksumCode, sizeof(DS_I3SCOL_HEADER2)-sizeof(DS_I3SCOL_HEADER), &dwBytesRead, NULL) )
			{
				CloseHandle(m_hFile);
				m_hFile = INVALID_HANDLE_VALUE;
				return m_hFile; // success
			}
		}
	}
	return m_hFile;
}


// 파일 닫기
void i3scolFile::Close(void)
{
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		if(m_bSave)
		{
			// write header
			if(INVALID_SET_FILE_POINTER != ::SetFilePointer(m_hFile, 0, 0, FILE_BEGIN))
			{
				DWORD dwBytesWritten = 0;
				WriteFile(m_hFile, &m_head, sizeof(m_head), &dwBytesWritten, NULL);
			}
		}
		
		CloseHandle( m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

// 오브젝트 읽기 (내부에서 인덱스를 유지하고 있기 때문에 읽기/쓰기를 섞어 쓰면 안됨)
DWORD i3scolFile::Read(NxShapeSet* pShapeSet)
{
	I3ASSERT(pShapeSet);
	
	DWORD dwBytesRead	= 0;
	DWORD dwBytesTotal	= 0;
	BOOL  bError = FALSE;

	DS_I3SCOL_DESC	desc;
	i3mem::FillZero(&desc, sizeof(desc));

	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		// set file pointer
		if(INVALID_SET_FILE_POINTER == SetFilePointer(m_hFile, m_head.size + m_index * sizeof(desc), 0, FILE_BEGIN)) 
		{
			return 0;
		}
		
		// read description
		if(FALSE == ReadFile(m_hFile, &desc, sizeof(DS_I3SCOL_DESC), &dwBytesRead, NULL))
		{
			return 0;
		}
		if ( dwBytesRead == 0 )
		{
			return 0;
		}
		
		dwBytesTotal += dwBytesRead;
		
		pShapeSet->reset();
		
		// memory alloc
		if( desc.tsize > 0 )
		{
			pShapeSet->m_pI3Terrain			= (I3_PHYSIX_TERRAIN*) malloc( desc.tsize );
			pShapeSet->m_numTerrains			= desc.tsize / desc.tstep;
		}
		if( desc.vsize > 0 )
		{
			pShapeSet->m_pvVertList			= (NxF32*)	malloc( desc.vsize);
			pShapeSet->m_numVertices		= desc.vsize / desc.vstep;
		}
		if( desc.fsize > 0 )
		{
			pShapeSet->m_pI3FaceList		= (I3_PHYSIX_TRI*)	malloc( desc.fsize );
			pShapeSet->m_numFaces			= desc.fsize / desc.fstep;
		}
		if( desc.nxsize > 0 )
		{
			pShapeSet->m_pNxData			= (char*)malloc( desc.nxsize );
			pShapeSet->m_NxDataSize			= desc.nxsize;
		}
		
		// setup structure
		pShapeSet->m_matTransform		= desc.transform;	// transform
		pShapeSet->m_ShapeType			= desc.shapeType;
		pShapeSet->m_ShapeGroup			= desc.group;		// collision group
		pShapeSet->m_restitution		= desc.restitution;
		pShapeSet->m_staticFriction		= desc.staticFriction;	
		pShapeSet->m_dynamicFriction	= desc.dynamicFriction;

		// read variable size data
		// terrain
		if(INVALID_SET_FILE_POINTER == SetFilePointer(m_hFile, desc.toffset, 0, FILE_BEGIN))
		{
			bError = TRUE; goto exit_tag;
		}
		if(FALSE == ReadFile(m_hFile, pShapeSet->m_pI3Terrain, desc.tsize, &dwBytesRead, NULL)) 
		{
			bError = TRUE; goto exit_tag;
		}
		dwBytesTotal += dwBytesRead;
		// vertex list
		if(FALSE == ReadFile(m_hFile, pShapeSet->m_pvVertList, desc.vsize, &dwBytesRead, NULL))
		{
			bError = TRUE; goto exit_tag;
		}
		dwBytesTotal += dwBytesRead;
		// face list
		if(FALSE == ReadFile(m_hFile, pShapeSet->m_pI3FaceList, desc.fsize, &dwBytesRead, NULL))
		{
			bError = TRUE; goto exit_tag;
		}
		dwBytesTotal += dwBytesRead;
		// collection data
		if(FALSE == ReadFile(m_hFile, pShapeSet->m_pNxData, desc.nxsize, &dwBytesRead, NULL))
		{
			bError = TRUE; goto exit_tag;
		}
		dwBytesTotal += dwBytesRead;

		m_index++;
	}

exit_tag:
	if( bError )
	{
		pShapeSet->reset();
		return 0;
	}

	return dwBytesTotal;
}

void i3scolFile::GetChecksum( UINT32* ui32Checksum )
{
	memcpy( ui32Checksum, m_head.m_ChecksumCode, sizeof(UINT32)*4 );
}