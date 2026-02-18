#include "pch.h"
#include "ModeScript.h"

#include "ScriptDef.h"

IModeScript::IModeScript(i3Stage* pStage) : m_hL( luaL_newstate() ), m_pStage(pStage)
{
	I3ASSERT(m_hL);
	luaL_openlibs(m_hL);
	
	m_msa.l = m_hL;
	m_msa.narg = 0;

	memset( m_i32RoomModeData, 0, sizeof(INT32) * MODE_DATA_MAX );
}

IModeScript::~IModeScript()
{
	lua_close(m_hL);
	IModeScript::_clear_modedata_cb();
}


void IModeScript::Reset( i3Stage* pStage, bool bDataReset )
{
	m_pStage	= pStage;
	
	if( bDataReset)
	{
		memset( m_i32RoomModeData, 0, sizeof(INT32) * MODE_DATA_MAX );
	}
}

bool IModeScript::Open(const char* szFilename)
{
	lua_pushcclosure(m_hL, on_error, 0);
	int errfunc = lua_gettop(m_hL);

	if(luaL_loadfile(m_hL, szFilename) == 0)
	{
		if(lua_pcall(m_hL, 0, 0, errfunc) != 0)
		{
			lua_pop(m_hL, 1);
			return false;
		}
	}
	else
	{
		print_error(m_hL, "%s", lua_tostring(m_hL, -1));
		lua_pop(m_hL, 1);
		return false;
	}
	lua_pop(m_hL, 1);
	
	// 클래스 등록
	lua_pushnumber( m_hL, lua_userdata_type_owner );
	lua_pushlightuserdata( m_hL, (void*)this );
	lua_settable( m_hL, LUA_REGISTRYINDEX );
	
	
	this->OnOpen();

	return true;
}

void IModeScript::RegFunc(const luaL_Reg* pLuaReg)
{
	for( INT32 i = 0; pLuaReg[i].name != NULL; i++ )
	{
		lua_register( m_hL, pLuaReg[i].name, pLuaReg[i].func );
	}
}

void IModeScript::call_stack(lua_State* L, int n)
{
	lua_Debug ar;
	if(lua_getstack(L, n, &ar) == 1)
	{
		lua_getinfo(L, "nSlu", &ar);

		const char* indent;
		if(n == 0)
		{
			indent = "->\t";
			print_error(L, "\t<call stack>");
		}
		else
		{
			indent = "\t";
		}

		if(ar.name)
			print_error(L, "%s%s() : line %d [%s : line %d]", indent, ar.name, ar.currentline, ar.source, ar.linedefined);
		else
			print_error(L, "%sunknown : line %d [%s : line %d]", indent, ar.currentline, ar.source, ar.linedefined);

		call_stack(L, n+1);
	}
}

int IModeScript::on_error(lua_State *L)
{
	const char* szError = lua_tostring(L, -1);
	if (szError)
		print_error(L, "%s", szError);
	else
		print_error(L, "unknown lua error");

	call_stack(L, 0);

	return 0;	
}


//


INT32	IModeScript::get_global_int(const char* g)
{
	lua_pushstring(m_hL, g);
	lua_rawget(m_hL, LUA_GLOBALSINDEX);

	if( !IModeScript::convertible_numeric() )
	{
		I3TRACE("Return type 'int' expected for variable '%s'", g);
	}
	INT32 res =	IModeScript::get_stack_int();
	lua_pop(m_hL, 1);			// getglobal의 결과치스택은 비운다..
	return res;
}

REAL32	IModeScript::get_global_real(const char* g)
{
	lua_pushstring(m_hL, g);
	lua_rawget(m_hL, LUA_GLOBALSINDEX);

	if( !IModeScript::convertible_numeric() )
	{
		I3TRACE("Return type 'int' expected for variable '%s'", g);
	}

	REAL32 res = IModeScript::get_stack_real();
	lua_pop(m_hL, 1);			// getglobal의 결과치스택은 비운다..
	return res;
}

