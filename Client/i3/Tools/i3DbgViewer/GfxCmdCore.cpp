#include "stdafx.h"
#include "GfxCmdCore.h"

#if 0
#define		LOG			I3TRACE
#else
#define		LOG(...)	__noop
#endif

GfxCmdCore::GfxCmdCore(void)
{
	m_hFont = i3TDK::CreateDefFont( 10, "Tahoma", 0);
}

GfxCmdCore::~GfxCmdCore(void)
{
	if( m_hFont != NULL)
	{
		::DeleteObject( m_hFont);
		m_hFont = NULL;
	}
}

char * _lexToken( char * pszStr, char * pszToken)
{
	// skip white space;
	while( *pszStr)
	{
		if( (*pszStr == '\n') ||
			(*pszStr == '\t') ||
			(*pszStr == '\r') ||
			(*pszStr == ' '))
		{
			pszStr++;
		}
		else
		{
			break;
		}
	}

	while( *pszStr)
	{
		switch( *pszStr)
		{
			case '\n' :
			case '\t' :
			case '\r' :
			case ' ' :
				*pszToken = 0;
				return pszStr + 1;

			default :
				*pszToken = *pszStr;
				*pszToken++;
				break;
		}

		pszStr++;
	}

	*pszToken = 0;

	return pszStr;
}

void	GfxCmdCore::_getStr( char * pszToken)
{
	m_pszCur = _lexToken( m_pszCur, pszToken);
}

void	GfxCmdCore::_getStrLine( char * pszStr)
{
	while( *m_pszCur)
	{
		if( *m_pszCur == '\n')
		{
			*pszStr = 0;
			return;
		}
		else
		{
			*pszStr = *m_pszCur;
			pszStr++;
		}
		m_pszCur++;
	}

	*pszStr = 0;
}

INT32	GfxCmdCore::_getInt(void)
{
	char conv[256];

	m_pszCur = _lexToken( m_pszCur, conv);

	return atoi( conv);
}

REAL32	GfxCmdCore::_getReal(void)
{
	char conv[256];

	m_pszCur = _lexToken( m_pszCur, conv);

	return (REAL32) atof( conv);
}

bool	GfxCmdCore::_getBool(void)
{
	char conv[256];

	m_pszCur = _lexToken( m_pszCur, conv);

	return atoi( conv) != 0;
}

