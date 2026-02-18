#include "i3GfxType.h"
#include "i3GfxResource.h"
#include "../i3Base/i3Debug.h"
#include "i3GfxUtil.h"
#include "i3GfxCaps.h"
#include "i3RenderContext.h"
#include "i3Base/string/ext/integral_to_comma_str.h"


I3_CLASS_INSTANCE( i3GfxResource);

static i3GfxResource *		g_pGfxResHead = nullptr;
static i3Mutex *			g_pGfxResMutex = nullptr;

CALLBACK_EXTERNAL_OUTOFMEMORY i3GfxResource::m_CallBackExternalOutOfMemory = nullptr;

i3GfxResource *	 i3GfxResource::getHeadOfGfxResource(void)
{
	return g_pGfxResHead;
}

i3GfxResource::i3GfxResource(void)
{
	LinkToResList();

#if defined( I3_DEBUG)
	::GetLocalTime( &m_CreateTime );
#endif
}

i3GfxResource::~i3GfxResource(void)
{
	UnlinkFromResList();
}

void i3GfxResource::DeleteStaticMember()
{
	I3_SAFE_RELEASE(g_pGfxResMutex);
}

static bool _GfxResourceTerminateProc( void * pData)
{
	I3_SAFE_RELEASE( g_pGfxResMutex);

	return true;
}

void i3GfxResource::LinkToResList(void)
{
	if( g_pGfxResMutex == nullptr)
	{
		g_pGfxResMutex = i3Mutex::new_object();

		i3System::AddTerminateProc( 4, _GfxResourceTerminateProc, nullptr);
	}

	g_pGfxResMutex->Enter();

	m_pNextRes = g_pGfxResHead;
	g_pGfxResHead = this;

	g_pGfxResMutex->Leave();
}

void i3GfxResource::UnlinkFromResList(void)
{
	i3GfxResource * pTemp, * pPrev;

//	I3ASSERT( g_pGfxResMutex != nullptr);			// 뮤텍스가 소거된 뒤에도 리소스가 제거될수 있으므로 ASSERT를 지우고,
	I3ASSERT( g_pGfxResHead != nullptr);			// 널검사로 바꿉니다.. (2012.09.11.수빈)
	
	if (g_pGfxResMutex)
		g_pGfxResMutex->Enter();

	if( g_pGfxResHead == this)
	{
		g_pGfxResHead = m_pNextRes;
	}
	else
	{
		pTemp = g_pGfxResHead;

		while((pTemp != nullptr) && (pTemp->m_pNextRes != this))
		{
			pPrev = pTemp;
			pTemp = pTemp->m_pNextRes;
		}
				
		if (pTemp)
		{
			pTemp->m_pNextRes = m_pNextRes;
		}
		else
		{
			I3ASSERT(pTemp != nullptr);
		}
	}

	m_pNextRes = nullptr;

	if (g_pGfxResMutex)
		g_pGfxResMutex->Leave();
}

void i3GfxResource::SetUsageFlag( UINT32 flag)
{
	m_Usage = flag;

	if( m_Usage & I3G_USAGE_RENDERTARGET)
	{
		setRestoreType( I3G_RESTORE_NONE);
	}
}

void i3GfxResource::setRestorePoint( i3Stream * pStream)
{
	if( i3::kind_of<i3FileStream*>(pStream))
	{
		setRestoreType( I3G_RESTORE_FILE);
		setResourceLoadPath( ((i3FileStream *) pStream)->GetFileNameString());
	}
	else if( i3::kind_of<i3MemStream*>(pStream))
	{
		i3MemStream * pMem = (i3MemStream *) pStream;

		setRestoreType( I3G_RESTORE_MEM);
		setRestoreData( (char *) pMem->GetHandle());
		m_ResDataSize = pMem->GetSize();
	}

	setResourceLoadOffset( pStream->GetPosition());
}

bool i3GfxResource::Destroy( bool bLostDevice)
{
	SetLoaded(false);

	return true;
}

bool i3GfxResource::Revive( i3RenderContext * pCtx)
{
	i3ResourceFile resFile;

	switch( m_RestoreType)
	{
		case I3G_RESTORE_FILE :
			{
				i3FileStream strm;

				I3ASSERT( !m_strResPath.empty() ); // (m_szResPath[0] != 0);

				if( strm.Open( m_strResPath.c_str(), STREAM_READ | STREAM_SHAREREAD) == false)
				{
					I3PRINTLOG(I3LOG_FATAL,  "RESTORE : [%s] %s 파일을 읽어들일 수 없습니다.", GetName(), m_strResPath);
					return false;
				}

				strm.SetPosition( (INT32) m_ResOffset, STREAM_BEGIN);

				resFile.SetStream( &strm);

				if( OnLoad( &resFile) == STREAM_ERR)
				{
					I3PRINTLOG(I3LOG_FATAL,  "RESTORE : [%s] %s 파일을 읽어들일 수 없습니다.", GetName(), m_strResPath);
					return false;
				}

				resFile.SetStream( nullptr);

				SetLoaded(true);
			}
			break;

		case I3G_RESTORE_MEM :
			{
				i3MemStream strm;

				I3ASSERT( m_pResData != nullptr);

				if( strm.Open( m_pResData, STREAM_READ | STREAM_SHAREREAD, m_ResDataSize) == false)
				{
					I3PRINTLOG(I3LOG_FATAL,  "메모리로부터 %s Resource를 읽어들일 수 없습니다.", GetName());
					return false;
				}

				strm.SetPosition( (INT32) m_ResOffset, STREAM_BEGIN);

				resFile.SetStream( &strm);

				if( OnLoad( &resFile) == STREAM_ERR)
				{
					I3PRINTLOG(I3LOG_FATAL,  "메모리로부터 %s Resource를 읽어들일 수 없습니다.", GetName());
					return false;
				}

				resFile.SetStream( nullptr);

				SetLoaded(true);
			}
			break;
	}

	return true;
}

