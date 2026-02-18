#include "i3CommonType.h"
#include "i3Memory.h"
#include "i3CompressHuffman.h"
#include "i3Debug.h"
#include "i3Stream.h"
#include "i3StreamFile.h"

#include <stdlib.h>

typedef UINT8  HM_DATA;
typedef UINT16 ID_DATA;

typedef struct _tagi3HuffmanFileHeader
{
	UINT32  m_HeaderSize;
	UINT32  m_FlagSize;
	UINT32  m_EncSize;
	UINT32  m_EncBitSize;
	UINT32  m_UnitDecSize;
	UINT32  m_AccessTblCount;
	UINT32  m_AccessTblSize;
	UINT32  m_OriginalSize;
} I3_HUFFMAN_HEADER;

typedef struct 
{
	HM_DATA		m_Data;
	UINT32		m_Count;
} DATA_COUNT;

typedef struct 
{
	UINT32			size;
	UINT32			enc_size;
	UINT32			num;
	DATA_COUNT *	count;
	UINT32			tree_count;
	UINT16			root_id;
} DATA_INFO;

typedef struct _TREE 
{
	struct _TREE *next_tree;
	struct _TREE *prev_right_tree;
	struct _TREE *prev_left_tree;
	ID_DATA tree_id;
	ID_DATA tree_right_id;
	ID_DATA tree_left_id;
	UINT8 bit;
} TREE;

typedef struct _TOP_TREE 
{
	TREE *tree_link;
} TOP_TREE;

typedef struct 
{
	UINT32 count;
	TREE  *current_tree;
} SORT_TABLE;

typedef struct 
{
	HM_DATA data;
	UINT32   flag;
	UINT8  flag_width;
} FLAG_TABLE;

typedef struct 
{
	ID_DATA tree_id;
	ID_DATA tree_branch[2];
} TREE_TABLE;

typedef struct 
{
	TREE_TABLE    *tree_table;
	UINT8 *access_table;
	UINT8 *data;
} DECODE_TABLE;

static UINT32 s_ResultSize;
static UINT32 s_SourceSize;

static SORT_TABLE *hmInitLinkTable( TOP_TREE *, DATA_INFO *);
static FLAG_TABLE *hmInitFlagTable( DATA_INFO *);
static void hmSortLink( SORT_TABLE *, UINT32);

UINT32 i3CompressHuffman::GetResultSize(void)
{
	return s_ResultSize;
}

UINT32 i3CompressHuffman::GetSourceSize(void)
{
	return s_SourceSize;
}

static INT32 huffman_compare( const void *op1, const void *op2)
{
	const SORT_TABLE *st1 = (SORT_TABLE *)op1;
	const SORT_TABLE *st2 = (SORT_TABLE *)op2;

	if(st1->count > st2->count) return -1;
	if(st1->count < st2->count) return  1;

	return 0;
}

static UINT32 hmCheckDataNums( HM_DATA *pSortedData, UINT32   DataSize)
{
	HM_DATA dat;
	UINT32   loop;
	UINT32   data_nums = 1;

	for( loop = 1; loop < DataSize; loop++) 
	{
		dat = *(pSortedData + loop - 1);

		if(dat != *(pSortedData+loop)) 
			data_nums++;
	}

	return data_nums;
}

static void hmCreateCountData( HM_DATA    *pData, UINT32      DataSize, DATA_COUNT *pDataCount)
{
	HM_DATA dat;
	UINT32   loop;
	UINT32   pos = 0;
	UINT32   count = 1;

	for( loop = 1; loop < DataSize; loop++) 
	{
		dat = *(pData + loop - 1);

		if(dat != *(pData+loop)) 
		{
			pDataCount[pos].m_Data = dat;
			pDataCount[pos].m_Count = count;
			count = 0;
			pos++;
		}

		count++;
	}

	pDataCount[pos].m_Data  = *(pData + loop - 1);
	pDataCount[pos].m_Count = count;
}

static INT32 data_info_compare( const void *op1, const void *op2)
{
	const HM_DATA *dt1 = (const HM_DATA *)op1;
	const HM_DATA *dt2 = (const HM_DATA *)op2;

	if(*dt1 > *dt2) return -1;
	if(*dt1 < *dt2) return  1;

	return 0;
}

