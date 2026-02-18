#include "i3FrameworkPCH.h"
#include "i3Framework.h"
#include "i3GameNode.h"
#include "i3GameRes.h"
#include "i3GameResSceneGraph.h"
#include "i3GameResChara.h"
#include "i3GameResCamera.h"
#include "i3GameResAnimation.h"
#include "i3GameResTexture.h"
#include "i3GameResWorld.h"
#include "i3GameResBinary.h"
#include "i3GameResPath.h"
#include "i3GameResObject.h"
#include "i3GameResRespawn.h"
#include "i3GameResEvent.h"
#include "i3GameSceneInfo.h"
#include "i3ResourceManager.h"
#include "i3GameObjBase.h"
#include "i3GameObj.h"
#include "i3CameraObject.h"
#include "i3Stage.h"
#include "i3Viewer.h"
#include "i3AI.h"
#include "i3AIState.h"
#include "i3FontManager.h"
#include "i3StageInfo.h"
#include "i3StageStack.h"
#include "i3FadeEffect.h"
#include "i3ViewerFramework.h"
#include "i3InputDispatchManager.h"
#include "i3Chara.h"
#include "i3SgLayer.h"
#include "i3AIResManager.h"
#include "i3InstanceManager.h"
#include "i3World.h"
#include "i3AIDriver.h"
#include "i3Object.h"
#include "i3EffectManager.h"
#include "i3LightObject.h"
#include "i3RespawnObj.h"
#include "i3WorldSectionTable.h"
#include "i3PathPoint.h"
#include "i3PathLine.h"

#include "i3Trigger.h"
#include "i3GameObjRef.h"

#include "i3ActionMeta.h"

#include "i3UI.h"

#include "i3Lua.h"

static bool s_bInitFrameworkMeta = false;

#if defined( I3_WINDOWS)
HINSTANCE	g_hInstFramework = nullptr;
#endif

#pragma optimize ("g",off)

static bool AIP_Mark = false;

I3_EXPORT_FRAMEWORK
void AIP_Marker_Framework(void) { AIP_Mark = true; }

I3_EXPORT_FRAMEWORK
void i3FrameworkRegisterMetas(void)
{
	if ( AIP_Mark == false)
		AIP_Marker_Framework();

	i3BaseRegisterMetas();
	i3InputRegisterMetas();
	i3SceneRegisterMetas();

	if( s_bInitFrameworkMeta == false)
	{
		{
			i3AIResManager * pManager = i3AIResManager::new_object_ref();
			i3ResourceFile::AddResourceManager( i3AI::static_meta(), pManager);
		}

		s_bInitFrameworkMeta = true;
	}
}

#pragma optimize ("g",on)

#if defined( I3_WINDOWS) && defined( I3_DLL)
BOOL WINAPI DllMain( HANDLE hInst, DWORD dwReason, LPVOID)
{
	switch( dwReason)
	{
		case DLL_PROCESS_ATTACH :	//i3FrameworkRegisterMetas();
			i3System::HoldDllModule((HMODULE)hInst);
			g_hInstFramework  = (HINSTANCE) hInst;
			break;
	}

	return true;
}

#endif
