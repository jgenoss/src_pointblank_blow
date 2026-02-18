#include "i3GfxType.h"
#include "i3VertexFormat.h"

I3_CLASS_INSTANCE( i3VertexFormat);

i3VertexFormat::i3VertexFormat( bool bHasPosition, INT8 nTextureCoordCount )
{
	// Set default values
	SetHasPosition( bHasPosition );
	if( nTextureCoordCount > 0 )
	{
		SetTextureCoordCount( nTextureCoordCount );
	}

#if defined( I3G_OGLES)
	
	SetPositionType( I3VF_TYPE_FIXED32_16);
	SetNormalType( I3VF_TYPE_FIXED32_16);
	SetTangentType( I3VF_TYPE_FIXED32_16);
	SetColorType( I3VF_TYPE_COLOR32_RGBA_8888);
	SetTextureCoordType( I3VF_TYPE_FIXED32_16);
	SetBlendWeightType( I3VF_TYPE_FIXED32_16);
	SetTangentType( I3VF_TYPE_FIXED32_16);
	SetBinormalType( I3VF_TYPE_FIXED32_16);
#else
	SetPositionType( I3VF_TYPE_REAL32);
	SetNormalType( I3VF_TYPE_REAL32);
	SetTangentType( I3VF_TYPE_REAL32);
	SetColorType( I3VF_TYPE_COLOR32_RGBA_8888);
	SetTextureCoordType( I3VF_TYPE_REAL32);
	SetBlendWeightType( I3VF_TYPE_REAL32);
	SetTangentType( I3VF_TYPE_FIXED32_16);
	SetBinormalType( I3VF_TYPE_FIXED32_16);
#endif
}