static char hmAnalyzeData( UINT8 * pData,  UINT32 DataSize,  DATA_INFO *pDataInfo)
{
	UINT32   data_size;
	HM_DATA *buf;
	

	pDataInfo->size = DataSize;
	pDataInfo->tree_count = 0;

	data_size = (DataSize + sizeof(HM_DATA) - 1) / sizeof(HM_DATA);

	buf = (HM_DATA *) i3MemAlloc( data_size * sizeof(HM_DATA));
	if(buf == NULL) return 0;

	memcpy(buf, pData, DataSize);
	qsort( buf, data_size, sizeof(HM_DATA), data_info_compare);

	pDataInfo->num = hmCheckDataNums(buf, data_size);

	if(pDataInfo->num * sizeof(DATA_COUNT) > 0)
	{
		pDataInfo->count = (DATA_COUNT *) i3MemAlloc( pDataInfo->num * sizeof(DATA_COUNT));

		hmCreateCountData(buf, data_size, pDataInfo->count);
	}
	else
	{
		pDataInfo->count = NULL;
	}

	i3MemFree(buf);

	return 1;
}

static SORT_TABLE *hmInitLinkTable( TOP_TREE  *pTopTree, DATA_INFO *pDataInfo)
{
	UINT32      loop;
	SORT_TABLE *sort_table;

	//INT32 hMem;
	//hMem = i3MemAlloc( pDataInfo->num * sizeof(SORT_TABLE));
	sort_table = (SORT_TABLE *) i3MemAlloc( pDataInfo->num * sizeof(SORT_TABLE));
	if(sort_table == NULL) return NULL;

	for(loop = 0; loop < pDataInfo->num; loop++) 
	{
		sort_table[loop].count = pDataInfo->count[loop].m_Count;
		sort_table[loop].current_tree = pTopTree[loop].tree_link;
	}

	return sort_table;
}

static FLAG_TABLE *hmInitFlagTable( DATA_INFO *pDataInfo)
{
	UINT32      loop;
	FLAG_TABLE *flag_table;

	//INT32 hMem;
	//hMem = i3MemAlloc( pDataInfo->num * sizeof(FLAG_TABLE));
	flag_table = (FLAG_TABLE *) i3MemAlloc( pDataInfo->num * sizeof(FLAG_TABLE));
	if(flag_table == NULL) return NULL;

	for(loop = 0; loop < pDataInfo->num; loop++) 
	{
		flag_table[loop].flag       = 0;
		flag_table[loop].flag_width = 0;
		flag_table[loop].data       = pDataInfo->count[loop].m_Data;
	}

	return flag_table;

}

static TOP_TREE *hmInitTopTree( DATA_INFO *pDataInfo)
{
	UINT32    loop;
	TOP_TREE *top_tree;

	//INT32 hMem;
	//hMem = i3MemAlloc( pDataInfo->num * sizeof(TOP_TREE));

	top_tree = (TOP_TREE *) i3MemAlloc( pDataInfo->num * sizeof(TOP_TREE));
	if(top_tree == NULL) return NULL;

	for(loop = 0; loop < pDataInfo->num; loop++) 
	{
		//hMem = i3MemAlloc( sizeof(TREE));
		top_tree[loop].tree_link = (TREE *) i3MemAlloc( sizeof(TREE));

		top_tree[loop].tree_link->next_tree       = NULL;
		top_tree[loop].tree_link->prev_right_tree = NULL;
		top_tree[loop].tree_link->prev_left_tree  = NULL;

		top_tree[loop].tree_link->tree_id       = (UINT16) pDataInfo->tree_count+1;
		top_tree[loop].tree_link->tree_right_id = 0;
		top_tree[loop].tree_link->tree_left_id  = pDataInfo->count[loop].m_Data;

		pDataInfo->tree_count++;
	}

	return top_tree;
}

static void hmSortLink( SORT_TABLE *pSortTable, UINT32      DataNum)
{
	qsort(pSortTable, DataNum, sizeof(SORT_TABLE), huffman_compare);
}