bool GfxCmdCore::_ParseLog( i3Stream * pOut, char * pBuff, VEC3D * pMin, VEC3D * pMax)
{
	char szStr[ 256], cmd;
	bool bExit = false;
	INT32 i, j;
	VEC3D	vec;
	bool bFirst = true;
	
	m_pszCur = pBuff;

	while( bExit == false)
	{
		_getStr( szStr);

		switch( szStr[0])
		{
			case 0 :		// End of drawing
				cmd = GCMD_END;
				pOut->Write( &cmd, 1);
				bExit = true;
				break;

			case 'p' :
			case 'P' :		// Draw Points		( 'p' <count> )
				{
					cmd = GCMD_DRAWPOINT;
					pOut->Write( &cmd, 1);

					CMD_DRAWPOINT data;
					data._count = _getInt();
					data._size = 1.0f;

					LOG( "POINT : %d, %f\n", data._count, data._size);

					pOut->Write( &data, sizeof(data));

					for( i = 0; i < (INT32) data._count; i++)
					{
						vec.x = _getReal();
						vec.y = _getReal();
						vec.z = _getReal();

						LOG( "\t%f %f %f\n", vec.x, vec.y, vec.z);

						if( bFirst)
						{
							i3Vector::Copy( pMin, &vec);
							i3Vector::Copy( pMax, &vec);
							bFirst = false;
						}
						else
						{
							i3Vector::Minimize( pMin, pMin, &vec);
							i3Vector::Maximize( pMax, pMax, &vec);
						}

						pOut->Write( &vec, sizeof(vec));
					}
				}
				break;

			case 'l' :
			case 'L' :		// Draw Lines		( 'l' <count> )
				{
					cmd = GCMD_DRAWLINE;
					pOut->Write( &cmd, 1);

					CMD_DRAWLINE data;
					data._count = _getInt();

					LOG( "LINE %d\n", data._count);

					pOut->Write( &data, sizeof(data));

					for( i = 0; i < (INT32) data._count; i++)
					{
						for( j = 0; j < 2; j++)
						{
							vec.x = _getReal();
							vec.y = _getReal();
							vec.z = _getReal();

							LOG( "\t%f %f %f\n", vec.x, vec.y, vec.z);

							if( bFirst)
							{
								i3Vector::Copy( pMin, &vec);
								i3Vector::Copy( pMax, &vec);
								bFirst = false;
							}
							else
							{
								i3Vector::Minimize( pMin, pMin, &vec);
								i3Vector::Maximize( pMax, pMax, &vec);
							}

							pOut->Write( &vec, sizeof(vec));
						}
					}
				}
				break;

			case 't' :
			case 'T' :		// Draw Triangles	( 't' <count> <line_boolean> <normal_boolean> )
				{
					cmd = GCMD_DRAWTRI;
					pOut->Write( &cmd, 1);

					CMD_DRAWTRI data;
					VEC3D		* pPos;

					data._count = _getInt();
					data._flag = 0;
					
					if( _getBool())		data._flag |= DRAWTRI_FLAG_LINE;
					if( _getBool())		data._flag |= DRAWTRI_FLAG_NORMAL;

					pOut->Write( &data, sizeof(data));

					LOG( "TRI %d\n", data._count);

					pPos = new VEC3D[ data._count * 3];

					for( i = 0; i < (INT32) data._count; i++)
					{
						for( j = 0; j < 3; j++)
						{
							vec.x = _getReal();
							vec.y = _getReal();
							vec.z = _getReal();

							LOG( "\t%f %f %f\n", vec.x, vec.y, vec.z);

							if( bFirst)
							{
								i3Vector::Copy( pMin, &vec);
								i3Vector::Copy( pMax, &vec);
								bFirst = false;
							}
							else
							{
								i3Vector::Minimize( pMin, pMin, &vec);
								i3Vector::Maximize( pMax, pMax, &vec);
							}

							pOut->Write( &vec, sizeof(vec));

							i3Vector::Copy( &pPos[ (i * 3) + j], &vec);
						}
					}

					if( data._flag & DRAWTRI_FLAG_LINE)
					{
						cmd = GCMD_DRAWLINE;
						CMD_DRAWLINE lineData;

						lineData._count = data._count * 3;
						
						pOut->Write( &cmd, 1);
						pOut->Write( &lineData, sizeof( lineData));

						for( i = 0; i < (INT32) data._count; i++)
						{
							pOut->Write( &pPos[ (i * 3)], sizeof(VEC3D));
							pOut->Write( &pPos[ (i * 3) + 1], sizeof(VEC3D));

							pOut->Write( &pPos[ (i * 3) + 1], sizeof(VEC3D));
							pOut->Write( &pPos[ (i * 3) + 2], sizeof(VEC3D));

							pOut->Write( &pPos[ (i * 3) + 2], sizeof(VEC3D));
							pOut->Write( &pPos[ (i * 3)], sizeof(VEC3D));
						}
					}

					delete pPos;
				}
				break;

			case 's' :
			case 'S' :		// Text				( 's' <x> <y> <z> Sample string...)
				{
					char szTemp[512];

					cmd = GCMD_TEXT;
					pOut->Write( &cmd, 1);

					CMD_TEXT data;
					data._x = _getReal();
					data._y = _getReal();
					data._z = _getReal();

					_getStrLine( szTemp);
					data._count = strlen( szTemp);
					
					pOut->Write( &data, sizeof(data));

					pOut->Write( szTemp, data._count);

					LOG( "TEXT %f %f %f  - %s\n", data._x, data._y, data._z, szTemp);
				}
				break;

			case 'c' :
			case 'C' :		// Color			( 'c' <r> <g> <b> <a>)
				{
					cmd = GCMD_COLOR;
					pOut->Write( &cmd, 1);

					CMD_COLOR data;
					UINT8	r, g, b, a;
					r = (UINT8) (_getInt() & 0xFF);
					g = (UINT8) (_getInt() & 0xFF);
					b = (UINT8) (_getInt() & 0xFF);
					a = (UINT8) (_getInt() & 0xFF);

					i3Color::Set( &data._color, r, g, b, a);

					LOG( "COLOR %d %d %d %d\n", r, g, b, a);

					pOut->Write( &data, sizeof(data));
				}
				break;
		}
	}

	return true;
}

