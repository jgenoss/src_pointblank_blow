#include "i3GfxType.h"
#include "i3TextureCube.h"

//I3_ABSTRACT_CLASS_INSTANCE( i3TextureCube, i3Texture);
I3_CLASS_INSTANCE( i3TextureCube);

i3TextureCube::i3TextureCube(void)
{
	addTextureFlag( I3I_TEXFLAG_CUBE);
}


char * i3TextureCube::Lock( INT32 /*level*/, I3G_LOCKMETHOD /*lock*/, RECT * /*pRect*/)
{
	I3ASSERT_0;

	return nullptr;
}

void i3TextureCube::Unlock( INT32 /*level*/)
{
	I3ASSERT_0;
}