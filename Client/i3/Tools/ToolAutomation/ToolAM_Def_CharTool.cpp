
#include "ToolAMPCH.h"
#include "ToolAM_Def_CharTool.h"

namespace ToolAM
{
	namespace CharTool
	{

		std::string EvaluateLODFileName(const char* evalStr, const char* heroName, 
			const char* maskName, INT32 lodIndex)
		{
			char buf[1024]={0};

			if(!evalStr)
				return std::string();
			
			const char* c = evalStr;
			INT32 index = 0;

			while( *c )
			{
				if(*c == '%')
				{
					char d = *(c+1);

					if(d)
					{
						bool parsed = false;

						do 
						{
							if(d == 'h')
							{
								strcpy(&buf[index], heroName);
								index += strlen(heroName);
								c +=2;
							} 
							else if(d == 'm')
							{
								strcpy(&buf[index], maskName);
								index += strlen(maskName);
								c +=2;
							}
							else if(d == 'n')
							{
								char lodIdxBuf[12]={0};
								sprintf_s(lodIdxBuf, 12, "%d", lodIndex);

								strcpy(&buf[index], lodIdxBuf);
								index += strlen(lodIdxBuf);
								c +=2;
							}
							else
								break;
							
							parsed = true;

						} while(0);

						if(parsed)
							continue;
					}
				}

				buf[index++] = *c++;
			}

			if(index)
				return std::string(buf);

			return std::string();
		}

	} // end of namespace CharTool

} // end of namespace ToolAM
