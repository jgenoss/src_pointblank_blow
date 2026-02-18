#include "stdafx.h"
#include "i3TDKObjectBase.h"
#include "i3TDKUpdateSystem.h"

#include "i3TDKPropertyContainer.h"
#include "i3TDKProperty.h"
#include "i3TDKPropertyBool.h"
#include "i3TDKPropertyInt.h"
#include "i3TDKPropertyReal.h"
#include "i3TDKPropertyString.h"
#include "i3TDKPropertyUint.h"

#include "i3TDKPropertySubItem.h"
#include "i3TDKPropertySubItemStatic.h"
#include "i3TDKPropertySubItemEdit.h"
#include "i3TDKPropertySubItemComboBox.h"
#include "i3TDKPropertySubItemColor.h"
#include "i3TDKPropertySubItemFilePath.h"
#include "i3TDKPropertySubItemUserCtrl.h"

#include "i3TDKWindowInfo.h"

#include "i3TDKGlobalVariable.h"
#include "i3TDKGlobalRes.h"

#include "PropertyGrid/i3TDKPropertyGridCtrl.h"

static bool s_bTDKRegisterMeta = false;

static bool _TDKTerminateProc( void * pData)
{
	i3TDKGlobalRes::Close();

	I3_SAFE_RELEASE( g_pTDKEnvSet);

	return true;
}

I3_EXPORT_TDK
void i3TDKRegisterMetas(void)
{
	i3BaseRegisterMetas();
	i3MathRegisterMetas();
	i3GfxRegisterMetas();
	i3SceneRegisterMetas();
	i3FrameworkRegisterMetas();
	i3OptRegisterMetas();

	if( s_bTDKRegisterMeta == false)
	{
		s_bTDKRegisterMeta = true;

		{
			#ifdef _DEBUG	// Debug
				g_hInstTDK = GetModuleHandle( _T("i3TDK_DEBUG.dll") );
			#else			// Distribute
				g_hInstTDK = GetModuleHandle( _T("i3TDK_DISTRIBUTE.dll") );
			#endif

			I3ASSERT( g_hInstTDK != nullptr);

			g_pTDKEnvSet = i3TDKEnvSet::new_object();

			i3TDKGlobalRes::Init();

			if( g_pTDKEnvSet->getCurWorkDir()[0] != '\0')
				i3ResourceFile::SetWorkingFolder( (char*) g_pTDKEnvSet->getCurWorkDir());

			i3System::AddTerminateProc( 10, _TDKTerminateProc, nullptr);
		}
	}
}