static UINT32 hmCreateNewBranch( SORT_TABLE *pSortTable1, SORT_TABLE *pSortTable2, SORT_TABLE * /* pSortTable */ , UINT32 NewTreeID)
{
	TREE * pNewTree;

	//INT32 hMem;	
	//hMem = i3MemAlloc( sizeof(TREE));
	pNewTree = (TREE *) i3MemAlloc( sizeof(TREE));

	pNewTree->next_tree = NULL;
	pNewTree->tree_id = (UINT16) NewTreeID+1;

	pSortTable1->current_tree->bit = 0;
	pSortTable1->current_tree->next_tree = pNewTree;
	pNewTree->prev_right_tree = pSortTable1->current_tree;

	pNewTree->tree_right_id = pSortTable1->current_tree->tree_id;

	pSortTable2->current_tree->bit = 1;
	pSortTable2->current_tree->next_tree = pNewTree;
	pNewTree->prev_left_tree = pSortTable2->current_tree;

	pNewTree->tree_left_id = pSortTable2->current_tree->tree_id;

	pSortTable2->count += pSortTable1->count;

	pSortTable2->current_tree = pNewTree;

	return NewTreeID+1;
}

static UINT32 hmFlipFlag( UINT32 Flag,  UINT32 Width)
{
	UINT32 loop;
	UINT32 local_flag = 0;

	for(loop = 0; loop < Width; loop++) 
	{
		if((Flag&(1<<loop)) != 0) 
		{
			local_flag |= 1<<(Width-loop-1);
		}
	}

	return local_flag;
}

static FLAG_TABLE *hmCreateFlagFromTree( TOP_TREE  *pTopTree, DATA_INFO *pDataInfo)
{
	UINT32 loop;
	TREE  *tree;
	UINT32  flag;
	UINT8 flag_width;
	FLAG_TABLE *flag_table;

	pDataInfo->enc_size = 0;

	flag_table = hmInitFlagTable( pDataInfo);
	if(flag_table == NULL) return NULL;

	for(loop = 0; loop < pDataInfo->num; loop++) 
	{
		flag       = 0;
		flag_width = 0;

		tree = pTopTree[loop].tree_link;

		while(tree->next_tree != NULL) 
		{
			flag |= (tree->bit<<flag_width);
			flag_width += 1;
			tree = tree->next_tree;
		}

		if(flag_width == 0) flag_width = 1;

		flag = hmFlipFlag(flag, flag_width);
		flag_table[loop].flag       = flag;
		flag_table[loop].flag_width = flag_width;

		pDataInfo->enc_size += flag_width * pDataInfo->count[loop].m_Count;
	}

	return flag_table;
}

static TREE_TABLE *hmDestroyTree( TOP_TREE  *pTopTree, DATA_INFO *pDataInfo, UINT32     Size)
{
	UINT32 loop;
	TREE       *tree;
	TREE       *next_tree;
	TREE_TABLE *tree_table;

	//INT32 hMem;
	//hMem = i3MemAlloc( Size);
	tree_table = (TREE_TABLE *) i3MemAlloc( Size);
	if(tree_table == NULL) return NULL;

	for(loop = 0; loop < pDataInfo->num; loop++) 
	{
		tree = pTopTree[loop].tree_link;

		while(tree != NULL) 
		{
			tree_table[tree->tree_id].tree_id        = tree->tree_id;
			tree_table[tree->tree_id].tree_branch[0] = tree->tree_right_id;
			tree_table[tree->tree_id].tree_branch[1] = tree->tree_left_id;

			if(tree->next_tree != NULL) 
			{
				if(tree->bit) tree->next_tree->prev_left_tree  = NULL;
				else          tree->next_tree->prev_right_tree = NULL;
			}

			if(tree->prev_left_tree != NULL) 
			{
				tree->prev_left_tree->next_tree = NULL;
			}

			if(tree->prev_right_tree != NULL) 
			{
				tree->prev_right_tree->next_tree = NULL;
			}

			next_tree = tree->next_tree;
			i3MemFree( tree);
			tree = next_tree;
		}
	}

	tree_table[0].tree_id        = tree_table[pDataInfo->root_id].tree_id;
	tree_table[0].tree_branch[0] = tree_table[pDataInfo->root_id].tree_branch[0];
	tree_table[0].tree_branch[1] = tree_table[pDataInfo->root_id].tree_branch[1];

	i3MemFree( pTopTree);

	return tree_table;
}

