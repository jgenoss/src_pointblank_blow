#include "i3CommonType.h"

#include "pool/pool_dbg.h"
#include "pool/pool_dbg_apply.h"
#include "pool/pool_dbg_block.h"
#include "pool/cs.h"

#include "inst/ginst.h"

#if defined(_DEBUG) && defined(I3_POOL_DBG)		//  릴리즈버전에는 구현 자체를 제낄 것...

#pragma warning(push)
#pragma warning(disable : 4995 )
#include <intrin.h>
#pragma warning(pop)

namespace i3
{
	// 디버그용 누수정보는 락프리로 해석하기가 무척 어려움...(속편하게 단일인스턴스로 만들고, 크리티컬 섹션을 걸자..)

	// 32+16 = 48 / 40+16 = 56바이트..
	// 바이트 최대정렬은 os비트에 관계없이 컴파일디폴트옵션 8임..

	struct dbg_state : public shared_ginst<dbg_state>
	{
		dbg_state() : first(NULL), last(NULL), request_count(0), accum_size(0), curr_alloc_size(0), max_alloc_size(0),
			max_size_per_alloc(0), breakAlloc(0), critical_section(1024) { }

		dbg_block*	first;
		dbg_block*	last;
		DWORD		request_count;
		DWORD		accum_size;				// 누적만...
		DWORD		curr_alloc_size;		// 누적과 감소..
		DWORD		max_alloc_size;			// curr_alloc_size의 최대치를 기록..
		DWORD		max_size_per_alloc;		// 1회 얼로케이션당 최대치..
		DWORD		breakAlloc;				// 브레이크변수..
		cs			critical_section;
	};
	
	template<> struct use_delayed_atexit_ginst<dbg_state> : true_type {};

	void		init_dbg_util() 
	{	
		dbg_state::i();   
	}
	
	const static BYTE		byCheck_DeadLand	= 0xDD;		// 데드.
	const static BYTE		byCheck_CleanLand	= 0xCD;		// 클린.
	const static DWORD		dwCheck_NoMansLand =  0xFDFDFDFD;
	const static uintptr_t	dwCheck_AlignLand = ( sizeof(void*) == sizeof(DWORD) ) ? 0xEDEDEDED : 0xEDEDEDEDEDEDEDED;	// 빈영역.
	
	__forceinline int			Extract_LineNumber(DWORD dwLineFlag) { return  static_cast<int>(dwLineFlag & 0x00FFFFFF);	}
	
	__forceinline size_t		Extract_Align(DWORD dwLineFlag) { return static_cast<size_t>(dwLineFlag >> 24); }

	__forceinline DWORD			Make_LineFlag(int line, size_t align) 
	{
		return ( static_cast<DWORD>(align) << 24 ) | ( static_cast<DWORD>(line) & 0x00FFFFFF );
	}
	
	size_t		calculate_dbg_data_size( size_t size)				// 풀은 이것으로 할당하고...
	{
		return sizeof( i3::dbg_block ) + sizeof(DWORD) + size;
	}

	
	size_t		calculate_aligned_dbg_data_size( size_t size, size_t adjusted_align)
	{
		return sizeof( i3::dbg_block) + sizeof(DWORD) + size + adjusted_align + sizeof( i3::aligned_dummy );
	}

