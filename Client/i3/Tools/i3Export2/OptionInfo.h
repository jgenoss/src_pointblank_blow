#if !defined( __I3EXP_OPTION_INFO_H)
#define __I3EXP_OPTION_INFO_H

namespace i3tool
{
	namespace exporter
	{
		enum KEYSAMPLE_TYPE
		{
			KEYSAMPLE_PICKEDONLY = 0,
			KEYSAMPLE_SAMPLE,
		};
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace i3tool
{
	namespace exporter
	{
		namespace pack
		{
			struct OPTION_INFO
			{
				// 선택한 것만 Export 한다.
				bool		m_bExportSelectedOnly = false;

				// Object Type : Light
				bool		m_bExportLight = false;

				// 문제점을 파악하여 Report 한다.
				bool		m_bReportError = false;

				// Animation을 Export 한다.
				bool		m_bExportAnim = false;

				// Object Type : Mesh
				bool		m_bExportMesh = false;

				// Object Type : Camera
				bool		m_bExportCamera = false;

				// Physics 관련 Geometry를 Rendering 가능하도록 Export할 것인지
				// 그렇지 않은지를 설정
				bool		m_bExportPhysicsGeometry = false;

				KEYSAMPLE_TYPE		m_KeySampleType = KEYSAMPLE_PICKEDONLY;

				INT32		m_SamplingInterval = 0;

				UINT32		m_bExportI3S = 0;

				INT32		m_SelectBoneWndX = 0;
				INT32		m_SelectBoneWndY = 0;
				INT32		m_SelectBoneWndCX = 0;
				INT32		m_SelectBoneWndCY = 0;

				char		m_szExcludeNodeName[64] = { 0 };

				UINT32		m_bForceExportAnim = 0;
				UINT32		m_bViewAfterExport = 0;

				REAL32		m_SkinWidth = 0.0f;
				BOOL		m_bRotateAxis = FALSE;
				BOOL		m_bExportShapeAsConvex = FALSE;
				BOOL		m_bCreateRappingConvex = FALSE;
				BOOL		m_bCreateForSimulate = FALSE;
				UINT32		pad0[1] = { 0 };

				char		m_szLastPath[260] = { 0 };
				char		m_szLastName[260] = { 0 };

				UINT32		m_bTreatControlAsBone = 0;
				BOOL		m_bUseMaterialName = FALSE;		/// Material Name으로 Node를 구성. false이면 Geometry Name으로 구성한다.
				UINT32		pad1[31] = { 0 };
			};


			inline void I3EXP_OPTION_INFO_Copy(OPTION_INFO* out, const OPTION_INFO* in)
			{
				if (in && out)
					memcpy(out, in, sizeof(OPTION_INFO));
			}

			inline void I3EXP_OPTION_INFO_SetDefault(OPTION_INFO* opt)
			{
				if (!opt)
					return;

				memset(opt, 0, sizeof(OPTION_INFO));
				opt->m_bExportI3S = TRUE;
				opt->m_bExportSelectedOnly = false;
				opt->m_bReportError = true;
				opt->m_bExportAnim = false;
				opt->m_bExportMesh = true;
				opt->m_bExportCamera = true;
				opt->m_bExportLight = true;
				opt->m_bExportPhysicsGeometry = false;
				opt->m_KeySampleType = KEYSAMPLE_PICKEDONLY;
				opt->m_SamplingInterval = 10;
				opt->m_SelectBoneWndX = -1;
				opt->m_SelectBoneWndY = -1;
				opt->m_SelectBoneWndCX = -1;
				opt->m_SelectBoneWndCY = -1;
				opt->m_szExcludeNodeName[0] = 0;
				opt->m_SkinWidth = 0.01f;
				opt->m_bRotateAxis = TRUE;
				opt->m_bExportShapeAsConvex = FALSE;
				opt->m_bCreateRappingConvex = FALSE;
				opt->m_bCreateForSimulate = FALSE;
			}
		}
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

#endif
