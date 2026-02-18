#include "i3SceneDef.h"
#include "i3PhysixUtil.h"
#include "NxStream.h"
#include "i3Base/string./compare/generic_is_niequal.h"
#include "i3Base/string/ext/generic_string_size.h"

struct _TERRAIN_NAME_TABLE
{
	char				m_szName[32];
	I3_TERRAIN_TYPE		m_Terrain;
} ;

static _TERRAIN_NAME_TABLE s_TerrainNameTable[] =
{
	{	"CCR_",				I3_TERRAIN_CONCRETE	},
	{	"MET_",				I3_TERRAIN_STEEL	},
	{	"GND_",				I3_TERRAIN_GROUND	},
	{	"WOOD_",			I3_TERRAIN_WOOD	},
	{	"SNOW_",			I3_TERRAIN_SNOW	},
	{	"WTD_",				I3_TERRAIN_WATER_DEEP	},
	{	"WTS_",				I3_TERRAIN_WATER_SHALLOW	},
	{	"CLAY_",			I3_TERRAIN_WET_CLAY	},
	{	"GRASS_",			I3_TERRAIN_GRASS	},
	{	"MARB_",			I3_TERRAIN_MARBLE	},
	{	"LEAF_",			I3_TERRAIN_FALLLEAF	},
	{	"TH_CCR_",			I3_TERRAIN_CONCRETE_THIN	},
	{	"TH_MET_",			I3_TERRAIN_STEEL_THIN	},
	{	"TH_WOOD_",			I3_TERRAIN_WOOD_THIN	},
	{	"TH_MARB_",			I3_TERRAIN_MARBLE_THIN	},
	{	"PAP_",				I3_TERRAIN_PAPER},						// 종이
	{	"GLASS_",			I3_TERRAIN_GLASS},						// 유리
	{	"PLASTIC_",			I3_TERRAIN_PLASTIC},					// 플라스틱
	{	"RUB_",				I3_TERRAIN_RUBBER},						// 고무
	{	"CLOTH_",			I3_TERRAIN_CLOTH},						// 천
	{	"WR_MET_",			I3_TERRAIN_STEEL_WIRE	},				
	{	"WR_WOOD_",			I3_TERRAIN_WOOD_WIRE	},				
	{	"WR_PLASTIC_",		I3_TERRAIN_PLASTIC_WIRE },					
	{	"TH_GND_",			I3_TERRAIN_GROUND_THIN},
	{	"TH_SNOW_",			I3_TERRAIN_SNOW_THIN},
	{	"TH_CLAY_",			I3_TERRAIN_WET_CLAY_THIN},
	{	"TH_GRASS_",		I3_TERRAIN_GRASS_THIN},
	{	"TH_PAP_",			I3_TERRAIN_PAPER_THIN},
	{	"TH_LEAF_",			I3_TERRAIN_FALLLEAF_THIN},
	{	"TH_GLASS_",		I3_TERRAIN_GLASS_THIN},
	{	"TH_PLASTIC_",		I3_TERRAIN_PLASTIC_THIN},
	{	"TH_RUB_",			I3_TERRAIN_RUBBER_THIN},
	{	"TH_CLOTH_",		I3_TERRAIN_CLOTH_THIN},
	{	"TH_BARBEDWIRE_",	I3_TERRAIN_BARBEDWIRE},
	{	"BLOOD_",			I3_TERRAIN_BLOOD},
	{	"TEMP1_",			I3_TERRAIN_TEMP1},
	{	"TEMP2_",			I3_TERRAIN_TEMP2},
	{	"ND_CLOTH_",		I3_TERRAIN_CLOTH_NO_DECAL},
	{	"NONE_",			I3_TERRAIN_NONE },
	{	"",					I3_TERRAIN_NA}
};

const char *		NXU::GetTerrainNameByType( INT32 type)
{
	return (const char *) s_TerrainNameTable[ type].m_szName;
}