static UINT32 hmCopyHeader2Buf( UINT8    *pBuf, I3_HUFFMAN_HEADER *pHeader)
{
	memcpy( pBuf, pHeader, pHeader->m_HeaderSize);

	return pHeader->m_HeaderSize;
}

static UINT32 hmCopyTree2Buf( UINT8     *pBuf, TREE_TABLE *pTreeTable,  DATA_INFO  *pDataInfo)
{
	UINT32 loop;
	UINT32 offset = 0;

	for(loop = 0; loop < pDataInfo->tree_count+1; loop++) 
	{
		memcpy(pBuf +offset, &pTreeTable[loop], sizeof(TREE_TABLE));
		offset += sizeof(TREE_TABLE);
	}

	return offset;
}

static UINT32 hmCopyData2Buf( UINT8 *pDst, UINT32 AccessTblNum, UINT32 AccessTblSize, UINT8 *pSrc, UINT32 Size, FLAG_TABLE *pFlagTable)
{
	HM_DATA val;
	UINT32   loop, loop2, len;
	UINT32   flag;
	UINT32   pos;
	UINT32   bitpos       = 0;
	UINT32   m_EncBitSize = 0;
	UINT32   *access_tbl  = (UINT32   *)(pDst);
	UINT16 *data_buf		= (UINT16 *)(pDst+AccessTblNum*sizeof(UINT32));
	UINT32   access_tbl_pos = 0;

	for(loop = 0; loop < Size; loop += sizeof(HM_DATA)) 
	{
		if(loop % AccessTblSize == 0) 
		{
			if(m_EncBitSize > AccessTblSize*8) 
			{
				I3TRACE( "Data Incleased!!\n");
				I3TRACE( "%d -> %d bit\n", AccessTblSize * 8, m_EncBitSize);
				return 0;
			}

			m_EncBitSize = 0;
			*(access_tbl+access_tbl_pos++) = bitpos;
		}

		val = *((HM_DATA *)(pSrc+loop));
		pos = 0;
		while(val != pFlagTable[pos].data) pos++;

		flag = pFlagTable[pos].flag;
		len  = pFlagTable[pos].flag_width;

		for(loop2 = 0; loop2 < len; loop2++) 
		{
			if((flag & ( 1 << (loop2))) != 0) 
			{
				*(data_buf+(bitpos>>4)) |= (1 << (bitpos & 15));
			}

			bitpos++;
			m_EncBitSize++;
		}
	}

	*(access_tbl+access_tbl_pos) = bitpos;

	return bitpos;

}

UINT8 * i3CompressHuffman::Encode( UINT8 *pData, UINT32  Size, UINT32  AccessUnitSize)
{
	static DATA_INFO data_info;

	SORT_TABLE *sort_table;
	TOP_TREE   *top_tree;
	FLAG_TABLE *flag_table;
	TREE_TABLE *tree_table;
	I3_HUFFMAN_HEADER  header;
	UINT32  num;
	UINT8 *buf;
	UINT32  offset = 0;

	if(Size == 0) return NULL;

	if(AccessUnitSize == 0) AccessUnitSize = Size;

	hmAnalyzeData(pData, Size, &data_info);

	top_tree   = hmInitTopTree(&data_info);
	sort_table = hmInitLinkTable(top_tree, &data_info);

	num = data_info.num;

	while(num > 1) 
	{
		hmSortLink( sort_table, num);

		num--;

		data_info.tree_count = hmCreateNewBranch( &sort_table[num], &sort_table[num-1], sort_table, data_info.tree_count);
	}

	data_info.root_id = sort_table[0].current_tree->tree_id;

	flag_table = hmCreateFlagFromTree(top_tree, &data_info);

	header.m_FlagSize = (data_info.tree_count+1)*sizeof(TREE_TABLE);

	tree_table = hmDestroyTree(top_tree, &data_info, header.m_FlagSize);

	i3MemFree( sort_table );
	i3MemFree( data_info.count );

	header.m_EncBitSize = data_info.enc_size;
	header.m_EncSize = (header.m_EncBitSize+7) / 8 + 1;

	header.m_HeaderSize     = sizeof(I3_HUFFMAN_HEADER);
	header.m_OriginalSize        = Size;
	header.m_UnitDecSize   = AccessUnitSize;
	header.m_AccessTblCount  = (Size+AccessUnitSize - 1) / AccessUnitSize + 1;
	header.m_AccessTblSize = header.m_AccessTblCount * sizeof(UINT32);

	{
		//INT32 hMem;
		//hMem = i3MemAlloc( header.m_HeaderSize + header.m_FlagSize + header.m_AccessTblSize + header.m_EncSize);
		buf = (UINT8 *) i3MemAlloc( header.m_HeaderSize + header.m_FlagSize + header.m_AccessTblSize + header.m_EncSize);
	}

	offset += hmCopyHeader2Buf(buf, &header);
	offset += hmCopyTree2Buf(buf+offset, tree_table, &data_info);

	hmCopyData2Buf(buf+offset, header.m_AccessTblCount, AccessUnitSize, pData, Size, flag_table);

	i3MemFree( flag_table );
	i3MemFree( tree_table );

	s_ResultSize  = header.m_HeaderSize;
	s_ResultSize += header.m_FlagSize;
	s_ResultSize += header.m_AccessTblSize;
	s_ResultSize += header.m_EncSize;

	return buf;
}

