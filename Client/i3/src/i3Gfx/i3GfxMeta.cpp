#include "i3GfxType.h"
#include "i3Base.h"
#include "i3Math.h"
#include "i3GfxResource.h"
#include "i3RenderTarget.h"
#include "i3Texture.h"
#include "i3TextureMem.h"
#include "i3TextureCube.h"
#include "i3TextureCubeMem.h"
#include "i3VertexArray.h"
#include "i3ImageFile.h"
#include "i3ImageFileBMP.h"
#include "i3ImageFileI3I.h"
#include "i3Light.h"
#include "i3IndexArray.h"
#include "i3GfxCaps.h"
#include "i3PerformanceReport.h"
#include "i3GPUCommandList.h"
#include "i3TextureSequence.h"
#include "i3RenderContext.h"
#include "i3GfxOption.h"
#include "i3TextureResManager.h"
#include "i3DisplayList.h"
#include "i3UVPositioner.h"

#ifndef I3_WINCE
	#include "i3Shader.h"
	#include "i3ShaderCache.h"
	#include "i3ShaderGen.h"
#endif

#if defined( I3G_DX)
#include "i3TextureDX.h"
#include "i3VertexArrayDX.h"
#include "i3VertexArrayMem.h"
#include "i3IndexArrayMem.h"
#include "i3GfxFontDX.h"
#include "i3GfxFontDX2.h"
#include "i3GfxMovie.h"
#include "i3ShaderCode.h"
#include "i3TextureCubeDX.h"
#include "i3GfxVertexBank.h"

#include "i3GfxOcclusionTest.h"

#include "i3PostProcess.h"
#include "i3PostProcessManager.h"
#include "i3PostProcessBloom.h"
#include "i3PostProcessNightVision.h"
#include "i3PostProcessMotionBlur.h"
#include "i3PostProcessHaze.h"
#include "i3PostProcessGamma.h"

#include "i3RasterizeChainBase.h"
#include "i3RasterizeChainContext.h"
#include "i3RasterizeChainManager.h"
#include "i3RasterizeChain_BloomCombine.h"
#include "i3RasterizeChain_BrightPathExtract.h"
#include "i3RasterizeChain_DownScale.h"
#include "i3RasterizeChain_GaussianBlur.h"
#include "i3RasterizeChain_NightVision.h"
#include "i3RasterizeChain_SimpleMotionBlur.h"
#include "i3RasterizeChain_ToneMapping.h"

#elif defined( I3G_XBOX)
#include "i3TextureXbox.h"
#elif defined( I3G_PSP)
#include "i3TexturePSP.h"
#include "i3VertexArrayPSP.h"
#elif defined( I3G_OGLES)
#include "i3TextureOGLES.h"
#include "i3VertexArrayOGLES.h"
#elif defined( I3_WINCE)
#include "i3TextureWINCE.h"
#include "i3VertexArrayWINCE.h"
#endif

static bool s_bGfxRegistered = false;

I3_EXPORT_GFX void i3GfxRegisterMetas(void)
{
	i3BaseRegisterMetas();
	i3MathRegisterMetas();

	if( s_bGfxRegistered == false)
	{
		{
			i3TextureResManager * pManager = i3TextureResManager::new_object_ref();

			i3ResourceFile::AddResourceManager( i3Texture::static_meta(), pManager);
		}

		s_bGfxRegistered = true;
	}
}

#if defined( I3_WINDOWS) && defined( I3_DLL)
BOOL WINAPI DllMain( HANDLE hInst, DWORD dwReason, LPVOID)
{
	switch( dwReason)
	{
		case DLL_PROCESS_ATTACH :	//i3GfxRegisterMetas();
			i3System::HoldDllModule((HMODULE)hInst);
			break;
	}

	return true;
}

#endif
