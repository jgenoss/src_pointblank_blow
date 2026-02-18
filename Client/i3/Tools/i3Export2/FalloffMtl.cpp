#include "stdafx.h"
#include "FalloffMtl.h"
#include "Util.h"

static IParamBlock2 *		s_pParams = NULL;
static IParamBlock2 *		s_pMixCurve = NULL;

static ParamID				s_paramIndexOfRefract;
static ParamID				s_paramColor2;

FalloffMtl::FalloffMtl(void)
{
}

FalloffMtl::~FalloffMtl(void)
{
}

void FalloffMtl::Bind(void)
{
	INT32 i, j;

	for( i = 0; i < NumParamBlocks(); i++)
	{
		IParamBlock2 * pParam = GetParamBlock( i);
		ParamBlockDesc2 * pDesc;

		if( pParam == NULL)
			continue;

		pDesc = pParam->GetDesc();

		if( i3::generic_is_iequal( pDesc->int_name, "parameters") )
		{
			s_pParams = pParam;
		}
		else if( i3::generic_is_iequal( pDesc->int_name, "mix curve") )
		{
			s_pMixCurve = pParam;
		}

		//pParam->ReleaseDesc( );
	}

	// Parameters
	for( j = 0; j < s_pParams->NumParams(); j++)
	{
		ParamDef def = s_pParams->GetParamDef( j);

		if( i3::generic_is_iequal( def.int_name, "ior") )
		{
			s_paramIndexOfRefract = def.ID;
		}
		else if( i3::generic_is_iequal( def.int_name, "color2") )
		{
			s_paramColor2 = def.ID;
		}
	}
}

REAL32 FalloffMtl::GetIndexOfRefract(void)
{
	I3ASSERT( s_pParams != NULL);
	REAL32 value = 1.33f;
	Interval itv;

	itv.Set( 0, 0);

	s_pParams->GetValue( s_paramIndexOfRefract, 0, value, itv);

	return value;
}

void FalloffMtl::GetColor2( COLORREAL * pColor)
{
	I3ASSERT( s_pParams != NULL);
	REAL32 value = 1.33f;
	Interval itv;
	AColor color;

	itv.Set( 0, 0);

	s_pParams->GetValue( s_paramColor2, 0, color, itv);

	pColor->r = color.r;
	pColor->g = color.g;
	pColor->b = color.b;
	pColor->a = color.a;
}

void FalloffMtl::DumpParam(void)
{
	INT32 i, j;

	for( i = 0; i < this->NumParamBlocks(); i++)
	{
		IParamBlock2 * pParam = GetParamBlock( i);
		ParamBlockDesc2 * pDesc;

		if( pParam == NULL)
			continue;

		pDesc = pParam->GetDesc();

		I3TRACE( "Param %d : %s\n", i, pDesc->int_name);

		for( j = 0; j < pParam->NumParams(); j++)
		{
			ParamDef def = pParam->GetParamDef( j);

			I3TRACE( "-------------------\n");
			I3TRACE( "ParamID : %x\n", def.ID);
			I3TRACE( "int_name : %s\n", def.int_name);
		}

		pParam->ReleaseDesc( );
	}

	I3TRACE( "sdfsdf\n");
}