static void hmCreateDecodeTable( UINT8 * pData, DECODE_TABLE *pDecodeTable)
{
	I3_HUFFMAN_HEADER *header = (I3_HUFFMAN_HEADER *)pData;

	UINT32 flag_tbl_ofs    = header->m_HeaderSize;
	UINT32 access_tbl_ofs  = header->m_HeaderSize+header->m_FlagSize;
	UINT32 data_ofs        = access_tbl_ofs+header->m_AccessTblSize;

	pDecodeTable->tree_table     = (TREE_TABLE *)(pData+flag_tbl_ofs);
	pDecodeTable->access_table   = (UINT8 *)(pData+access_tbl_ofs);
	pDecodeTable->data           = (UINT8 *)(pData+data_ofs);
}

static UINT8 *hmCopyDecodeData2Buf( UINT32 AccessUnit, UINT8 *pData, DECODE_TABLE *pDecodeTable)
{
	I3_HUFFMAN_HEADER  *header;
	TREE_TABLE *tree_table = pDecodeTable->tree_table;

	UINT32 val;
	UINT8  *buf;
	UINT16 *data;
	UINT32   bufofs = 0;
	UINT32   idx, count;
	UINT32   bitpos, endbit;
	ID_DATA tree_id;
	ID_DATA *branch;
	ID_DATA *root_branch;

	memcpy( &bitpos, pDecodeTable->access_table+(AccessUnit  )*4, 4);
	memcpy( &endbit, pDecodeTable->access_table+(AccessUnit+1)*4, 4);

	header = (I3_HUFFMAN_HEADER *)(pData);

	if(header->m_UnitDecSize == 0) return NULL;

	{
//		INT32 hMem;
//		hMem = i3MemAlloc( header->m_UnitDecSize);
		buf = (UINT8 *) i3MemAlloc( header->m_UnitDecSize);
		if(buf == NULL) return NULL;
	}

	root_branch = tree_table[0].tree_branch;
	branch      = root_branch;

	data  = (UINT16 *) pDecodeTable->data;

	idx   = bitpos >> 4;
	val   = (data[idx] | 0x10000) >> (bitpos & 15);
	count = endbit - bitpos;

	do 
	{
		tree_id = branch[val & 1];
		branch  = tree_table[tree_id].tree_branch;

		if(branch[0] == 0) 
		{
			memcpy( buf + bufofs, &branch[1], sizeof(HM_DATA));
			bufofs += sizeof(HM_DATA);
			branch  = root_branch;
		}

		if((val >>= 1) == 1)
		{
			idx += 1;
			val  = data[idx] | 0x10000;
		}
	} while(--count != 0);

	s_ResultSize = bufofs;

	return buf;
}

