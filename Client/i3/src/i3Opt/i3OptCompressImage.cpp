#include "i3OptPCH.h"
#include "i3OptCompressImage.h"
#include "i3Scene.h"

I3_CLASS_INSTANCE( i3OptCompressImage);

i3OptCompressImage::i3OptCompressImage(void)
{
	m_Class = CLASS_TEXTURE;
}

i3Texture *	i3OptCompressImage::CompressImage( i3Texture * pTex)
{
	pTex->SetPersistFormat( (I3G_IMAGE_FORMAT)(pTex->GetPersistFormat() | I3G_IMAGE_FORMAT_MASK_ZLIB));

	return pTex;
}

i3Texture *	i3OptCompressImage::DecompressImage( i3Texture * pTex)
{
	pTex->SetPersistFormat( (I3G_IMAGE_FORMAT)(pTex->GetPersistFormat() & ~I3G_IMAGE_FORMAT_MASK_ZLIB));

	return pTex;
}

void i3OptCompressImage::Trace( i3Node * pRoot)
{
	i3TextureBindAttr * pTexBind;
	i3Texture * pTex;

	i3::vector<i3RenderAttr*> List;

	i3Scene::FindAttrByExactType( pRoot, i3TextureBindAttr::static_meta(), List);

	for(size_t i = 0; i < List.size(); i++)
	{
		pTexBind = static_cast<i3TextureBindAttr*>(List[i]);

		pTex = pTexBind->GetTexture();

		if( pTex != nullptr)
			CompressImage( pTex);
	}
}
