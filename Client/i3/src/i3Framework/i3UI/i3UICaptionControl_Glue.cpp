#include "i3FrameworkPCH.h"
#include "i3UICaptionControl.h"
#include "i3Base/string/ext/utf8_to_utf16.h"


extern "C" int i3UIC_getMaxLength(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		i3Lua::PushInteger( L, pCtrl->getMaxTextCount());
	}
	
	return 1;
}

extern "C" int i3UIC_setMaxLength(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		INT32 length = i3Lua::GetIntegerArg(L, 2, 0);
		
		pCtrl->SetMaxTextCount( length);
	}

	return 0;
}

extern "C" int i3UIC_getLetterLimit(lua_State* L )
{
	i3UICaptionControl* pCtrl	= (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		i3Lua::PushInteger( L, pCtrl->getLetterLimit() );
	}

	return 1;
}

extern "C" int i3UIC_setStyle(lua_State *L)
{
	return 1;
}

extern "C" int i3UIC_getStyle(lua_State *L)
{
	return 1;
}

extern "C" int i3UIC_getString(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		const i3::const_wchar_range& str	= pCtrl->getTextRange();
		i3Lua::PushStringUTF16To8(L, str);
	}

	return 1;
}

/*
extern "C" int i3UIC_setStringImpl(lua_State *L, UINT codepage)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);

	if( pCtrl != nullptr)
	{
		char szStr[2048] = {0,};	// 좀더 큰 문자열 대비하여 버퍼사이즈를 늘림.. // [260]; (11.05.24.수빈)

		if( i3Lua::GetStringArg( L, 2, szStr, sizeof( szStr)) != nullptr)
		{
			INT32 len = i3::generic_string_size( szStr);

			if( len > 0)
			{
				WCHAR16 wStr[2048] = {0,};

				//	유니코드 문자열의 길이를 받아온다.
				INT32 length = i3String::MultiByteToWideChar( codepage, 0, szStr, len, nullptr, 0);

				//	유니코드 버퍼로 문자열을 받아온다.
				i3String::MultiByteToWideChar( codepage, 0, szStr, len, wStr, length);

				pCtrl->SetText( wStr);
			}
			else
				pCtrl->SetText( L"");
		}
		else
			pCtrl->SetText( L"");
	}

	return 0;
	
}
*/


extern "C" int i3UIC_setString(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);

	if( pCtrl != nullptr)
	{
		char szStr[2048] = {0,};	// 좀더 큰 문자열 대비하여 버퍼사이즈를 늘림.. // [260]; (11.05.24.수빈)

		if( i3Lua::GetStringArg( L, 2, szStr, sizeof( szStr)) != nullptr)
		{
			INT32 len = ::strlen( szStr);
			
			if (len)
			{
				i3::stack_wstring wstrText;
				i3::utf8_to_utf16( i3::literal_range( szStr, szStr + len), wstrText);
				pCtrl->SetText(wstrText);
			}
			else
			{
				pCtrl->SetText(L"");
			}
		}
		else
		{
			pCtrl->SetText(L"");
		}
	}
	
	return 0;

}

extern "C" int i3UIC_setStringPtr(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);

	if( pCtrl != nullptr)
	{
		const wchar_t* wszLiteral = reinterpret_cast<const wchar_t*>(i3Lua::GetUserDataArg(L, 2));	// 
		
		if (wszLiteral)
			pCtrl->SetText(wszLiteral);
		else
			pCtrl->SetText(L"");
	}

	return 0;
}


extern "C" int i3UIC_getStringLength(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		const WCHAR16*	pstr = pCtrl->getText();
		if( pstr == nullptr) return 0;

		INT32			nLenW = i3::generic_string_size( pstr);

		if( pCtrl->getLimitByteSize() )
		{
			INT32		nLenM = i3String::WideCharToMultiByte( CP_ACP, 0, pstr, nLenW, 0, 0, 0, 0 );

			i3Lua::PushInteger( L, nLenM );
		}
		else
		{
			i3Lua::PushInteger( L, nLenW );
		}
	}

	return 1;
}


extern "C" int i3UIC_getFontName(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		i3Lua::PushString(L, pCtrl->getFontName());
		return 1;
	}

	return 0;
}