	void*		apply_dbg_data_impl( const char* szFilename, int line, 
		void* raw_data, size_t orig_size, const char* szClassname, size_t align)
	{
		
		dbg_block* header = reinterpret_cast<dbg_block*>(raw_data);
		
		DWORD  line_flag = Make_LineFlag(line, align);
		
		DWORD real_data_size;
		if (align > 0 )
			real_data_size = DWORD( orig_size - ( align + sizeof( i3::aligned_dummy ) ) );
		else
			real_data_size = DWORD(orig_size);

		dbg_state& state = *dbg_state::i();

		///////////////////////////////////////////////////////////////////////////
		state.critical_section.lock();
		///////////////////////////////////////////////////////////////////////////
		if ( state.breakAlloc == state.request_count + 1 )
			__debugbreak();

		state.accum_size += real_data_size;	// 할당양 누적..(free시 감소시키지 않음)
		state.curr_alloc_size += real_data_size;	// 할당양 누적..(free시 감소)

		if ( state.curr_alloc_size > state.max_alloc_size )
			state.max_alloc_size = state.curr_alloc_size;	// 누적 최대치를 기록..

		if ( real_data_size > state.max_size_per_alloc )
			state.max_size_per_alloc = real_data_size;		// 1회 최고 할당량을 기록..

		if ( state.first )
			state.first->prev = header;					// 헤더포인터를 맨 앞으로..
		else
			state.last = header;

		header->next = state.first;
		header->prev = NULL;
		header->szFilename = szFilename;
		header->line_flag  = line_flag;
		header->data_size  = (DWORD)orig_size;
		header->szDataType = szClassname;							// crtdbg와는 이것만 차이가 있음..
		header->request_number = state.request_count;

		++state.request_count;		// 할당 갯수 (쓰레드별)
		state.first = header;
		////////////////////////////////////////////////////////////////////////////////////
		state.critical_section.unlock();
		////////////////////////////////////////////////////////////////////////////////////

		// 앞뒤에 무인도바이트..
		header->gap = dwCheck_NoMansLand;		// 0xFDFDFDFD  // 32,64관계없이 4바이트 채움..
		BYTE*  orig_start = reinterpret_cast<BYTE*>(header + 1);
		DWORD* orig_end = reinterpret_cast<DWORD*>( orig_start + orig_size );
		*orig_end = dwCheck_NoMansLand;

		::memset( orig_start, byCheck_CleanLand, orig_size);   //0xCD로 채움...

		return orig_start;

	}

	
	void*		apply_dbg_data( const char* szFilename, int line, 
		void* raw_data, size_t orig_size, const char* szClassname )
	{
		return apply_dbg_data_impl(szFilename, line, raw_data, orig_size, szClassname, 0);
	}

	void*		apply_aligned_dbg_data( const char* szFilename, int line, void* raw_data, 
		size_t adjusted_align, size_t orig_size, const char* szClassname)
	{
		// 사이즈 변형 요구됨..

		orig_size		 += adjusted_align + sizeof(aligned_dummy);
		void* raw_data2 = apply_dbg_data_impl(szFilename, line, raw_data, orig_size, szClassname, adjusted_align);

		uintptr_t result =( uintptr_t(raw_data2) + sizeof(aligned_dummy) + adjusted_align - 1 ) & ~(adjusted_align - 1);

		aligned_dummy* dummy = ( (aligned_dummy*)result ) - 1;
		dummy->head = raw_data2;
		dummy->gap  = dwCheck_AlignLand;

		return (void*) result;
	}

	//
	//  처리부터 하고, 풀에 반납하던가... (풀데이터의 헤더는 이 디버그 버퍼보다 더 앞에 있음)
	//
	