void GfxCmdCore::_Draw( i3RenderContext * pCtx, char * pCmd)
{
	i3RenderTarget * pTarget = pCtx->GetRenderTarget(0);
	char cmd = GCMD_START;

	if( (pCmd != NULL) && pCtx->BeginRender())
	{
		// No-Lighting
		pCtx->SetLightingEnable( FALSE);
		pCtx->SetTextureEnable( I3G_TEXTURE_BIND_DIFFUSE, FALSE);
		pCtx->SetSrcBlend( I3G_BLEND_SRCALPHA);
		pCtx->SetDestBlend( I3G_BLEND_ONE);
		pCtx->SetFaceCullMode( I3G_FACE_CULL_BACK);

		while( cmd != GCMD_END)
		{
			cmd = *pCmd;		pCmd++;

			switch( cmd)
			{
				case GCMD_DRAWPOINT :
					{
						CMD_DRAWPOINT * pData = (CMD_DRAWPOINT *) pCmd;

						pCtx->DrawPrimUP( I3G_PRIM_POINTLIST, pData->_count, (void *)(pData + 1), sizeof(VEC3D));

						pCmd += sizeof(CMD_DRAWPOINT) + (sizeof(VEC3D) * pData->_count);
					}
					break;

				case GCMD_DRAWLINE :
					{
						CMD_DRAWLINE * pData = (CMD_DRAWLINE *) pCmd;

						pCtx->DrawPrimUP( I3G_PRIM_LINELIST, pData->_count, (void *)(pData + 1), sizeof(VEC3D));

						pCmd += sizeof(CMD_DRAWLINE) + (sizeof(VEC3D) * pData->_count * 2);
					}
					break;

				case GCMD_DRAWTRI :
					{
						CMD_DRAWTRI * pData = (CMD_DRAWTRI *) pCmd;

						pCtx->DrawPrimUP( I3G_PRIM_TRILIST, pData->_count, (void *)(pData + 1), sizeof(VEC3D));

						pCmd += sizeof(CMD_DRAWTRI) + (sizeof(VEC3D) * pData->_count * 3);
					}
					break;

				case GCMD_TEXT :
					{
						CMD_TEXT * pData = (CMD_TEXT *) pCmd;

						pCmd += sizeof(CMD_TEXT) + pData->_count;
					}
					break;

				case GCMD_COLOR :
					{
						CMD_COLOR * pData = (CMD_COLOR *) pCmd;

						pCtx->SetColor( &pData->_color);

						if( pData->_color.a < 255)
						{
							pCtx->SetBlendEnable( TRUE);
						}
						else
						{
							pCtx->SetBlendEnable( FALSE);
						}
						
						pCmd += sizeof(CMD_COLOR);
					}
					break;
			}
		}

		pCtx->EndRender();
	}
}

void GfxCmdCore::_DrawText( i3RenderContext * pCtx, HWND hwnd, char * pCmd)
{
	char cmd = GCMD_START;
	HDC dc;
	i3RenderTarget * pTarget = pCtx->GetRenderTarget( 0);

	if( pCmd != NULL)
	{
		dc = ::GetDC( hwnd);

		SelectObject( dc, m_hFont);
		::SetBkMode( dc, TRANSPARENT);
		::SetTextColor( dc, RGB( 255, 255, 255));

		while( cmd != GCMD_END)
		{
			cmd = *pCmd;		pCmd++;

			switch( cmd)
			{
				case GCMD_DRAWPOINT :
					{
						CMD_DRAWPOINT * pData = (CMD_DRAWPOINT *) pCmd;

						pCmd += sizeof(CMD_DRAWPOINT) + (sizeof(VEC3D) * pData->_count);
					}
					break;

				case GCMD_DRAWLINE :
					{
						CMD_DRAWLINE * pData = (CMD_DRAWLINE *) pCmd;

						pCmd += sizeof(CMD_DRAWLINE) + (sizeof(VEC3D) * pData->_count * 2);
					}
					break;

				case GCMD_DRAWTRI :
					{
						CMD_DRAWTRI * pData = (CMD_DRAWTRI *) pCmd;

						pCmd += sizeof(CMD_DRAWTRI) + (sizeof(VEC3D) * pData->_count * 3);
					}
					break;

				case GCMD_TEXT :
					{
						CMD_TEXT * pData = (CMD_TEXT *) pCmd;
						VEC3D vec, scale;
						INT32 _x, _y;
						
						vec.x = pData->_x;
						vec.y = pData->_y;
						vec.z = pData->_z;

						i3Vector::TransformCoord( &vec, &vec, pCtx->GetViewMatrix());
						i3Vector::TransformCoord( &vec, &vec, pCtx->GetProjectMatrix());

						if((vec.z > 0.0f) && (vec.z < 1.0f))
						{
							i3Vector::Set( &scale, 0.5f, -0.5f, 1.0f);
							i3Vector::Mul( &vec, &vec, &scale);
							vec.x += 0.5f;
							vec.y += 0.5f;
							
							_x = (INT32) (vec.x * pTarget->GetWidth());
							_y = (INT32) (vec.y * pTarget->GetHeight());

							::TextOut( dc, _x, _y, (LPCTSTR) (pData + 1), pData->_count);
						}

						pCmd += sizeof(CMD_TEXT) + pData->_count;
					}
					break;

				case GCMD_COLOR :
					{
						CMD_COLOR * pData = (CMD_COLOR *) pCmd;
						
						pCmd += sizeof(CMD_COLOR);
					}
					break;
			}
		}

		::ReleaseDC( hwnd, dc);
	}
}