extern "C" int i3UIC_setFontName(lua_State *L)
{
	char szFont[ 260];

	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);

	if( pCtrl != nullptr)
	{
		if(i3Lua::GetStringArg(L, 2, szFont, sizeof( szFont), "") != nullptr)
		{
			INT32 cLength = i3::generic_string_size( szFont );
			INT32 wLength = i3String::MultiByteToWideChar( CP_UTF8, 0, szFont, cLength, nullptr, 0);

			wchar_t *wc = (wchar_t*)alloca( sizeof( wchar_t) * (wLength + 1));

			i3String::MultiByteToWideChar( CP_UTF8, 0, szFont, cLength, wc, wLength);
			wc[wLength] = 0;

			cLength = i3String::WideCharToMultiByte( CP_ACP, 0, wc, wLength, 0, 0, 0, 0 ); 
			char * cbuf = (char*)alloca( sizeof( char) * (cLength + 1));
			cbuf[cLength] = 0;

			i3String::WideCharToMultiByte( CP_ACP, 0, wc, wLength, cbuf, cLength, 0, 0 ); // 

			pCtrl->SetFont( cbuf, pCtrl->getFontSize());
		}
	}

	return 0;
}

extern "C" int i3UIC_getFont(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		i3Lua::PushString(L, pCtrl->getFontName());
		i3Lua::PushInteger(L, pCtrl->getFontSize());
		i3Lua::PushInteger(L, pCtrl->getFontWeight());

		return 3;
	}

	return 0;
}

extern "C" int i3UIC_setFont(lua_State *L)
{
	char szFont[ 260];

	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);

	if( pCtrl != nullptr)
	{
		if( i3Lua::GetStringArg(L, 2, szFont, sizeof( szFont), "") != nullptr)
		{
			INT32 fontSize = i3Lua::GetIntegerArg(L, 3, 10);
			INT32 fontWeight = i3Lua::GetIntegerArg(L, 4, 0);

			pCtrl->SetFont( szFont, fontSize, fontWeight);
		}
	}

	return 0;
}

extern "C" int i3UIC_getFontSizePt(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		i3Lua::PushInteger(L, pCtrl->getFontSize());

		return 1;
	}

	return 0;
}

extern "C" int i3UIC_setFontSizePt(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		INT32 sizePT = i3Lua::GetIntegerArg(L, 2, 10);
		pCtrl->SetFont(pCtrl->getFontName(), sizePT);
	}
	
	return 0;
}

extern "C" int i3UIC_getFontWeight(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		i3Lua::PushInteger( L, pCtrl->getFontWeight());
		return 1;
	}

	return 0;
}


extern "C" int i3UIC_setFontWeight(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		INT32 weight = i3Lua::GetIntegerArg(L, 2, 0);
		
		pCtrl->SetFont(pCtrl->getFontName(), pCtrl->getFontSize(), weight);
	}

	return 0;
}

extern "C" int i3UIC_getTextAlign(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		i3Lua::PushInteger( L, (INT32)pCtrl->getTextAlignX());
		i3Lua::PushInteger( L, (INT32)pCtrl->getTextAlignY());

		return 2;
	}

	return 0;
}

extern "C" int i3UIC_setTextAlign(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		INT32 alignX = i3Lua::GetIntegerArg(L, 2, 0);
		INT32 alignY = i3Lua::GetIntegerArg(L, 3, 0);

		pCtrl->SetTextAlign( (I3UI_TEXT_ALIGN_X)alignX, (I3UI_TEXT_ALIGN_Y)alignY);
	}

	return 0;
}

extern "C" int i3UIC_getTextAlignX(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		i3Lua::PushInteger( L, (INT32)pCtrl->getTextAlignX());

		return 1;
	}

	return 0;
}


extern "C" int i3UIC_getTextAlignY(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		i3Lua::PushInteger( L, (INT32)pCtrl->getTextAlignY());

		return 1;
	}
	
	return 0;
}

extern "C" int i3UIC_setTextAlignX(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		INT32 alignX = i3Lua::GetIntegerArg(L, 2, 0);

		pCtrl->SetAlign( (TEXT_HORZ_ALIGN)alignX, pCtrl->getTextAlignY());
	}

	return 0;
}


extern "C" int i3UIC_setTextAlignY(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		INT32 alignY = i3Lua::GetIntegerArg(L, 2, 0);

		pCtrl->SetAlign( pCtrl->getTextAlignX(), (TEXT_VERT_ALIGN)alignY);
	}

	return 0;
}


extern "C" int i3UIC_getTextSpace(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		i3Lua::PushInteger( L, pCtrl->getTextSpaceX());
		i3Lua::PushInteger( L, pCtrl->getTextSpaceY());

		return 2;
	}

	return 0;
}

extern "C" int i3UIC_setTextSpace(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		INT32 spaceX = i3Lua::GetIntegerArg(L, 2, 0);
		INT32 spaceY = i3Lua::GetIntegerArg(L, 3, 0);

		pCtrl->SetTextSpace( spaceX, spaceY);
	}

	return 1;
}

