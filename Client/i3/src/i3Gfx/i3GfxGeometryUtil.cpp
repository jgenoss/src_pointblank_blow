#include "i3GfxType.h"
#include "i3Math.h"
#include "i3GfxGeometryUtil.h"
#include "i3VertexArray.h"

i3VertexArray * i3GfxGeometryUtil::CreateRectangle( REAL32 Width, REAL32 Height, REAL32 Z,
							bool bUV,
							bool bNormal,
							COLORREAL * pColor1, COLORREAL * pColor2,
							COLORREAL * pColor3, COLORREAL * pColor4)
{
	i3VertexFormat Format;
	i3VertexArray * pVA;
	REAL32 MidX, MidY;

	//
	// Vertex Format
	//
	{
		Format.SetHasPosition( true);

		if( pColor1 == nullptr)
			Format.SetHasColor( false);
		else
			Format.SetHasColor( true);

		if( bUV)
		{
			Format.SetTextureCoordSetCount( 1);
			Format.SetTextureCoordCount( 2);
		}
		else
		{
			Format.SetTextureCoordSetCount( 0);
		}

		Format.SetHasNormal( bNormal);
	}

	pVA = i3VertexArray::new_object();

	pVA->Create( &Format, 6, 0);

	pVA->Lock( 0);

	MidX = Width * 0.5f;
	MidY = Height * 0.5f;

	// Position
	{
		VEC3D vec;

		i3Vector::Set( &vec, -MidX, -MidY, Z);
		pVA->SetPosition( 0, &vec);

		i3Vector::Set( &vec, MidX, -MidY, Z);
		pVA->SetPosition( 1, &vec);

		i3Vector::Set( &vec, -MidX, MidY, Z);
		pVA->SetPosition( 2, &vec);

		pVA->SetPosition( 3, &vec);

		i3Vector::Set( &vec, MidX, -MidY, Z);
		pVA->SetPosition( 4, &vec);

		i3Vector::Set( &vec, MidX, MidY, Z);
		pVA->SetPosition( 5, &vec);
	}

	// Vertex Color
	if( Format.GetHasColor())
	{
		if (pColor1 == nullptr || pColor2 == nullptr || pColor3 == nullptr || pColor4 == nullptr)
		{
			I3ASSERT(pColor1 != nullptr);
			I3ASSERT(pColor2 != nullptr);
			I3ASSERT(pColor3 != nullptr);
			I3ASSERT(pColor4 != nullptr);
		}
		else
		{
			pVA->SetColor(0, pColor1);
			pVA->SetColor(1, pColor3);
			pVA->SetColor(2, pColor2);
			pVA->SetColor(3, pColor2);
			pVA->SetColor(4, pColor3);
			pVA->SetColor(5, pColor4);
		}
	}

	// Texture Coords
	if( bUV)
	{
		VEC2D vec;

		i3Vector::Set( &vec, 0.0f, 1.0f);
		pVA->SetTextureCoord( 0, 0, &vec);

		i3Vector::Set( &vec, 1.0f, 1.0f);
		pVA->SetTextureCoord( 0, 1, &vec);

		i3Vector::Set( &vec, 0.0f, 0.0f);
		pVA->SetTextureCoord( 0, 2, &vec);

		pVA->SetTextureCoord( 0, 3, &vec);

		i3Vector::Set( &vec, 1.0f, 1.0f);
		pVA->SetTextureCoord( 0, 4, &vec);

		i3Vector::Set( &vec, 1.0f, 0.0f);
		pVA->SetTextureCoord( 0, 5, &vec);
	}

	// Normal
	if( bNormal)
	{
		VEC3D vec = { 0.0f, 0.0f, -1.0f };
		
		for(INT32 i = 0; i < 6; i++)
		{
			pVA->SetNormal( i, &vec);
		}
	}

	pVA->Unlock();

	return pVA;
}
