#if !defined( __I3_PHYSIX_UTIL_H)
#define __I3_PHYSIX_UTIL_H

#if defined ( I3_PHYSX)

#include "i3Collidee.h"
#include "i3PhysixDefine.h"
#include "NxStream.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Cooked mesh converter
//	maked by gurisama. 2007.08.13

const int mesh_type_size = 4;

enum MeshSerialFlag
{
	MSF_MATERIALS		=	(1<<0),
	MSF_FACE_REMAP		=	(1<<1),
	MSF_HARDWARE_MESH	=	(1<<2),
	MSF_8BIT_INDICES	=	(1<<3),
	MSF_16BIT_INDICES	=	(1<<4),
};

enum ModelFlag
{
	OPC_QUANTIZED	= (1<<0),	// Compressed/uncompressed tree
	OPC_NO_LEAF		= (1<<1),	// Leaf/NoLeaf tree
	OPC_SINGLE_NODE	= (1<<2),	// Special case for 1-node models
};

enum CmMeshType
{
	MT_INVALID_MESH		=   0,
	MT_CONVEX_MESH		=   1,
    MT_TRIANGLE_MESH	=   2,
};

class CmMeshData
{
public:

	CmMeshData() {};
	~CmMeshData() {};
	
	unsigned int numVertices;			// Number of vertices.
	unsigned int numTriangles;			// Number of triangles.
	unsigned int pointStrideBytes;		// Offset between vertex points in bytes.
	unsigned int triangleStrideBytes;	// Offset between triangles in bytes.
	void* points;						// Pointer to array of vertex positions.
	void* triangles;					// Pointer to array of triangle inices.
	unsigned int flags;					// Flags bits.

	unsigned int materialIndexStride;	// Otherwise this is the offset between material indices in bytes.
	void* materialIndices;				// Optional pointer to first material index, or NULL.
	unsigned int heightFieldVerticalAxis;    // The mesh may represent either an arbitrary mesh or a height field.
	float heightFieldVerticalExtent;	// If this mesh is a height field, this sets how far 'below ground' the height volume extends.
	float convexEdgeThreshold;			// Parameter allows you to setup a tolerance for the convex edge detector.
};

class UserStream : public NxStream
{
protected:
	UINT32						m_nPos;
	UINT32						m_nSize;
	char *						m_pPointer;

public:
	UserStream()
	{
		m_nPos = 0; m_nSize = 0; m_pPointer = NULL;
	}
	virtual						~UserStream() {};

	NxU32			getPos() { return m_nPos; }
	void			setPos( NxU32 pos ) { m_nPos = pos; }
	void			setBuffer( char * pBuffer, UINT32 nSize ) { m_pPointer = pBuffer; m_nSize = nSize; }

	void			addPos( INT32 nSize ) { m_nPos += nSize; }
	NxU8			ReadByte()				{ NxU8 vel = readByte(); addPos(sizeof(NxU8)); return vel;}
	NxU16			ReadWord()				{ NxU16 vel = readWord(); addPos(sizeof(NxU16)); return vel;}
	NxU32			ReadDword()				{ NxU32 vel = readDword(); addPos(sizeof(NxU32)); return vel;}
	float			ReadFloat()				{ float vel = readFloat(); addPos(sizeof(float)); return vel;}
	double			ReadDouble()			{ double vel = readDouble(); addPos(sizeof(double)); return vel;}
	void			ReadBuffer( void *buf, NxU32 size)			{ readBuffer(buf, size); addPos(size); }

	virtual		bool			advanceStream(NxU32 nbBytes);

	virtual		NxU8			readByte() const;
	virtual		NxU16			readWord() const;
	virtual		NxU32			readDword() const;
	virtual		float			readFloat() const;
	virtual		double			readDouble() const;
	virtual		void			readBuffer(void* buffer, NxU32 size) const;

	virtual		NxStream&		storeByte(NxU8 b);
	virtual		NxStream&		storeWord(NxU16 w);
	virtual		NxStream&		storeDword(NxU32 d);
	virtual		NxStream&		storeFloat(NxReal f);
	virtual		NxStream&		storeDouble(NxF64 f);
	virtual		NxStream&		storeBuffer(const void* buffer, NxU32 size);
};

namespace NXU
{
	I3_INLINE
	void	Copy( NxMat34 * pDest, MATRIX * pSrc)
	{
		VEC3D tmp;

		i3Vector::Normalize( &tmp, i3Matrix::GetRight( pSrc));
		pDest->M.setColumn( 0, *((NxVec3 *) &tmp));

		i3Vector::Normalize( &tmp, i3Matrix::GetUp( pSrc));
		pDest->M.setColumn( 1, *((NxVec3 *) &tmp));

		i3Vector::Normalize( &tmp, i3Matrix::GetAt( pSrc));
		pDest->M.setColumn( 2, *((NxVec3 *) &tmp));

		pDest->t.x = i3Vector::GetX( i3Matrix::GetPos( pSrc));
		pDest->t.y = i3Vector::GetY( i3Matrix::GetPos( pSrc));
		pDest->t.z = i3Vector::GetZ( i3Matrix::GetPos( pSrc));

		/*
		pDest->M.setColumn( 0, *((NxVec3 *) i3Matrix::GetRight( pSrc)));
		pDest->M.setColumn( 1, *((NxVec3 *) i3Matrix::GetUp( pSrc)));
		pDest->M.setColumn( 2, *((NxVec3 *) i3Matrix::GetAt( pSrc)));
		pDest->t.x = i3Vector::GetX( i3Matrix::GetPos( pSrc));
		pDest->t.y = i3Vector::GetY( i3Matrix::GetPos( pSrc));
		pDest->t.z = i3Vector::GetZ( i3Matrix::GetPos( pSrc));
		*/
	}

	I3_INLINE
	void	Copy( MATRIX * pDest, NxMat34 * pSrc)
	{
		NxVec3 vec;

		pSrc->M.getColumn( 0, vec);
		i3Matrix::SetRight( pDest,	vec.x, vec.y, vec.z, 0.0f);

		pSrc->M.getColumn( 1, vec);
		i3Matrix::SetUp( pDest,	vec.x, vec.y, vec.z, 0.0f);

		pSrc->M.getColumn( 2, vec);
		i3Matrix::SetAt( pDest,	vec.x, vec.y, vec.z, 0.0f);

		i3Matrix::SetPos( pDest,	pSrc->t.x, pSrc->t.y, pSrc->t.z, 1.0f);
	}

	I3_EXPORT_SCENE
	const char *		GetTerrainNameByType( INT32 type);

	I3_EXPORT_SCENE
	I3_TERRAIN_TYPE		GetTerrainTypeByName( const char * pszName);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	Cookedmesh Convert

	I3_EXPORT_SCENE
	bool GetCookedData( NxStream &stream, CmMeshType &mesh_type, CmMeshData &mesh_data, bool &hintCollisionSpeed );
};

#endif

#endif
