#include "i3GfxType.h"
#include "i3ImageFileBGLoader.h"
#include "i3ImageFile.h"

I3_CLASS_INSTANCE( i3ImageFileBGLoader);

i3ImageFileBGLoader::~i3ImageFileBGLoader()
{
	Destroy();
}


bool i3ImageFileBGLoader::Create( char * pszName, UINT32 Attr /*= 0*/, UINT32 StackSize /*= 4096*/, void * pUserData /*= nullptr*/, INT32 priority /*= 3*/)
{
	m_pSignal = i3Signal::new_object();
	m_pSignal->Create();
	m_pSignal->Reset();

	return i3Thread::Create( pszName, Attr, StackSize, pUserData, priority);
}

/** \brief child class에서 호출하여 쓰레드를 종료시켜야 한다. */
void i3ImageFileBGLoader::Destroy( void)
{
	m_bRun = false;
	if( m_pSignal != nullptr)
	{
		m_pSignal->Raise();
	}

	while( m_bExitResult == false)
	{
		i3Timer::Sleep(10);
	}

	m_State = THREAD_STOPPED;

	I3_SAFE_RELEASE( m_pSignal);
}

UINT32 i3ImageFileBGLoader::OnRunning( void * pData)
{
	m_bExitResult = false;
	while( m_bRun)
	{
		I3ASSERT( m_pSignal != nullptr);
		m_pSignal->WaitFor( m_SignalTime);

		Run();

		m_pSignal->Reset();
	}

	m_bExitResult = true;
	return 0;
}

void i3ImageFileBGLoader::Run( void)
{
	if( (m_LoadInfo._pTempTex != nullptr) && (m_LoadInfo._pLoadableTex != nullptr) )
	{
		LoadTexture( &m_LoadInfo);
	}
}

void i3ImageFileBGLoader::RequestLoad( tagSPLITTEX_LOADINFO * pInfo)
{
	m_LoadInfo._pTempTex = pInfo->_pTempTex;
	I3_REF_CHANGE( m_LoadInfo._pLoadableTex, pInfo->_pLoadableTex);

	m_pSignal->Raise();
}

void i3ImageFileBGLoader::LoadTexture( tagSPLITTEX_LOADINFO * pInfo)
{
	i3ImageFile file;

	file.EnableDump( true);

	// load
	if( file.LoadI3I( pInfo->_pTempTex->GetName(), pInfo->_pLoadableTex) != nullptr)
	{
		I3_MUST_RELEASE(pInfo->_pLoadableTex);
	}
	else
	{
		I3PRINTLOG(I3LOG_FATAL,  "can't load texture %s", pInfo->_pTempTex->GetName());
	}

	pInfo->_pTempTex->BindFullLoadedTexture( pInfo->_pLoadableTex);

	//I3TRACE( "Image BGLoad Complate : %s\n", pInfo->_pTempTex->GetName());

#if defined( I3_DEBUG) && 0
	static int fa = 0;
	i3ImageFile sfile;
	i3::string str;
	i3::sprintf( str, "D:/full%d.i3i", fa);
	fa++;
	sfile.Save( str.c_str(), pInfo->_pLoadableTex);
#endif

	// release
	pInfo->_pTempTex = nullptr;
	I3_SAFE_RELEASE( pInfo->_pLoadableTex);
}