I3_TERRAIN_TYPE		NXU::GetTerrainTypeByName( const char * pszName)
{
	INT32 i;
	char * pszTerrainName;
	INT32 len;

	for( i = 0; s_TerrainNameTable[i].m_szName[0] != 0; i++)
	{
		pszTerrainName = s_TerrainNameTable[i].m_szName;
		len = i3::generic_string_size( pszTerrainName);

//		if( i3String::NCompare( pszTerrainName, pszName, len) == 0)
		if (i3::generic_is_niequal( pszTerrainName, pszName, len) )
		{
			return s_TerrainNameTable[i].m_Terrain;
		}
	}

	return I3_TERRAIN_NA;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool UserStream::advanceStream(NxU32 nbBytes)
{
	m_nPos += nbBytes;
	return true;
}

NxU8 UserStream::readByte() const
{
	NxU8  Vel = (NxU8)(m_pPointer[m_nPos]);
	return (Vel);
}
NxU16 UserStream::readWord() const
{
	NxU16 vel = (NxU16) m_pPointer[m_nPos];
	return vel;
}

NxU32 UserStream::readDword() const
{
	NxU32 vel = (NxU32) m_pPointer[m_nPos];
	return vel;
}
float UserStream::readFloat() const
{
	float vel = (float) m_pPointer[m_nPos];
	return vel;
}
double UserStream::readDouble() const
{
	double vel = (double) m_pPointer[m_nPos];
	return vel;
}
void UserStream::readBuffer(void* buffer, NxU32 size) const
{
	i3mem::Copy( buffer, &m_pPointer[m_nPos], size);
}

NxStream& UserStream::storeByte(NxU8 b)
{
	*((NxU8*)&m_pPointer[m_nPos]) = b;
	m_nPos += sizeof(NxU8);
	return *this;
}
NxStream& UserStream::storeWord(NxU16 w)
{
	*((NxU16*)&m_pPointer[m_nPos]) = w;
	m_nPos += sizeof(NxU16);
	return *this;
}
NxStream& UserStream::storeDword(NxU32 d)
{
	*((NxU32*)&m_pPointer[m_nPos]) = d;
	m_nPos += sizeof(NxU32);
	return *this;
}
NxStream& UserStream::storeFloat(NxReal f)
{
	*((NxReal*)&m_pPointer[m_nPos]) = f;
	m_nPos += sizeof(NxReal);
	return *this;
}
NxStream& UserStream::storeDouble(NxF64 f)
{
	*((NxF64*)&m_pPointer[m_nPos]) = f;
	m_nPos += sizeof(NxF64);
	return *this;
}
NxStream& UserStream::storeBuffer(const void* buffer, NxU32 size)
{
	i3mem::Copy( &m_pPointer[m_nPos], (void*)buffer, size );
	m_nPos += size;
	return *this;
}

static NxU16 flip(const NxU16* v)
{
	const NxU8* b = (const NxU8*)v;
	NxU16 f;
	NxU8* bf = (NxU8*)&f;
	bf[0] = b[1];
	bf[1] = b[0];
	return f;
}

static NxU32 flip(const NxU32* v)
{
	const NxU8* b = (const NxU8*)v;
	NxU32 f;
	NxU8* bf = (NxU8*)&f;
	bf[0] = b[3];
	bf[1] = b[2];
	bf[2] = b[1];
	bf[3] = b[0];
	return f;
}

static NxF32 flip(const NxF32* v)
{
	NxU32 d = flip((const NxU32*)v);
	return ((NxF32&)(d));
}

NxU16 ReadWord(bool mismatch, NxStream &stream )
{
	NxU16 d;
	UserStream * userStream = (UserStream*)&stream;
	d = userStream->ReadWord();
	if(mismatch)	d = flip(&d);
	return d;
}

NxU32 ReadDword(bool mismatch, NxStream &stream )
{
	NxU32 d;
	UserStream * userStream = (UserStream*)&stream;
	d = userStream->ReadDword();
	if(mismatch)	d = flip(&d);
	return d;
}

NxF32 ReadFloat(bool mismatch, NxStream &stream )
{
	NxU32 d;
	UserStream * userStream = (UserStream*)&stream;
	d = userStream->ReadDword();
	if(mismatch)	d = flip(&d);
	return (NxF32 &)(d);
}

void writeWord(NxU16 value, bool mismatch, NxStream& stream)
{
	if(mismatch)	value = flip(&value);
	stream.storeWord(value);
}

void writeDword(NxU32 value, bool mismatch, NxStream& stream)
{
	if(mismatch)	value = flip(&value);
	stream.storeDword(value);
}

void writeFloat(NxF32 value, bool mismatch, NxStream& stream)
{
	if(mismatch)	value = flip(&value);
	stream.storeFloat(value);
}

void skipIndices( NxStream &stream, NxU32 maxIdx, NxU32 nbIndices, bool max16Bit)
{
	NxU32 i;
	UserStream * userStream = (UserStream*)&stream;

	if(maxIdx <= 0xff)
	{
		for( i = 0; i < nbIndices; i++ )
		{
			userStream->ReadByte();
		}
	}
	else if ((maxIdx <= 0xffff) || (max16Bit))
	{
		for( i = 0; i < nbIndices; i++ )
			userStream->ReadWord();
	}
	else
	{
		for( i = 0; i < nbIndices; i++ )
			userStream->ReadDword();
	}
}

bool readFloatBuffer( NxF32* dest, NxU32 nbFloats, bool mismatch, NxStream &stream )
{
	UserStream * userStream = (UserStream*)&stream;

	userStream->ReadBuffer(dest, sizeof(NxF32)*nbFloats);
	if(mismatch)
	{
		for( NxU32 i=0; i < nbFloats;i++ )
		{
			dest[i] = flip(&dest[i]);
		}
	}
	return true;
}

bool readIntBuffer( NxU32* dest, NxU32 nbInts, bool mismatch, NxStream &stream )
{
	return readFloatBuffer( (NxF32*)dest, nbInts, mismatch, stream );
}

void readIndices( NxU32 maxIndex, NxU32 nbIndices, NxU32* indices, NxStream &stream, bool platformMismatch)
{
	UserStream * userStream = (UserStream*)&stream;

	if(maxIndex<=0xff)
	{
		for(NxU32 i=0;i<nbIndices;i++)
		{
			indices[i] = userStream->ReadByte();
		}
	}
	else if(maxIndex<=0xffff)
	{
		for(NxU32 i=0;i<nbIndices;i++)
		{
			indices[i] = ReadWord(platformMismatch, stream);
		}
	}
	else
	{
		readIntBuffer( indices, nbIndices, platformMismatch, stream);
	}
}

void readIndices(NxU32 maxIndex, NxU32 nbIndices, NxU16* indices, NxStream &stream, bool platformMismatch)
{
	UserStream * userStream = (UserStream*)&stream;

	if(maxIndex<=0xff)
	{
		for(NxU32 i=0;i<nbIndices;i++)
		{
			indices[i] = userStream->ReadByte();
		}
	}
	else
	{
		for(NxU32 i=0;i<nbIndices;i++)
		{
			indices[i] = ReadWord(platformMismatch, stream);
		}
	}
}


bool hasQuantizedBox( NxStream &stream, bool mismatch, bool& hintCollisionSpeed)
{
	UserStream * userStream = (UserStream*)&stream;

	hintCollisionSpeed = false;
//	NxU32 modelSize = ReadDword(mismatch, stream);

	// Import header
	NxU8 hdr[4];
	userStream->ReadBuffer( hdr, sizeof(NxU8)*4);
	if(hdr[0]!='O' || hdr[1]!='P' || hdr[2]!='C')
		return false;

	bool mod_mismatch = (hdr[3] != 1);

//	NxU32 mod_version	= ReadDword(mod_mismatch, stream);
	NxU32 modelCode		= ReadDword(mod_mismatch, stream);

	hintCollisionSpeed = (modelCode & OPC_QUANTIZED) != OPC_QUANTIZED;

	return true;
}

void saveChunk(NxI8 a, NxI8 b, NxI8 c, NxI8 d, NxStream &stream)
{
	stream.storeByte(a);
	stream.storeByte(b);
	stream.storeByte(c);
	stream.storeByte(d);
}

bool writeHeader(NxI8 a, NxI8 b, NxI8 c, NxI8 d, NxU32 version, bool mismatch, NxStream &stream )
{
	// Store endianness
	NxI8 streamFlags = 1;
	if(mismatch)	streamFlags^=1;

	// Export header
	saveChunk('N', 'X', 'S', streamFlags, stream);	// "Novodex stream" identifier
	saveChunk(a, b, c, d, stream);					// Chunk identifier
//	stream.storeDword(version);						// Version number
	writeDword(version, mismatch, stream);

	return true;
}

void readChunk(NxI8& a, NxI8& b, NxI8& c, NxI8& d, NxStream &stream )
{
	UserStream * userStream = (UserStream*)&stream;

	a = userStream->ReadByte();
	b = userStream->ReadByte();
	c = userStream->ReadByte();
	d = userStream->ReadByte();
}

bool readHeader(NxI8 hdr1, NxI8 hdr2, NxI8 hdr3, NxI8& a, NxI8& b, NxI8& c, NxI8& d, NxU32& version, bool& mismatch, NxStream &stream )
{
	// Import header
	NxI8 h1, h2, h3, h4;
	readChunk(h1, h2, h3, h4, stream);
	if(h1!=hdr1 || h2!=hdr2 || h3!=hdr3)
		return false;

	NxI8 fileLittleEndian = h4&1;
	mismatch = fileLittleEndian!= 1;

	readChunk(a, b, c, d, stream);

	version = ReadDword(mismatch, stream);
	return true;
}

bool getTriMeshData( NxStream &stream, NxU32 version, const bool mismatch, bool& hintCollisionSpeed, CmMeshData& meshDesc)
{
	UserStream* userStream = (UserStream*)&stream;
	// Import serialization flags
	NxU32 serialFlags = ReadDword(mismatch, stream);
	
	if (serialFlags & MSF_HARDWARE_MESH)
		meshDesc.flags |= NX_MF_HARDWARE_MESH;

	// Import misc values
	meshDesc.convexEdgeThreshold		= ReadFloat(mismatch, stream);
	meshDesc.heightFieldVerticalAxis	= (NxHeightFieldAxis)ReadDword(mismatch, stream);
	meshDesc.heightFieldVerticalExtent	= ReadFloat(mismatch, stream);

	// Import mesh
	meshDesc.pointStrideBytes	= sizeof(NxPoint);
    meshDesc.numVertices		= ReadDword(mismatch, stream);
	I3ASSERT( meshDesc.numVertices > 0);
    NxPoint* points = new NxPoint[meshDesc.numVertices];
	NetworkDump_Malloc( points, sizeof(NxPoint) * meshDesc.numVertices, __FILE__, __LINE__);

	meshDesc.triangleStrideBytes = sizeof(NxTriangle32);
	meshDesc.numTriangles		= ReadDword(mismatch, stream);
	I3ASSERT( meshDesc.numTriangles > 0 );
	NxTriangle32* tris = new NxTriangle32[meshDesc.numTriangles];
	NetworkDump_Malloc( tris, sizeof(NxTriangle32) * meshDesc.numTriangles, __FILE__, __LINE__);

	// Get vertices
	userStream->ReadBuffer( points, sizeof(NxPoint)*meshDesc.numVertices );
    if(mismatch)
	{
		for(NxU32 i=0; i < meshDesc.numVertices; i++)
		{
			points[i].x = flip(&points[i].x);
			points[i].y = flip(&points[i].y);
			points[i].z = flip(&points[i].z);
		}
	}
	meshDesc.points = points;

	// Get triangles
	if(serialFlags & MSF_8BIT_INDICES)
	{
		for(NxU32 i=0; i < meshDesc.numTriangles; i++)
		{
			tris[i].v[0] = userStream->ReadByte();
			tris[i].v[1] = userStream->ReadByte();
			tris[i].v[2] = userStream->ReadByte();
		}
	}
	else if(serialFlags & MSF_16BIT_INDICES)
	{
		for(NxU32 i=0; i < meshDesc.numTriangles; i++)
		{
			NxU16 i0 = userStream->ReadWord();
			NxU16 i1 = userStream->ReadWord();
			NxU16 i2 = userStream->ReadWord();

			if(mismatch)
			{
				i0 = flip(&i0);
				i1 = flip(&i1);
				i2 = flip(&i2);
			}
			tris[i].v[0] = i0;
			tris[i].v[1] = i1;
			tris[i].v[2] = i2;
		}
	}
	else
	{	
		userStream->ReadBuffer( tris, sizeof(NxTriangle32)*meshDesc.numTriangles);
		if(mismatch)
		{
			for(NxU32 i=0; i < meshDesc.numTriangles; i++)
			{
				tris[i].v[0] = flip(&tris[i].v[0]);
				tris[i].v[1] = flip(&tris[i].v[1]);
				tris[i].v[2] = flip(&tris[i].v[2]);
			}
		}
	}
	meshDesc.triangles = tris;


	// Get materials
	if(serialFlags & MSF_MATERIALS)
	{
		NxMaterialIndex* materials = new NxMaterialIndex[meshDesc.numTriangles];
		NetworkDump_Malloc( materials, sizeof( NxMaterialIndex) * meshDesc.numTriangles, __FILE__, __LINE__);
		userStream->ReadBuffer( materials, sizeof(NxMaterialIndex)*meshDesc.numTriangles);

		if(mismatch)
		{
			for(NxU32 i=0; i < meshDesc.numTriangles; i++)
				materials[i] = flip(&materials[i]);
		}

		meshDesc.materialIndices = materials;
		meshDesc.materialIndexStride = sizeof(NxMaterialIndex);
	}


	bool convOK = false;
	//UINT32 offset = userStream->getPos();
	
	for( INT32 j=0; j <= 1; j++)
	{
		/*if (j > 0)
		{
			// Reset file pointer and try to load the data as a 2.3.x SDK mesh
			//int ret_val = fseek(userStream->fp, offset, SEEK_SET);
			INT32 ret_val = pStream->SetPosition( offset, STREAM_BEGIN );
			I3ASSERT( ret_val == 0 );
		}
		*/

		if(serialFlags & MSF_FACE_REMAP)
		{
			NxU32* remap = new NxU32[meshDesc.numTriangles];
			NetworkDump_Malloc( remap, sizeof(NxU32) * meshDesc.numTriangles, __FILE__, __LINE__);
	
			if (j == 0)
			{
				readIndices(ReadDword(mismatch, stream), meshDesc.numTriangles, remap, stream, mismatch);
			}
			else
			{
				// 2.3.x SDK
				readIntBuffer( remap, meshDesc.numTriangles, mismatch, stream );
			}
			I3_MUST_DELETE_ARRAY( remap);
		}

	#ifdef SUPPORT_CONVEX_PARTS
		NxU32 numConvexParts = ReadDword(mismatch, stream);
		NxU32 numFlatParts = ReadDword(mismatch, stream);

		if(numConvexParts)
		{
			NxU16* convexParts = new NxU16[meshDesc.numTriangles];
			userStream->ReadBuffer(convexParts, sizeof(NxU16)*meshDesc.numTriangles);
			if(mismatch)
			{
				for(NxU32 i=0; i < meshDesc.numTriangles; i++)
					convexParts[i] = flip(&convexParts[i]);
			}

			delete [] convexParts;
		}
		if(numFlatParts)
		{
			if(numFlatParts < 256)
			{
				NxU8* flatParts8 = new NxU8[meshDesc.numTriangles];
				userStream->ReadBuffer(flatParts8, sizeof(NxU8)*meshDesc.numTriangles);

				delete [] flatParts8;
			}
			else
			{
				NxU16* flatParts16 = new NxU16[meshDesc.numTriangles];
				userStream->ReadBuffer(flatParts16, sizeof(NxU16)*meshDesc.numTriangles);

				delete [] flatParts16;
			}
	//		if(mismatch)
	//			{
	//			for(NxU32 i=0;i<mesh.getNumTriangles();i++)
	//				mesh._flatParts[i] = flip(&mesh._flatParts[i]);
	//			}
		}
	#endif

		// Import tree model flag
		convOK = hasQuantizedBox( stream, mismatch, hintCollisionSpeed);

		if (convOK)
			break;
	}

	return convOK;
}


bool getConvMeshData( NxStream &stream, NxU32 version, const bool mismatch, NxU16& useUnquantizedNormals, bool& hintCollisionSpeed, CmMeshData& meshDesc)
{
	UserStream* userStream = (UserStream*)&stream;
	// Import serialization flags
	NxU32 serialFlags = ReadDword(mismatch, stream);

	useUnquantizedNormals = 0;


	if (version > 0)
	{
		// Import collision hull header
		NxI8	collHullType[mesh_type_size];
		NxU32	collHullVersion;
		bool	collHullMismatch;


		bool validFile = readHeader('I', 'C', 'E', collHullType[0], collHullType[1], collHullType[2], collHullType[3],
						 collHullVersion, collHullMismatch, stream);
		if (!validFile || !((collHullType[0] == 'C') && (collHullType[1] == 'L') && (collHullType[2] == 'H') && (collHullType[3] == 'L')))
		{
			printf("Error: Invalid collision hull header.\n");
			return false;
		}

		// Import collision hull header
		NxI8	convHullType[mesh_type_size];
		NxU32	convHullVersion;
		bool	convHullMismatch;

		validFile = readHeader('I', 'C', 'E', convHullType[0], convHullType[1], convHullType[2], convHullType[3],
					convHullVersion, convHullMismatch, stream);
		if (!validFile || !((convHullType[0] == 'C') && (convHullType[1] == 'V') && (convHullType[2] == 'H') && (convHullType[3] == 'L')))
		{
			printf("Error: Invalid convex hull header.\n");
			return false;
		}

		// Import figures
		meshDesc.pointStrideBytes = sizeof(NxPoint);
		meshDesc.numVertices = ReadDword(convHullMismatch, stream);
		I3ASSERT( meshDesc.numVertices > 0);
		NxPoint* points  = new NxPoint[meshDesc.numVertices];
		NetworkDump_Malloc( points, sizeof( NxPoint) * meshDesc.numVertices, __FILE__, __LINE__);

		meshDesc.triangleStrideBytes = sizeof(NxTriangle32);
		meshDesc.numTriangles = ReadDword(convHullMismatch, stream);
		I3ASSERT( meshDesc.numTriangles > 0);
		NxTriangle32* tris = new NxTriangle32[meshDesc.numTriangles];
		NetworkDump_Malloc( tris, sizeof( NxTriangle32) * meshDesc.numTriangles, __FILE__, __LINE__);

		NxU32 nbEdges			= ReadDword(convHullMismatch, stream);
		NxU32 nbPolygons		= ReadDword(convHullMismatch, stream);
		const NxU32 polyDataSize = 4 + 24 + 2*sizeof(size_t);  // sizeof(PxHullPolygonData)
								// WARNING: 64bit portabilitty issue. There are two pointers in this structure
								// the size of which are assumed to match the size on the platform they were saved on,
								// i.e., sizeof(size_t) might have to be changed if this code is compiled on a
								// 64Bit system

		NxU32 nb				= ReadDword(convHullMismatch, stream);
		NxU32 lastOffset		= ReadDword(convHullMismatch, stream);

		// Import vertices
		readFloatBuffer((NxF32*) points, 3*meshDesc.numVertices, convHullMismatch, stream);
		meshDesc.points = points;

		// Import triangles
		NxU32 maxIdx = ReadDword(convHullMismatch, stream);
		readIndices(maxIdx, 3*meshDesc.numTriangles, (NxU32*) tris, stream, convHullMismatch);
		meshDesc.triangles = tris;

		// Import hull normals
		NxPoint* normals = new NxPoint[meshDesc.numVertices];
		NetworkDump_Malloc( normals, sizeof( NxPoint) * meshDesc.numVertices, __FILE__, __LINE__);
		if(convHullVersion >= 5)
		{
			useUnquantizedNormals = ReadWord(convHullMismatch, stream);
		}

		// Import hull normals
		if((convHullVersion >= 4) && !useUnquantizedNormals)
		{
			for(NxU32 i=0; i < meshDesc.numVertices; i++)
			{
				ReadWord(convHullMismatch, stream);
			}
		}
		else
		{
			readFloatBuffer((NxF32*) normals, 3*meshDesc.numVertices, convHullMismatch, stream);
			if(convHullVersion < 2)
			{
				for(NxU32 i=0; i < meshDesc.numVertices; i++)	
				{
					normals[i].x = -normals[i].x;
					normals[i].y = -normals[i].y;
					normals[i].z = -normals[i].z;
				}
			}
		}
		I3_MUST_DELETE_ARRAY( normals);

		// Import hull center
		ReadFloat(convHullMismatch, stream);
		ReadFloat(convHullMismatch, stream);
		ReadFloat(convHullMismatch, stream);

		// Import polygons
		userStream->advanceStream(nbPolygons * polyDataSize);
		/*stream.ReadBuffer(mData.mPolygons, mData.mNbPolygons*sizeof(PxHullPolygonData));
		if(Mismatch)
		{
			for(udword i=0;i<mData.mNbPolygons;i++)
			{
				Flip(mData.mPolygons[i].mNbVerts);
				Flip(mData.mPolygons[i].mFlags);
				Flip((void *&)mData.mPolygons[i].mERef16);
  				Flip((void *&)mData.mPolygons[i].mVRef8);
				Flip(mData.mPolygons[i].mPlane[0]);
				Flip(mData.mPolygons[i].mPlane[1]);
				Flip(mData.mPolygons[i].mPlane[2]);
				Flip(mData.mPolygons[i].mPlane[3]);
				Flip(mData.mPolygons[i].mMin);
				Flip(mData.mPolygons[i].mMax);
			}
		}*/

		if(convHullVersion >= 3)
		{
			userStream->advanceStream(nb);
			maxIdx = ReadDword(convHullMismatch, stream);
			
			skipIndices( stream, maxIdx, nb, true);
			//for(udword i=0;i<Nb;i++)	mData.mVertexData8[i] = stream.ReadByte();
			//ReadIndices(ReadDword(Mismatch, stream), Nb, mData.mEdgeData16, stream, Mismatch);
		}

		ReadDword(convHullMismatch, stream);
		ReadDword(convHullMismatch, stream);
		
		// Import edges
		if(convHullVersion >= 3)
		{
			userStream->advanceStream(2* nbEdges);
			/*for(udword i=0;i<mData.mNbEdges;i++)
			{
				mData.mEdges8[i].Ref0 = stream.ReadByte();
				mData.mEdges8[i].Ref1 = stream.ReadByte();
			}*/
		}

		if((convHullVersion >= 4) && !useUnquantizedNormals)
		{
			userStream->advanceStream(nbEdges * sizeof(NxU16));
			/*for(udword i=0;i<mData.mNbEdges;i++)
				DecodeNormal(mData.mEdgeNormals[i], ReadWord(Mismatch, stream));*/
		}
		else
		{
			userStream->advanceStream(nbEdges * sizeof(NxPoint));
			//ReadFloatBuffer(&mData.mEdgeNormals->x, 3*mData.mNbEdges, Mismatch, stream);
		}

		{
			maxIdx = ReadDword(convHullMismatch, stream);
			
			skipIndices( stream, maxIdx, nbEdges, true);

			maxIdx = ReadDword(convHullMismatch, stream);
			
			skipIndices( stream, maxIdx, nbEdges, true);
		}
		{
			maxIdx = ReadDword(convHullMismatch, stream);
			
			skipIndices( stream, maxIdx, nbEdges, false);
		}
		
		userStream->advanceStream(lastOffset);
		
		// Import valency header
		NxI8	valencyType[mesh_type_size];
		NxU32	valencyVersion;
		bool	valencyMismatch;

		validFile = readHeader('I', 'C', 'E', valencyType[0], valencyType[1], valencyType[2], valencyType[3],
					valencyVersion, valencyMismatch, stream);
		if (!validFile || !((valencyType[0] == 'V') && (valencyType[1] == 'A') && (valencyType[2] == 'L') && (valencyType[3] == 'E')))
		{
			printf("Error: Invalid valency header.\n");
			return false;
		}

		NxU32 valNbVerts = ReadDword(valencyMismatch, stream);
		NxU32 nbAdjVerts = ReadDword(valencyMismatch, stream);

		I3ASSERT(valencyVersion == 2);

		{
			maxIdx = ReadDword(valencyMismatch, stream);
			skipIndices( stream, maxIdx, valNbVerts, true);
		}
		userStream->advanceStream( nbAdjVerts );
	}
	else
	{
		// 2.3.x SDK

		// Import figures
		meshDesc.pointStrideBytes = sizeof(NxPoint);
		meshDesc.numVertices = ReadDword(mismatch, stream);
		I3ASSERT(meshDesc.numVertices);
		NxPoint* points  = new NxPoint[meshDesc.numVertices];
		NetworkDump_Malloc( points, sizeof( NxPoint) * meshDesc.numVertices, __FILE__, __LINE__);

		meshDesc.triangleStrideBytes = sizeof(NxTriangle32);
		meshDesc.numTriangles = ReadDword(mismatch, stream);
		I3ASSERT(meshDesc.numTriangles);
		NxTriangle32* tris = new NxTriangle32[meshDesc.numTriangles];
		NetworkDump_Malloc( tris, sizeof( NxTriangle32) * meshDesc.numTriangles, __FILE__, __LINE__);

		// Import vertices
		readFloatBuffer((NxF32*) points, 3*meshDesc.numVertices, mismatch, stream);
		meshDesc.points = points;

		// Import triangles
		readIntBuffer((NxU32*) tris, 3*meshDesc.numTriangles, mismatch, stream);
		meshDesc.triangles = tris;

		if(serialFlags & MSF_MATERIALS)
		{
			userStream->advanceStream( sizeof(NxMaterialIndex) * meshDesc.numTriangles);
		}

		if(serialFlags & MSF_FACE_REMAP)
		{
			userStream->advanceStream( sizeof(NxU32) * meshDesc.numTriangles );
		}
	}


	// Import tree model flag
	if (!hasQuantizedBox(stream, mismatch, hintCollisionSpeed))
		return false;

	return true;
}


bool NXU::GetCookedData( NxStream &stream, CmMeshType &mesh_type, CmMeshData &meshDesc, bool &hintCollisionSpeed)
{
	mesh_type = MT_INVALID_MESH;
	hintCollisionSpeed = false;

	NxI8	head;
	NxI8	meshType[mesh_type_size];		// Mesh type ID
	NxU32	version;
	bool	mismatch;
	bool	check = true;

   	// Import header
	UserStream * userStream = (UserStream*) &stream;

	while( check )
	{
		head = userStream->ReadByte();

		if( head == 'N' )
		{
			head = userStream->ReadByte();
			if( head == 'X' )
			{
				head = userStream->ReadByte();
				if( head == 'S' )
				{
					userStream->addPos( -3);
					check = false;
				}
			}
		}
	}
	
	
	bool validFile = readHeader('N', 'X', 'S', meshType[0], meshType[1], meshType[2], meshType[3], version, mismatch, stream);

    if (!validFile)
    {
		I3TRACE("Error: Invalid header in file.\n" );
        return false;
    }

    if ((meshType[0] == 'M') && (meshType[1] == 'E') && (meshType[2] == 'S') && (meshType[3] == 'H'))
    {
        // general mesh
		mesh_type = MT_TRIANGLE_MESH;

		// Get old mesh
		if (getTriMeshData( stream, version, mismatch, hintCollisionSpeed, meshDesc))
		{
			return true;
		}
		else
		{
			I3TRACE("Error: Loading cooked mesh file failed.\n");
			return false;
		}
    }
    else if ((meshType[0] == 'C') && (meshType[1] == 'V') && (meshType[2] == 'X') && (meshType[3] == 'M'))
    {
        // convex mesh
		mesh_type = MT_CONVEX_MESH;

		// Get old mesh
		NxU16 useUnquantizedNormals;
        if (getConvMeshData( stream, version, mismatch, useUnquantizedNormals, hintCollisionSpeed, meshDesc))
		{
			if (useUnquantizedNormals)
				meshDesc.flags |= NX_CF_USE_UNCOMPRESSED_NORMALS;

			return true;
		}
		else
		{
			I3TRACE("Error: Loading cooked mesh file failed.\n" );
			return false;
		}
    }
    else
    {
        I3TRACE("Error: Unsupported mesh type in file.\nOnly meshes and convex meshes are supported.\n");
        return false;
    }
}