extern "C" int i3UIC_getTextSpaceX(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		i3Lua::PushInteger( L, pCtrl->getTextSpaceX());

		return 1;
	}

	return 0;
}

extern "C" int i3UIC_setTextSpaceX(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		INT32 spaceX = i3Lua::GetIntegerArg(L, 2, 0);

		pCtrl->SetTextSpace( spaceX, pCtrl->getTextSpaceY());
	}

	return 0;
}

extern "C" int i3UIC_getTextExtentX(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		if (pCtrl->getUIText() != nullptr)
		{
			i3Lua::PushNumber(L, (REAL32) pCtrl->getUIText()->getExtentWidth() );
			return 1;
		}
	}
	return 0;
}

extern "C" int i3UIC_getTextSpaceY(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		i3Lua::PushInteger( L, pCtrl->getTextSpaceY());
		return 1;
	}

	return 0;
}

extern "C" int i3UIC_setTextSpaceY(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		INT32 spaceY = i3Lua::GetIntegerArg(L, 2, 0);

		pCtrl->SetTextSpace( pCtrl->getTextSpaceX(), spaceY);
	}

	return 0;
}

extern "C" int i3UIC_getTextColor(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		COLOR col;
		i3Color::ConvertRGBA32( pCtrl->getTextColor(), &col);
		
		i3Lua::PushInteger(L, (INT32)(col.r));
		i3Lua::PushInteger(L, (INT32)(col.g));
		i3Lua::PushInteger(L, (INT32)(col.b));
		i3Lua::PushInteger(L, (INT32)(col.a));

		return 4;
	}

	return 0;
}

extern "C" int i3UIC_setTextColor(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		COLOR col;

		INT32 r = i3Lua::GetIntegerArg(L, 2, 0);
		INT32 g = i3Lua::GetIntegerArg(L, 3, 0);
		INT32 b = i3Lua::GetIntegerArg(L, 4, 0);
		INT32 a = i3Lua::GetIntegerArg(L, 5, 0);

		i3Color::Set(&col, (UINT8)r, (UINT8)g, (UINT8)b, (UINT8)a);

		pCtrl->SetTextColor(&col);
	}

	return 0;
}

// 귀찮게 인자를 늘려가면서 하기 몹시 피곤할때가 훨씬 많으므로, 4바이트짜리 색상설정함수가 반드시 필요한 법이다. (11.06.29.수빈)

extern "C" int i3UIC_setTextColor4Byte(lua_State* L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		INT32 rgba = i3Lua::GetIntegerArg(L, 2, 0);
		COLOR col; col.SetRGBA(rgba);
		pCtrl->SetTextColor(&col);
	}
	return 0;
}



extern "C" int i3UIC_getTextShadowColor(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		COLOR col;
		i3Color::ConvertRGBA32( pCtrl->getTextShadowColor(), &col);

		i3Lua::PushInteger(L, (INT32)(col.r));
		i3Lua::PushInteger(L, (INT32)(col.g));
		i3Lua::PushInteger(L, (INT32)(col.b));
		i3Lua::PushInteger(L, (INT32)(col.a));

		return 4;
	}

	return 0;
}

extern "C" int i3UIC_setTextShadowColor(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		COLOR col;

		INT32 r = i3Lua::GetIntegerArg(L, 2, 0);
		INT32 g = i3Lua::GetIntegerArg(L, 3, 0);
		INT32 b = i3Lua::GetIntegerArg(L, 4, 0);
		INT32 a = i3Lua::GetIntegerArg(L, 5, 0);

		i3Color::Set(&col, (UINT8)r, (UINT8)g, (UINT8)b, (UINT8)a);

		pCtrl->SetTextShadowColor( i3Color::ConvertRGBA32( &col));
	}

	return 0;
}

//extern "C" int i3UIC_getTextShaodwQuality(lua_State *L)
//{
//	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
//	if( pCtrl != nullptr)
//	{
//		i3Lua::PushInteger( L, (INT32)pCtrl->getTextShadowQuality());
//		return 1;
//	}
//
//	return 0;
//}
//
//extern "C" int i3UIC_getTextShaodwQualityEnable(lua_State *L)
//{
//	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
//	if( pCtrl != nullptr)
//	{
//		i3Lua::PushInteger( L, (pCtrl->isTextShadow())? 1 : 0);
//
//		return 1;
//	}
//
//	return 0;
//}
//
//extern "C" int i3UIC_setTextShaodwQuality(lua_State *L)
//{
//	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
//	if( pCtrl != nullptr)
//	{
//		INT32 Quality = i3Lua::GetIntegerArg( L, 2, 0);
//		if( Quality < 0)
//			pCtrl->setTextShadow( false);
//		else
//			pCtrl->setTextShadow( true, (FONT_SHADOW_QUALITY)Quality);
//
//		return 0;
//	}
//
//	return 0;
//}