void i3GfxResource::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3ResourceObject::CopyTo( pDest, method);

	i3GfxResource * pRes = (i3GfxResource *) pDest;

	pRes->m_Usage = m_Usage;
	pRes->m_RestoreType = m_RestoreType;
	pRes->m_strResPath = m_strResPath;
	pRes->m_ResDataSize = m_ResDataSize;
	pRes->m_ResOffset = m_ResOffset;
}

#if defined( I3_DEBUG)
#include "i3Surface.h"
#include "i3Texture.h"

void i3GfxResource::DumpTextures(void)
{
	i3GfxResource * pTemp = g_pGfxResHead;
	UINT32		accmSize = 0, i = 0, j, texCount = 0;
	char conv[256];

	I3TRACE( "-------------------------------- Textures -----------------------------------\n");

	while( pTemp != nullptr)
	{
		if( i3::kind_of<i3Texture*>(pTemp))
		{
			i3Texture * pTex = (i3Texture *) pTemp;
			INT32 width, height, bits;
			UINT32 size;

			size = 0;

			//if( i3Gfx::GetImageFormatBitCount( pTex->GetFormat()) > 8)
			{
				if( pTex->getSurfaceCount() > 0)
				{
					width = pTex->GetWidth();
					height = pTex->GetHeight();
					bits = pTex->GetBitsPerPixel();
					size = 0;

					for( j = 0; j < (UINT32) pTex->getSurfaceCount(); j++)
					{
						i3Surface * pSurface = pTex->getSurface( j);

						size += pSurface->GetDataSize();
					}
				}
				else
				{
					width = 0;
					height = 0;
					bits = 0;
					size = 0;
				}

				//i3::integral_to_comma_str( size, conv, sizeof(conv) - 1);

				INT32 iDate	= (pTemp->m_CreateTime.wYear * 10000) + (pTemp->m_CreateTime.wMonth) * 100 + pTemp->m_CreateTime.wDay;

	#ifndef I3_WINCE
				I3TRACE( "%4d,%4d,%2d,%s,%s,%s(%d) : [%d:%02d:%02d:%02d]\n", width, height, bits, pTemp->getResourceLoadPath(),
					pTemp->GetName(), pTemp->_getSourceFileName(), pTemp->_getSourceLineCount(),
					iDate, pTemp->m_CreateTime.wHour, pTemp->m_CreateTime.wMinute, pTemp->m_CreateTime.wSecond);
	#endif
			}

			accmSize += size;
			texCount++;
		}

		i++;
		pTemp = pTemp->getNextRes();
	}

	i3::integral_to_comma_str( accmSize, conv, sizeof(conv) - 1);
	I3TRACE( "Total %d textures - %s bytes of VRAM are used\n", texCount, conv);
	I3TRACE( "---------------------------------------------------------------------------------\n");
}

void i3GfxResource::DumpVertexArrays(void)
{
	i3GfxResource * pTemp = g_pGfxResHead;

	I3TRACE( "-------------------------------- Vertex Arrays -----------------------------------\n");

	while( pTemp != nullptr)
	{
		if( i3::kind_of<i3VertexArray*>(pTemp))
		{
			i3VertexArray * pVA = (i3VertexArray *) pTemp;

			i3::string str;
			switch( pVA->getRestoreType())
			{
			case I3G_RESTORE_FILE : str = "FILE";	break;
			case I3G_RESTORE_MEM :	str = "MEM";	break;
			default : str = "UNKNOWN";	break;
			}

			I3TRACE( "CNT:%5d, RESTORE:%s  - %s, %s (%d)\n", pVA->GetCount(), str, pVA->getResourceLoadPath(), pVA->_getSourceFileName(), pVA->_getSourceLineCount());
		}

		pTemp = pTemp->getNextRes();
	}

	I3TRACE( "---------------------------------------------------------------------------------\n");
}

void i3GfxResource::DumpIndexArrays( void)
{
	i3GfxResource * pTemp = g_pGfxResHead;

	I3TRACE( "-------------------------------- Index Arrays -----------------------------------\n");

	while( pTemp != nullptr)
	{
		if( i3::kind_of<i3IndexArray*>(pTemp))
		{
			i3IndexArray * pIA = (i3IndexArray *) pTemp;

			i3::string str;
			switch( pIA->getRestoreType())
			{
			case I3G_RESTORE_FILE : str = "FILE";	break;
			case I3G_RESTORE_MEM :	str = "MEM";	break;
			default : str = "UNKNOWN";	break;
			}

			I3TRACE( "CNT:%5d, RESTORE:%s  - %s, %s (%d)\n", pIA->GetCount(), str, pIA->getResourceLoadPath(), pIA->_getSourceFileName(), pIA->_getSourceLineCount());
		}

		pTemp = pTemp->getNextRes();
	}

	I3TRACE( "---------------------------------------------------------------------------------\n");
}

void i3GfxResource::Dump( void)
{
	DumpVertexArrays();
	DumpIndexArrays();
	DumpTextures();
}

void i3GfxResource::GetStat( I3G_RES_STAT * pInfo)
{
	i3GfxCaps * pCaps = i3RenderContext::GetCaps();
	i3GfxResource * pTemp = g_pGfxResHead;

	pInfo->reset();

	pInfo->m_TotalVRAM = pCaps->getVRAMSize();

	while( pTemp != nullptr)
	{
		pTemp->OnQueryStat( pInfo);

		pTemp = pTemp->getNextRes();
	}
}

#endif
