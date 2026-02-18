#include "pch.h"
#include "StageTable.h"
#include "XmlWrapper.h"
#include "NationalCodeDef.h"

StageTable StageTable::m_Instance;

StageTable::StageTable(void)
{
}

StageTable::~StageTable(void)
{
}

void StageTable::SetNationalColumnName(UINT8 ui8NationalCode)
{
	switch(ui8NationalCode)
	{
	case Dev: // 0
		i3String::Copy( m_astrColumnName[0], "X_Dev", MAX_STRING_COUNT ); break;
	case Indonesia : //1
		i3String::Copy( m_astrColumnName[0], "X_Indonesia", MAX_STRING_COUNT ); break;
	case Russia: //2
		i3String::Copy( m_astrColumnName[0], "X_Russia", MAX_STRING_COUNT ); break;
	case Thai: //3
		i3String::Copy( m_astrColumnName[0], "X_Thai", MAX_STRING_COUNT ); break;
	case Korea: //4
		i3String::Copy( m_astrColumnName[0], "X_Korea", MAX_STRING_COUNT ); break;
	case Turkey: //5
		i3String::Copy( m_astrColumnName[0], "X_Turkey", MAX_STRING_COUNT ); break;
	case Brazil: //6
		i3String::Copy( m_astrColumnName[0], "X_Brazil", MAX_STRING_COUNT ); break;
	case LatinAmerica: //7
		i3String::Copy( m_astrColumnName[0], "X_Latin_America", MAX_STRING_COUNT ); break;
	case NorthAmerica: //8
		i3String::Copy( m_astrColumnName[0], "X_NorthAmerica", MAX_STRING_COUNT ); break;
	case China: //9
		i3String::Copy( m_astrColumnName[0], "X_China", MAX_STRING_COUNT ); break;
	case Italy: //10
		i3String::Copy( m_astrColumnName[0], "X_Italy", MAX_STRING_COUNT ); break;
	case Philippines: //11
		i3String::Copy( m_astrColumnName[0], "X_Philippines", MAX_STRING_COUNT ); break;
	case MiddleEast: //12
		i3String::Copy( m_astrColumnName[0], "X_MiddleEast", MAX_STRING_COUNT ); break;
	case Taiwan: //13
		i3String::Copy( m_astrColumnName[0], "X_Taiwan", MAX_STRING_COUNT ); break;
	case Singmal: //14
		i3String::Copy( m_astrColumnName[0], "X_Singmal", MAX_STRING_COUNT ); break;
	case Vietnam: //15
		i3String::Copy( m_astrColumnName[0], "X_Vietnam", MAX_STRING_COUNT ); break;
	default:
		i3String::Copy( m_astrColumnName[0], "X__Default_", MAX_STRING_COUNT ); break;
	}

	i3String::Copy( m_astrColumnName[1], m_astrColumnName[0], MAX_STRING_COUNT );
	i3String::Copy( m_astrColumnName[2], m_astrColumnName[0], MAX_STRING_COUNT );
	m_astrColumnName[1][0] = 'Y';
	m_astrColumnName[2][0] = 'Z';
}

void StageTable::ParseStageInfo( IDispatch *pNode )
{
	i3mem::FillZero( m_abBombB,	sizeof( m_abBombB ) );

	XmlNodeWrapper xRoot(pNode);

	for( INT32 i = 0; i < xRoot.NumNodes(); i++ )
	{
		XmlNodeWrapper xBody = xRoot.GetNode(i);
		if ( xBody.GetName() != "KEY_1" )			continue;
		
		for( INT32 j = 0; j < xBody.NumNodes(); j++ )
		{
			XmlNodeWrapper xMap = xBody.GetNode(j);
			if ( xMap.GetName() != "KEY_2" )		continue;
			if ( "_N/A" == xMap.GetAttribVal(0) )	continue;

			I3TRACE("%s\n", xMap.GetAttribVal(0).c_str());

			INT32 i32OrdCnt = 0;
			INT32 i32Ord[STAGE_MODE_MAX] = { 0, };
			for( INT32 k = STAGE_UID_NONE + 1; k < STAGE_UID_MAX; k++ )
			{							
				if ( 0 == i3String::Compare( g_pConfig->m_aMapLoad[k].m_strCodeName, xMap.GetAttribVal(0).c_str() ) )
				{
					i32Ord[i32OrdCnt++] = k;
				}
			}

			for( INT32 loop = 0; loop < i32OrdCnt; loop++ )
			{
				for( INT32 k = 0; k < xMap.NumNodes(); k++ )
				{
					XmlNodeWrapper xValues = xMap.GetNode(k);

					if (xValues.GetName() == "KEY_3")
					{
						for (INT32 m = 0; m < xValues.NumNodes(); m++)
						{
							XmlNodeWrapper xMovement = xValues.GetNode(m);

							I3TRACE("Name		: %s\n", xMovement.GetName().c_str());
							I3TRACE("Attribval	: %s\n", xMovement.GetAttribVal(0).c_str());
							I3TRACE("Attribname	: %s\n", xMovement.GetAttribName(0).c_str());

							if (xMovement.GetAttribVal(0) == "Add_FrontMoveSpeed")
							{
								m_fFrontMoveSpeed[i32Ord[loop]] = atof(xMovement.GetAttribVal(m_ui8NationalCode).c_str());
							}
							if (xMovement.GetAttribVal(0) == "Add_JumpRunFrontMoveSpeed")
							{
								m_fJumpRunSpeed[i32Ord[loop]] = atof(xMovement.GetAttribVal(m_ui8NationalCode).c_str());
							}

							if (xMovement.GetAttribVal(0) == "Gravity")
							{
								m_rGravity[i32Ord[loop]] = atof(xMovement.GetAttribVal(m_ui8NationalCode).c_str());
							}

						}

					}
					if ( xValues.GetName() != "VEC3D_2" )	continue;
					
					NxVec3* pvPos = NULL;
					if ( xValues.GetAttribVal(0) == "BombPosition_A" )
					{
						pvPos = &m_vBombPosition_A[i32Ord[loop]];
					}
					if ( xValues.GetAttribVal(0) == "BombPosition_B" )
					{
						pvPos = &m_vBombPosition_B[i32Ord[loop]];
						m_abBombB[i32Ord[loop]] = true;
					}
				
					if ( NULL == pvPos )					continue;

					for( INT32 m = 0; m < xValues.NumAttributes(); m++ )
					{
						if ( xValues.GetAttribName(m) == m_astrColumnName[0] ) pvPos->x = (NxReal)atof(xValues.GetAttribVal(m).c_str());
						if ( xValues.GetAttribName(m) == m_astrColumnName[1] ) pvPos->y = (NxReal)atof(xValues.GetAttribVal(m).c_str());
						if ( xValues.GetAttribName(m) == m_astrColumnName[2] ) pvPos->z = (NxReal)atof(xValues.GetAttribVal(m).c_str());
					}
				}
			}
		}
	}
}