INT32	IModeScript::get_table_enum_int( const char* table_name, const char* enum_name)
{
	lua_pushstring(m_hL, table_name);
	lua_rawget(m_hL, LUA_GLOBALSINDEX);

	if (!IModeScript::convertible_table() )	
	{
		I3TRACE("Return type 'table' expected for variable '%s'", table_name);
	}
	lua_pushstring(m_hL, enum_name);
	lua_rawget(m_hL, -2);	// name이 아니라 테이블을 넣는다..
	if( !IModeScript::convertible_numeric() )
	{
		I3TRACE("Return type 'int' expected for variable '%s'", enum_name);
	}
	INT32 res = IModeScript::get_stack_int();
	
	lua_pop(m_hL, 2);		// 첫테이블과 그 내부값이므로 2개라고 보임..

	return res;
}

INT32	IModeScript::get_table_array_2d_int(const char* table_name, INT32 i, INT32 j)	// 이걸로 루프를 돌리면 아깝다.검색용도.
{
	lua_pushstring(m_hL, table_name);
	lua_rawget(m_hL, LUA_GLOBALSINDEX);
	
	if (!IModeScript::convertible_table() )	
	{
		I3TRACE("Return type 'table' expected for variable '%s'", table_name);
	}
	
	IModeScript::access_table_idx(i);
	IModeScript::access_table_idx(j);	
	
	INT32 res = IModeScript::get_stack_int();
	lua_pop(m_hL, 3);		// 이중테이블 + 결과수치...
	return res;
}

void	IModeScript::access_global_table(const char* table_name)
{
	lua_pushstring(m_hL, table_name);
	lua_rawget(m_hL, LUA_GLOBALSINDEX);
}
void	IModeScript::access_table(const char* table_name)
{
	lua_pushstring(m_hL, table_name);
	lua_rawget(m_hL, -2);
}


void	IModeScript::access_index_from_table(INT32 idx)
{
	lua_rawgeti(m_hL, -1, idx + 1); 
}



void IModeScript::print_error(lua_State *L, const char* fmt, ...)
{
	char text[4096] = { 0, };

	va_list args;
	va_start(args, fmt);
	vsprintf_s(text, fmt, args);
	va_end(args);

	lua_pushstring(L, "_ALERT");
	lua_gettable(L, LUA_GLOBALSINDEX);
	if(lua_isfunction(L, -1))
	{
		lua_pushstring(L, text);
		lua_call(L, 1, 0);
	}
	else
	{
		I3TRACE("%s\n", text);
		lua_pop(L, 1);
	}
}