UINT8 * i3CompressHuffman::Decode( UINT8 *pData, UINT32  AccessUnit)
{
	I3_HUFFMAN_HEADER *header;
	DECODE_TABLE decode_tbl;

	UINT8 *buf;

	header = (I3_HUFFMAN_HEADER *)(pData);

	if(header->m_AccessTblCount-1 <= AccessUnit) return NULL;

	hmCreateDecodeTable(pData, &decode_tbl);

	buf = hmCopyDecodeData2Buf(AccessUnit, pData, &decode_tbl);

	return buf;
}

BOOL i3CompressHuffman::EncodeFile( const char * pszSrc, const char * pszDest)
{
	i3FileStream stream;
	BOOL Rv;
	char * pSrcBuf, * pDestBuf;	
	UINT32 Rc, SrcSize, DestSize;

	Rv = stream.Open( pszSrc, STREAM_READ | STREAM_SHAREREAD);
	if( Rv == FALSE)
	{
		i3Log( "i3CompressHuffman::EncodeFile()", "Could not open %s source file.", pszSrc);
		return FALSE;
	}

	SrcSize = stream.GetSize();
	s_SourceSize = SrcSize;

	//INT32 hMem;
	//hMem = i3MemAlloc( SrcSize);
	pSrcBuf = (char *) i3MemAlloc( SrcSize);

	Rc = stream.Read( pSrcBuf, SrcSize);

	stream.Close();

	if( Rc == STREAM_ERR)
	{
		i3Log( "i3CompressHuffman::EncodeFile()", "Could not read %s source file", pszSrc);
		
		return FALSE;
	}

	pDestBuf = (char *) Encode( (UINT8 *) pSrcBuf, SrcSize, 0);
	DestSize = GetResultSize();

	Rv = stream.Create( pszDest, STREAM_WRITE);
	if( Rv == FALSE)
	{
		i3Log( "i3CompressHuffman::EncodeFile()", "Could not create %s destination file.", pszDest);
		I3MEM_SAFE_FREE( pDestBuf);
		return FALSE;
	}
	
	Rc = stream.Write( pDestBuf, DestSize);

	stream.Close();

	if( Rc == STREAM_ERR)
	{
		i3Log( "i3CompressHuffman::EncodeFile()", "Could not write %s destination file.", pszDest);
		I3MEM_SAFE_FREE( pDestBuf);
		return FALSE;
	}

	i3MemFree( pDestBuf );

	return TRUE;
}

BOOL i3CompressHuffman::DecodeFile( const char * pszSrc, const char * pszDest)
{
	i3FileStream stream;
	BOOL Rv;
	char * pSrcBuf, * pDestBuf;	
	UINT32 Rc, SrcSize, DestSize;

	Rv = stream.Open( pszSrc, STREAM_READ | STREAM_SHAREREAD);
	if( Rv == FALSE)
	{
		i3Log( "i3CompressHuffman::DecodeFile()", "Could not open %s source file.", pszSrc);
		return FALSE;
	}

	SrcSize = stream.GetSize();
	s_SourceSize = SrcSize;

	//INT32 hMem;
	//hMem = i3MemAlloc( SrcSize);
	pSrcBuf = (char *) i3MemAlloc( SrcSize);

	Rc = stream.Read( pSrcBuf, SrcSize);

	stream.Close();

	if( Rc == STREAM_ERR)
	{
		i3Log( "i3CompressHuffman::DecodeFile()", "Could not read %s source file", pszSrc);
		
		return FALSE;
	}

	pDestBuf = (char *) Decode( (UINT8 *) pSrcBuf, 0);
	DestSize = GetResultSize();

	Rv = stream.Create( pszDest, STREAM_WRITE);
	if( Rv == FALSE)
	{
		i3Log( "i3CompressHuffman::DecodeFile()", "Could not create %s destination file.", pszDest);
		I3MEM_SAFE_FREE( pDestBuf);
		return FALSE;
	}
	
	Rc = stream.Write( pDestBuf, DestSize);

	stream.Close();

	if( Rc == STREAM_ERR)
	{
		i3Log( "i3CompressHuffman::DecodeFile()", "Could not write %s destination file.", pszDest);
		I3MEM_SAFE_FREE( pDestBuf);
		return FALSE;
	}

	i3MemFree( pDestBuf);

	return TRUE;
}
