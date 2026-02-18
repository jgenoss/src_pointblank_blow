#include "i3Base.h"
#include "i3MathType.h"
#include "i3Vector2Array.h"
#include "i3Vector3Array.h"
#include "i3Vector4Array.h"
#include "i3Volume.h"
#include "i3BoundBox.h"
#include "i3MatrixStack.h"
#include "i3MatrixArray.h"
#include "i3MatrixObject.h"
#include "i3MatrixObjectList.h"
#include "i3TransformSource.h"
#include "i3TransformSequence.h"
#include "i3TransformSourceBind.h"
#include "i3TransformSourceCombiner.h"
#include "i3TransformModifier.h"
#include "i3KeyframeTable.h"
#include "i3MathGlobalVariable.h"
#include "i3RectCoordSource.h"
#include "i3Vector3List.h"
#include "i3Vector4List.h"
#include "i3PolygonList.h"
#include "i3Vector3List16Bits.h"
#include "i3Vector4List16Bits.h"

#include "i3Animation.h"
#include "i3Animation2.h"
#include "i3AnimationPackFile.h"
#include "i3AnimationResManager.h"

static bool s_bMathMetaRegistered = false;

I3_EXPORT_MATH void i3MathRegisterMetas(void)
{
	i3BaseRegisterMetas();

	if( s_bMathMetaRegistered == false)
	{
		{
			i3AnimationResManager * pManager = i3AnimationResManager::new_object_ref();

			i3ResourceFile::AddResourceManager( i3Animation::static_meta(), pManager);
			i3ResourceFile::AddResourceManager( i3Animation2::static_meta(), pManager);
		}

		s_bMathMetaRegistered = true;
	}
}

#if (defined( I3_WINDOWS) || defined( I3_WINCE) ) && defined( I3_DLL )
BOOL WINAPI DllMain( HANDLE hInst, DWORD dwReason, LPVOID)
{
	switch( dwReason)
	{
		case DLL_PROCESS_ATTACH :	//i3MathRegisterMetas();
			i3System::HoldDllModule((HMODULE)hInst);
			break;
	}

	return TRUE;
}

#endif