void IModeScript::print_stack( lua_State *L )
{
	for ( int i = lua_gettop(L) ; i >= 1 ; --i)
	{
		I3TRACE("No.%02d | ", i);
		switch (lua_type(L, i))
		{
		case LUA_TNIL:			I3TRACE("\t%s", lua_typename(L, lua_type(L, i)));												break;
		case LUA_TBOOLEAN:		I3TRACE("\t%s	%s", lua_typename(L, lua_type(L, i)), lua_toboolean(L, i)?"true":"false");	break;
		case LUA_TLIGHTUSERDATA:I3TRACE("\t%s	0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));				break;
		case LUA_TNUMBER:		I3TRACE("\t%s	%f", lua_typename(L, lua_type(L, i)), lua_tonumber(L, i));					break;
		case LUA_TSTRING:		I3TRACE("\t%s	%s", lua_typename(L, lua_type(L, i)), lua_tostring(L, i));					break;
		case LUA_TTABLE:		I3TRACE("\t%s	0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));				break;
		case LUA_TFUNCTION:		I3TRACE("\t%s()	0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));				break;
		case LUA_TUSERDATA:		I3TRACE("\t%s	0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));				break;
		case LUA_TTHREAD:		I3TRACE("\t%s", lua_typename(L, lua_type(L, i)));											break;
		default:				I3TRACE("unknown	| \n");																		break;
		}
	}
}

namespace
{
	struct DeleteFn
	{
		template<class T>
		void operator()(T* data) const { delete data; }
	};
}

mode_script_arg&	IModeScript::call(const char* str_fun)
{
	lua_getglobal( m_hL, str_fun );

	m_msa.l = m_hL;
	m_msa.narg = 0;
	m_msa.fun_name = str_fun;

	std::for_each(m_msa.rlist.begin(), m_msa.rlist.end(), DeleteFn());
	m_msa.rlist.clear();

	return m_msa;
}

mode_script_arg&	operator<<(mode_script_arg& a, REAL64 r)
{
	lua_pushnumber(a.l, lua_Number(r));	++a.narg;
	return a;
}

mode_script_arg&	operator<<(mode_script_arg& a, REAL32 r)
{
	lua_pushnumber(a.l, lua_Number(r));	++a.narg;
	return a;
}

mode_script_arg&	operator<<(mode_script_arg& a, bool  b)
{
	lua_pushboolean( a.l, b );	++a.narg;
	return a;
}

mode_script_arg&	operator<<(mode_script_arg& a, INT32 i)
{
	lua_pushinteger( a.l, i); ++a.narg;
	return a;
}

mode_script_arg&	operator<<(mode_script_arg& a, const char* str)
{
	lua_pushstring( a.l, str);	++a.narg;
	return a;
}

mode_script_arg&	operator>>(mode_script_arg& a, mode_script_arg& (*f)(mode_script_arg& )  )
{
	return (*f)(a);
}

template<class T>
struct mode_scrpit_tret : mode_script_arg::process_ret
{
	mode_scrpit_tret(T& data) : data(data) {}
	T& data;
	virtual void  operator()(mode_script_arg& a, INT32 mstack, INT32 i); 
};

// 멤버함수 특화..
template<>
void	mode_scrpit_tret<REAL64>::operator()(mode_script_arg& a, INT32 mstack, INT32 i)
{
	INT32 stack_idx = mstack + i; 

	if (!lua_isnumber(a.l, stack_idx))
	{
		I3TRACE("Return type 'double' expected for function call '%s', return value #i", a.fun_name, i);
	}

	data = lua_tonumber(a.l, stack_idx);
	
}

template<>
void	mode_scrpit_tret<REAL32>::operator()(mode_script_arg& a, INT32 mstack, INT32 i)
{
	INT32 stack_idx = mstack + i; 

	if (!lua_isnumber(a.l, stack_idx))
	{
		I3TRACE("Return type 'float' expected for function call '%s', return value #i", a.fun_name, i);
	}
	
	data = lua_tonumber(a.l, stack_idx);
	
}

template<>
void	mode_scrpit_tret<bool>::operator()(mode_script_arg& a, INT32 mstack, INT32 i)
{
	INT32 stack_idx = mstack + i; 

	if( !lua_isboolean(  a.l, stack_idx ) )
	{
		I3TRACE("Return type 'bool' expected for function call '%s', return value #i", a.fun_name, i);
	}

	data =	(lua_toboolean(  a.l, stack_idx ) != 0);
	
}

template<>
void	mode_scrpit_tret<INT32>::operator()(mode_script_arg& a, INT32 mstack, INT32 i)
{
	INT32 stack_idx = mstack + i; 

	if( !lua_isnumber( a.l, stack_idx ) )
	{
		I3TRACE("Return type 'int' expected for function call '%s', return value #i", a.fun_name, i);
	}
	data =	lua_tointeger(  a.l, stack_idx );
	
}	

template<>
void	mode_scrpit_tret<char*>::operator()(mode_script_arg& a, INT32 mstack, INT32 i)
{
	INT32 stack_idx = mstack + i; 

	if( !lua_isstring( a.l, stack_idx ) )
	{
		I3TRACE("Return type 'string' expected for function call '%s', return value #i", a.fun_name, i);
	}
	data = (char *) lua_tostring( a.l, stack_idx );
	
}


mode_script_arg&	operator>>(mode_script_arg& a, REAL64& r)
{
	a.rlist.push_back(new mode_scrpit_tret<REAL64>(r));
	return a;
}

mode_script_arg&	operator>>(mode_script_arg& a, REAL32& r)
{
	a.rlist.push_back(new mode_scrpit_tret<REAL32>(r));
	return a;
}

mode_script_arg&	operator>>(mode_script_arg& a, bool& b)
{
	a.rlist.push_back(new mode_scrpit_tret<bool>(b));
	return a;
}

mode_script_arg&	operator>>(mode_script_arg& a, INT32& i)
{
	a.rlist.push_back(new mode_scrpit_tret<INT32>(i));
	return a;
}

mode_script_arg&	operator>>(mode_script_arg& a, char*& str)
{
	a.rlist.push_back(new mode_scrpit_tret<char*>(str));
	return a;
}


mode_script_arg&	end_call(mode_script_arg& a)
{
	const INT32 num_ret = INT32(a.rlist.size());
	if( lua_pcall( a.l, a.narg, num_ret, 0 ) != 0 )
	{
		I3ASSERT(FALSE);
	}
	
	const INT32 mstack = -num_ret;

	for ( INT32 i = 0 ; i < num_ret ; ++i)
	{
		mode_script_arg::process_ret*& elem = a.rlist[i];
		(*elem)(a, mstack, i);
		delete elem;	elem = NULL;
	}
	// 스택을 비운다..
	lua_pop(a.l, num_ret);		// 리턴값의 갯수만큼 비워주자..
	//
	a.rlist.clear();

	return a;
}

// 서버로부터 갱신받도록 함..변경시점에 그냥 콜백만 간단이 호출한다..( 이방식은 솔직이 문제가 많다고 생각...)
// 개별변수가 독립변경되는것만으로 충분하다면 모를까..하나의 패킷프로토콜이 2가지 이상의 변수변경을 요구할 경우..
// 해당 프로토콜 이벤트처리가 이 변경에 대해 인지하는 것에 대해서는 아무런 사전 준비를 할수가 없다..
//
void		IModeScript::set_modedata(PROTOCOL p, P_BATTLE_SERVER_SYNC_MODEDATA_ACK* data, const INT32 num)
{
	for (INT32 i = 0, idx = 0, value = 0 ; i < num ; ++i)
	{
		// check
		idx =	data[i]._ui8SyncDataidx;
		value = data[i]._i32SyncData;
		
		I3ASSERT( idx >= 0 && idx < MODE_DATA_MAX );
		if( idx < 0 || idx >= MODE_DATA_MAX )
		{
			I3NOTICE("MODE_DATA_MAX Index error1 : %d", idx);
			continue;
		}
		m_i32RoomModeData[ idx ] = value;
		I3TRACE("[ModeScript] [ %d ] : %d\n", idx, value);
	}
	
	//모두 저장한 다음에 콜백을 부른다..
	for (INT32 i = 0, idx = 0; i < num ; ++i)
	{
		idx =	data[i]._ui8SyncDataidx;
		if( idx < 0 || idx >= MODE_DATA_MAX )
			continue;
		std::vector<ModeDataCB*>& v = m_modeDataCtn[idx];
		const INT32 num_cb = INT32(v.size());	
		const char* mode_idx_name = m_strModeDataString[idx].c_str();

		for (INT32 j = 0 ; j < num_cb ; ++j)
		{
			(*v[j])( this, p, mode_idx_name, idx, m_i32RoomModeData[ idx ]);		// 3-4번째인수는 꼭필요한것은 아니지만, 가독성에는 좋다...
		}
	}
}

INT32		IModeScript::find_modedata_index(const char* mode_index_name)
{
	return get_global_int(mode_index_name);
}

void	IModeScript::_remove_modedata_cb(const type_info& ti, const void* vo)
{
	for (INT32 i = 0 ; i < MODE_DATA_MAX ; ++i)
	{
		std::vector<ModeDataCB*>& v = m_modeDataCtn[i];
		if (v.empty()) continue;

		std::vector<ModeDataCB*>::iterator it = v.begin();
		for ( ; it != v.end() ; )
		{
			ModeDataCB* cb = *it;
			if (cb->is_object_same( ti, vo) )
			{
				delete cb;
				it = v.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

}

void	IModeScript::_clear_modedata_cb()
{
	
	for (INT32 i = 0 ; i < MODE_DATA_MAX ; ++i)
	{
		std::vector<ModeDataCB*>& v = m_modeDataCtn[i];
		if (v.empty()) continue;

		const INT32 n = INT32(v.size());
		for (INT32 j = 0 ; j < n ; ++j)
		{
			ModeDataCB* cb = v[j];
			delete cb;
		}
		v.clear();

		m_strModeDataString[i].clear();
	}
	
}


/*
bool IModeScript::Call(const char *strFunction, const char *strFormat, ...)
{
	va_list vl;
	INT32 i32Arg, i32Ret;

	va_start(vl, strFormat);
	lua_getglobal( m_hL, strFunction );

	BOOL bIsArg = TRUE;
	for( i32Arg = 0; *strFormat; i32Arg++ )
	{
		luaL_checkstack( m_hL, 1, "too many argumets" );

		switch( *strFormat++ )
		{
		case 'd':  // double argument
			lua_pushnumber(m_hL, (lua_Number)va_arg(vl, REAL64));
			break;
		case 'b':
			lua_pushboolean( m_hL, va_arg(vl, INT32) );
			break;
		case 'f':
			lua_pushnumber( m_hL, (lua_Number)va_arg(vl, REAL32) );
			break;
		case 'i':
			lua_pushinteger( m_hL, va_arg(vl, INT32) );
			break;
		case 's':	
			lua_pushstring( m_hL, va_arg(vl, char *) );
			break;
		case '>':
			bIsArg = FALSE;
			break;
		default:
			break;
		}

		if( bIsArg == FALSE ) break;
	}

	i32Ret = strlen(strFormat);

	if( lua_pcall( m_hL, i32Arg, i32Ret, 0 ) != 0 )
	{
		I3ASSERT(FALSE);
	}

	i32Ret = -i32Ret;
	int nIndex = 0;

	while( *strFormat )
	{
		switch( *strFormat++ )
		{
		case 'd':
			if (!lua_isnumber(m_hL, i32Ret))
			{
				I3TRACE("Return type 'double' expected for function call '%s', return value #i", strFunction, nIndex);
			}
			*va_arg(vl, double *) = lua_tonumber(m_hL, i32Ret);
			break;
		case 'b':
			if( !lua_isboolean( m_hL, i32Ret ) )
			{
				I3TRACE("Return type 'bool' expected for function call '%s', return value #i", strFunction, nIndex);
			}
			*va_arg(vl, int*) =	lua_toboolean( m_hL, i32Ret );
			break;
		case 'f':
			if( !lua_isnumber( m_hL, i32Ret ) )
			{
				I3TRACE("Return type 'float' expected for function call '%s', return value #i", strFunction, nIndex);
			}
			*va_arg(vl, float *) = (float)lua_tonumber( m_hL, i32Ret);
			break;
		case 'i':
			if( !lua_isnumber( m_hL, i32Ret ) )
			{
				I3TRACE("Return type 'int' expected for function call '%s', return value #i", strFunction, nIndex);
			}
			*va_arg(vl, int *) = lua_tointeger( m_hL, i32Ret );
			break;
		case 's':
			if( !lua_isstring( m_hL, i32Ret ) )
			{
				I3TRACE("Return type 'string' expected for function call '%s', return value #i", strFunction, nIndex);
			}
			*va_arg(vl, char **) = (char *) lua_tostring( m_hL, i32Ret );
			break;
		default:
			break;
		}
		nIndex++;
		i32Ret++;
	}

	va_end(vl);

	return TRUE;
}
*/
