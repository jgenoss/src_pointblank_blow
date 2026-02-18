#include "stdafx.h"
#include "i3SoundContext.h" 

#if defined( I3G_PSP)
#include "i3SoundPSP.h"
#include "i3SoundPlayStatePSP.h"

I3_CLASS_INSTANCE( i3SoundContext, i3ElementBase);
/*-----------------------------------------------------------------------------
// Name: i3SoundContext::i3SoundContext(void)
// Desc: Create
//----------------------------------------------------------------------------*/
i3SoundContext::i3SoundContext(void)
{
	m_pSasThread = NULL;
}

/*-----------------------------------------------------------------------------
// Name: i3SoundContext::~i3SoundContext(void)
// Desc: Destroy
//----------------------------------------------------------------------------*/
i3SoundContext::~i3SoundContext(void)
{

}

BOOL	i3SoundContext::Create(  void * pInitParam )
{
	I3SND_CONTEXTCREATE_PARAM * pParam = (I3SND_CONTEXTCREATE_PARAM*)pInitParam;

	m_InitParam.m_nFlags		= pParam->m_nFlags;
	m_InitParam.m_nVoiceCount	= pParam->m_nVoiceCount;

	if( InitSasModule() == FALSE )
	{
		I3TRACE("Failed i3SoundContext::Create( PSP ) >> InitSasModule()\n");
		return FALSE;
	}

	if( InitWaveModule() == FALSE )
	{
		I3TRACE("Failed i3SoundContext::Create( PSP ) >> InitSasModule()\n");
		return FALSE;
	}

	StartSasThread();

	return TRUE;
}

BOOL i3SoundContext::InitSasModule()
{
	INT32			nResult = 0;
	char			szPath[64];
	char			szModulePath[64];

	//Load Module
	SceUID				sasModule	= 0;
	SceKernelLMOption	kMode;

	strcpy( szPath, i3FileStream::GetPrefix());

	kMode.size = sizeof( kMode );
	kMode.mpidtext = SCE_KERNEL_PRIMARY_KERNEL_PARTITION;
	kMode.mpiddata = SCE_KERNEL_PRIMARY_KERNEL_PARTITION;
	kMode.position = SCE_KERNEL_LMWO_POS_Low;
	kMode.access   = SCE_KERNEL_LMWO_ACCESS_Noseek;

	//
	sprintf( szModulePath, "%sPRX/sc_sascore.prx", szPath );
	sasModule = sceKernelLoadModule( szModulePath, 0, &kMode );
	if( sasModule < 0 )
	{
		I3TRACE("Error i3SoundContext::sceKernelLoadModule(PSP)>> %s \n", szModulePath);
		return FALSE;
	}

	//
	nResult = sceKernelStartModule( sasModule, 0, NULL, NULL, NULL );
	if( nResult < 0)
	{
		I3TRACE( "Error i3SoundContext::sceKernelStartModule( PSP )>> %s\n", szModulePath);
		I3TRACE( "sceKernelStartModule returned Error Code : %x\n", nResult );
		return FALSE;
	}
	else
	{
		I3TRACE( "%s -> Module Start OK!\n", szModulePath);
	}

	return TRUE;
}

BOOL	i3SoundContext::InitWaveModule()
{
	INT32	nResult;

	nResult = sceWaveInit();
	if( nResult != SCE_OK )
	{
		I3TRACE("ERROR i3SoundContext::InitWaveModule( PSP )::sceWaveInit()\n");
		I3TRACE("sceWaveInit returned Error Code : %x\n", nResult );
		return FALSE;
	}

	nResult = sceWaveAudioSetSample( 0, I3SND_SAMPLE_SIZE );
	if( nResult != SCE_OK )
	{
		I3TRACE("ERROR i3SoundContext::InitWaveModule( PSP ):: sceWaveAudioSetSample()\n");
		I3TRACE("sceWaveAudioSetSample returned Error Code : %x\n", nResult );
		return FALSE;
	}

	nResult = sceSasInit();
	if( nResult != SCE_OK )
	{
		I3TRACE("ERROR i3SoundContext::InitWaveModule( PSP ):: sceSasInit()()\n");
		I3TRACE("sceSasInit() returned Error Code : %x\n", nResult );
		return FALSE;
	}

	nResult = sceWaveStart( SCE_KERNEL_MODULE_INIT_PRIORITY);
	if( nResult != SCE_OK )
	{
		I3TRACE("ERROR i3SoundContext::InitWaveModule( PSP ):: sceWaveStart()()\n");
		I3TRACE("sceWaveStart() returned Error Code : %x\n", nResult );
		return FALSE;
	}

	return TRUE;
}

BOOL	i3SoundContext::Destroy()
{
	BOOL	rv = TRUE;
	UINT32	nResult;	

	nResult = sceWaveEnd();
	if( nResult != SCE_OK )
	{
		#ifdef I3_DEBUG
		I3TRACE("sceWaveEnd() returned Error Code: %X\n", nResult );
		#endif
		rv = FALSE;
	}

	nResult = sceWaveExit();
	if( nResult != SCE_OK )
	{
		#ifdef I3_DEBUG
		I3TRACE( "sceWaveExit() returned Error Code: %X\n", nResult );
		#endif
		rv = FALSE;
	}

	return rv;
}

BOOL	i3SoundContext::StartSasThread()
{
	m_pSasThread = i3SoundSasThread::NewObject();
	m_pSasThread->Create("SasThread", NULL);
	return TRUE;
}

BOOL	i3SoundContext::StopSasThread()
{
	m_pSasThread->Terminate();
	I3_SAFE_RELEASE( m_pSasThread );
	return TRUE;
}
#endif	//#if defined( I3G_PSP)
