#include "i3OptPCH.h"
#include "i3OptPSPSwizzledImage.h"
#include "i3Scene.h"

I3_CLASS_INSTANCE( i3OptPSPSwizzledImage);

i3OptPSPSwizzledImage::i3OptPSPSwizzledImage(void)
{
	m_Class = CLASS_TEXTURE;
}

i3Texture *	i3OptPSPSwizzledImage::MakeSwizzledImage( i3Texture * pTex)
{
	UINT32 w, h;
	char * pSrc, * pTemp;
	INT32 level;
//	I3HMEM hMem;

	if( pTex->GetFormat() & I3G_IMAGE_FORMAT_MASK_PSP)
		return pTex;

	if( ((pTex->GetFormat() & I3G_IMAGE_FORMAT_MASK_PLATFORM) != 0) &&
		((pTex->GetFormat() & I3G_IMAGE_FORMAT_MASK_PLATFORM) != I3G_IMAGE_FORMAT_MASK_PSP))
	{
		i3Log( "", "Could not convert image from other native formats rather than PSP.");
		return pTex;
	}

	w = pTex->GetWidth();
	h = pTex->GetHeight();


	//hMem = i3MemAlloc( i3Gfx::CalcRasterBufferSize( w, h, 1, pTex->GetFormat()));
	pTemp = (char *) i3MemAlloc( i3Gfx::CalcRasterBufferSize( w, h, 1, pTex->GetFormat()));

	I3ASSERT(pTemp != nullptr);

	for( level = 0; level < (INT32) pTex->GetLevelCount(); level++)
	{
		pSrc = pTex->Lock( level);

		memcpy(pTemp, pSrc, i3Gfx::CalcRasterBufferSize( w, h, 1, pTex->GetFormat()));

		i3ImageConvertLinearToSwizzled( pSrc, pTemp, i3Gfx::CalcRasterBufferSize( w, 1, 1, pTex->GetFormat()), h, I3_PLATFORM_PSP );

		pTex->Unlock( level);

		w = w >> 1;
		h = h >> 1;
	}

	pTex->SetFormat( (I3G_IMAGE_FORMAT)(pTex->GetFormat() | I3G_IMAGE_FORMAT_MASK_PSP));
	pTex->SetPersistFormat( (I3G_IMAGE_FORMAT)(pTex->GetPersistFormat() | I3G_IMAGE_FORMAT_MASK_PSP));

	//i3mem::Free( hMem);
	i3MemFree(pTemp);

	return pTex;
}

void i3OptPSPSwizzledImage::Trace( i3Node * pRoot)
{
	i3TextureBindAttr * pTexBind;
	i3Texture * pTex;
	i3::vector<i3RenderAttr*> List;

	i3Scene::FindAttrByExactType( pRoot, i3TextureBindAttr::static_meta(), List);

	for(size_t i = 0; i < List.size(); i++)
	{
		pTexBind = (i3TextureBindAttr *) List[i];

		pTex = pTexBind->GetTexture();

		if( pTex != nullptr)
			MakeSwizzledImage( pTex);
	}
}