	void*		cleanup_raw_data_from_dbg_data( void* dbg_data)
	{
		if (!dbg_data)
			return NULL;

		uintptr_t* align_gap_pos = (uintptr_t*)(  (uintptr_t)dbg_data & ~(sizeof(uintptr_t) - 1)  ) - 1 ;

		if ( *align_gap_pos == dwCheck_AlignLand )
		{
			_RPT1( _CRT_ERROR, "The Block at 0x%p was allocated by aligned routines, use aligned deallocated function.", dbg_data);
			return NULL;
		}

		dbg_block* header = reinterpret_cast<dbg_block*>(dbg_data) - 1;
		if (header->gap != dwCheck_NoMansLand)
		{
			if (header->szFilename)
			{
				int iLine = Extract_LineNumber(header->line_flag);

				if (header->szDataType)
					_RPT5( _CRT_ERROR,  
					"HEAP CORRUPTION DETECTED: before block (#%d) at 0x%p.\n"
					"i3::pool_dbg detected that the app wrote to memory before start of heap buffer(type = %hs).\n"
					"\nMemory allocated at %hs(%d).\n",
					header->request_number,
					(BYTE*)dbg_data,
					header->szDataType,
					header->szFilename,
					iLine );
				else
					_RPT4( _CRT_ERROR,  
					"HEAP CORRUPTION DETECTED: before block (#%d) at 0x%p.\n"
					"i3::pool_dbg detected that the app wrote to memory before start of heap buffer.\n"
					"\nMemory allocated at %hs(%d).\n",
					header->request_number,
					(BYTE*)dbg_data,
					header->szFilename,
					iLine );
			}
			else
			{
				if (header->szDataType)
					_RPT3( _CRT_ERROR,  
					"HEAP CORRUPTION DETECTED: before block (#%d) at 0x%p.\n"
					"i3::pool_dbg detected that the app wrote to memory before start of heap buffer(type = %hs).\n",
					header->request_number,
					(BYTE*)dbg_data,
					header->szDataType);	
				else
					_RPT2(_CRT_ERROR,  
					"HEAP CORRUPTION DETECTED: before block (#%d) at 0x%p.\n"
					"i3::pool_dbg detected that the app wrote to memory before start of heap buffer.\n",
					header->request_number,
					(BYTE*)dbg_data);	
			}

		}

		DWORD* data_end = (DWORD*) ( (BYTE*)dbg_data + header->data_size );

		if ( *data_end != dwCheck_NoMansLand )
		{
			if (header->szFilename)
			{
				int iLine = Extract_LineNumber(header->line_flag);

				if (header->szDataType)
					_RPT5( _CRT_ERROR,  
					"HEAP CORRUPTION DETECTED: before block (#%d) at 0x%p.\n"
					"i3::pool_dbg detected that the app wrote to memory after end of heap buffer(type = %hs).\n"
					"\nMemory allocated at %hs(%d).\n",
					header->request_number,
					(BYTE*)dbg_data,
					header->szDataType,
					header->szFilename,
					iLine );
				else
					_RPT4( _CRT_ERROR,  
					"HEAP CORRUPTION DETECTED: before block (#%d) at 0x%p.\n"
					"i3::pool_dbg detected that the app wrote to memory after end of heap buffer.\n"
					"\nMemory allocated at %hs(%d).\n",
					header->request_number,
					(BYTE*)dbg_data,
					header->szFilename,
					iLine );
			}
			else
			{
				if (header->szDataType)
					_RPT3( _CRT_ERROR,  
					"HEAP CORRUPTION DETECTED: before block (#%d) at 0x%p.\n"
					"i3::pool_dbg detected that the app wrote to memory after end of heap buffer(type = %hs).\n",
					header->request_number,
					(BYTE*)dbg_data,
					header->szDataType);	
				else
					_RPT2(_CRT_ERROR,  
					"HEAP CORRUPTION DETECTED: before block (#%d) at 0x%p.\n"
					"i3::pool_dbg detected that the app wrote to memory after end of heap buffer.\n",
					header->request_number,
					(BYTE*)dbg_data);	
			}
		}	



		DWORD real_data_size;
		size_t align = Extract_Align(header->line_flag);
		if (align > 0 )
			real_data_size = header->data_size - (DWORD)( align + sizeof( i3::aligned_dummy ) );
		else
			real_data_size = header->data_size;

		dbg_state& state = *dbg_state::i();

		///  이 사이에 락이 필요함..
		////////////////////////////////////////////////////////////////////////
		state.critical_section.lock();
		////////////////////////////////////////////////////////////////////////
		
		state.curr_alloc_size -= real_data_size;

		if (header->next)
			header->next->prev = header->prev;
		else
			state.last = header->prev;

		if (header->prev)
			header->prev->next = header->next;
		else
			state.first = header->next;
////////////////////////////////////////////////////////////////////////
		state.critical_section.unlock();
////////////////////////////////////////////////////////////////////////
		
		::memset( header, byCheck_DeadLand, sizeof(dbg_block) + header->data_size + sizeof(DWORD));

		return header;			// raw_data pointer = header pointer
	}

	void*		cleanup_raw_data_from_aligned_dbg_data( void* dbg_data)
	{
		if (!dbg_data)
			return NULL;

		if (  *( (DWORD*)(dbg_data) - 1 ) == dwCheck_NoMansLand )
		{
			_RPT1(_CRT_ERROR, "The block at 0x%p was not allocated by _aligned routines, don't use aligned deallocation function.", dbg_data);
			return NULL;
		}

		aligned_dummy* dummy = (aligned_dummy*) ( uintptr_t(dbg_data) & ~(sizeof(uintptr_t) - 1) ) - 1;

		if ( dummy->gap != dwCheck_AlignLand )
		{
			_RPT1(_CRT_ERROR, "Damage before 0x%p which was allocated by aligned routine\n", dbg_data);
		}

		return cleanup_raw_data_from_dbg_data( dummy->head );		
	}