////////////////////////////////////////////////////////////////////////////////////

extern "C" int i3UIC_setShadowType(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		INT32 nType = i3Lua::GetIntegerArg(L, 2);
		if(nType < 3)
			pCtrl->SetTextShadowType((I3UI_TEXT_SHADOW)nType);
		else
			pCtrl->SetTextShadowType((I3UI_TEXT_SHADOW)0);
	}

	return 0;
}

extern "C" int i3UIC_setQualityType(lua_State *L)
{
	i3UICaptionControl * pCtrl = (i3UICaptionControl*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		INT32 nType = i3Lua::GetIntegerArg(L, 2);
		if(nType < 3)
			pCtrl->SetFontQuality((I3UI_FONT_QUALITY)nType);
		else
			pCtrl->SetFontQuality((I3UI_FONT_QUALITY)0);
	}

	return 0;
}


LuaFuncReg i3UICaptionControlGlue[] = 
{
	{"uiCaption_getMaxLetters",			i3UIC_getMaxLength			},
	{"uiCaption_setMaxLetters",			i3UIC_setMaxLength			},

	{"uiCaption_getLetterLimit",		i3UIC_getLetterLimit		},

	{"uiCaption_setStyle",				i3UIC_setStyle				},
	{"uiCaption_getStyle",				i3UIC_getStyle				},

	{"uiCaption_getText",				i3UIC_getString				},
	{"uiCaption_setText",				i3UIC_setString				},
	{"uiCaption_setTextPtr",			i3UIC_setStringPtr			},
	{"uiCaption_getTextLen",			i3UIC_getStringLength		},

	{"uiCaption_getFont",				i3UIC_getFont				},
	{"uiCaption_setFont",				i3UIC_setFont				},

	{"uiCaption_getFontName",			i3UIC_getFontName			},			// 수정
	{"uiCaption_setFontName",			i3UIC_setFontName			},			// 수정
	{"uiCaption_getFontSizePt",			i3UIC_getFontSizePt			},
	{"uiCaption_setFontSizePt",			i3UIC_setFontSizePt			},
	{"uiCaption_getFontWeight",			i3UIC_getFontWeight			},
	{"uiCaption_setFontWeight",			i3UIC_setFontWeight			},

	{"uiCaption_getTextAlign",			i3UIC_getTextAlign			},
	{"uiCaption_setTextAlign",			i3UIC_setTextAlign			},
	{"uiCaption_getTextAlignX",			i3UIC_getTextAlignX			},
	{"uiCaption_getTextAlignY",			i3UIC_getTextAlignY			},
	{"uiCaption_setTextAlignX",			i3UIC_setTextAlignX			},
	{"uiCaption_setTextAlignY",			i3UIC_setTextAlignY			},

	{"uiCaption_getTextSpace",			i3UIC_getTextSpace			},
	{"uiCaption_setTextSpace",			i3UIC_setTextSpace			},
	{"uiCaption_getTextSpaceX",			i3UIC_getTextSpaceX			},
	{"uiCaption_setTextSpaceX",			i3UIC_setTextSpaceX			},
	{"uiCaption_getTextSpaceY",			i3UIC_getTextSpaceY			},
	{"uiCaption_setTextSpaceY",			i3UIC_setTextSpaceY			},

	{"uiCaption_getTextColor",			i3UIC_getTextColor			},
	{"uiCaption_setTextColor",			i3UIC_setTextColor			},
	{"uiCaption_setTextColor4Byte",		i3UIC_setTextColor4Byte		},
	{"uiCaption_getTextShadowColor",	i3UIC_getTextShadowColor	},
	{"uiCaption_setTextShadowColor",	i3UIC_setTextShadowColor	},

	/*{"uiCaption_getTextShaodwQuality",	i3UIC_getTextShaodwQuality	},
	{"uiCaption_setTextShaodwQuality",	i3UIC_setTextShaodwQuality	},
	{"uiCaption_getTextShaodwQualityEnable",	i3UIC_getTextShaodwQualityEnable	},*/

	{"uiCaption_setShadowType",			i3UIC_setShadowType			},
	{"uiCaption_setQualityType",		i3UIC_setQualityType		},
	{"uiCaption_getTextExtentX",		i3UIC_getTextExtentX		},

	{nullptr, nullptr}
};

void i3UICaptionControl::RegisterLuaFunction(LuaState * pState)
{
	if( pState == nullptr)
		return;

	for(int i=0; i3UICaptionControlGlue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3UICaptionControlGlue[i].name, i3UICaptionControlGlue[i].func);
	}
}