	void	print_dbg_memory(dbg_block* mem )
	{
		static const int NUM_PRINT = 32;

		BYTE				printbuff[ NUM_PRINT + 1];
		BYTE				valbuff[ NUM_PRINT * 3 + 1];

		int i;
		
		size_t align = Extract_Align(mem->line_flag);

		BYTE*	data_start;
		int		num_size;

		if (align > 0)
		{
			data_start = (BYTE*) ( ( uintptr_t(mem + 1) + sizeof(aligned_dummy) + align - 1) & ~(align - 1) );
			num_size = int( uintptr_t(mem->data_size) - sizeof(aligned_dummy) - align);
			num_size   = ( num_size > NUM_PRINT ) ? NUM_PRINT : num_size;
		}
		else
		{
			data_start = (BYTE*)( mem + 1);
			num_size = ( mem->data_size > NUM_PRINT) ? NUM_PRINT : mem->data_size;
		}

//		int num_size = (  mem->data_size > NUM_PRINT) ? NUM_PRINT :  mem->data_size;
//		BYTE* data_start = (BYTE*)( mem + 1);

		BYTE ch;
		for ( i = 0 ; i < num_size ; ++i )		
		{	
			ch = data_start[i];
			printbuff[i] = isprint( ch ) ? ch : ' ';
			sprintf_s((char*)&valbuff[i*3], _countof(valbuff) - (i * 3), "%.2X ", ch);
		}
		printbuff[i] = '\0';
		_RPT2(_CRT_WARN, " Data: <%s> %s\n", printbuff, valbuff);
	}

	void		dump_leak()			// 프로세스 종료전에 호출될 필요가 있으므로, 
	{								// tls쪽 dll에서 callback이 되도록 조정할것..
		dbg_state& state = *dbg_state::i();	
		// pool이 소거되기 전에 처리할것...(사전처리)
		
		state.critical_section.lock();

		if (state.first)
		{
			_RPT0(_CRT_WARN, "i3::pool_dbg : Detected memory leaks!\n");

			_RPT0(_CRT_WARN, "Dumping objects ->\n");

			int iLine;
			size_t align;
			DWORD real_data_size;

			for ( dbg_block* head = state.first; head != NULL ; head = head->next )
			{
				
				if ( head->szFilename )		// 파일네임의 확보유무에 따라
				{
					iLine = Extract_LineNumber(head->line_flag);
					_RPT2(_CRT_WARN, "%hs(%d) : ", head->szFilename, iLine);
				}

				_RPT1(_CRT_WARN, "{%ld} ", head->request_number);
				
				align = Extract_Align(head->line_flag);
				
				if (align > 0)
					real_data_size = head->data_size - DWORD( align + sizeof(aligned_dummy) );
				else
					real_data_size = head->data_size;


				if ( head->szDataType)
					_RPT3(_CRT_WARN, "normal block at 0x%p, %Iu bytes long. (type = %hs).\n", (BYTE*)( head + 1), real_data_size, head->szDataType);
				else
					_RPT2(_CRT_WARN, "normal block at 0x%p, %Iu bytes long.\n", (BYTE*)( head + 1), real_data_size);

				print_dbg_memory(head);
			}
		}
		else
		{
			_RPT0(_CRT_WARN, "i3::pool_dbg : No leaks detected!\n");	// 릭없음..
		}
		
		_RPT0(_CRT_WARN, "\n");
		_RPT1(_CRT_WARN, "Total Alloc Count: %Id times.\n", state.request_count );
		_RPT1(_CRT_WARN, "Total Alloc Size: %Id bytes.\n", state.accum_size);
		_RPT1(_CRT_WARN, "Maximum Alloc State Size: %Id bytes.\n", state.max_alloc_size);
		_RPT1(_CRT_WARN, "Current Not yet Deallocated Size: %Id bytes.\n", state.curr_alloc_size );
		_RPT1(_CRT_WARN, "Maximum Block Size: %Id bytes.\n", state.max_size_per_alloc );
		_RPT0(_CRT_WARN, "\n");
		_RPT0(_CRT_WARN, "i3::pool_dbg : Object dump complete.\n");

		state.critical_section.unlock();
	}



	DWORD	set_break_alloc(DWORD alloc_number)
	{
		dbg_state& state = *dbg_state::i();	

		DWORD old =	state.breakAlloc;

		state.breakAlloc = alloc_number;

		return old;
	}



	
	
	
	
}


#endif