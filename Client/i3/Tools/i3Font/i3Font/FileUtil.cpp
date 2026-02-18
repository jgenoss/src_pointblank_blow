#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

void SplitPath(const char *path, char *drive, char *dir, char *fname, char *ext )
{
	char Drive[_MAX_DRIVE];
	char Dir[_MAX_DIR];
	char Fname[_MAX_FNAME];
	char Ext[_MAX_EXT];

	_splitpath(path,Drive,Dir,Fname,Ext);
	if(drive) strcpy(drive, Drive);
	if(  dir) strcpy(  dir, Dir  );
	if(fname) strcpy(fname, Fname);
	if(  ext) strcpy(  ext, Ext  );
}

void GetFileName(char *Out, const char *In)
{
	char Ext[_MAX_EXT];
	SplitPath(In, 0, 0, Out, Ext);
	strcat(Out, Ext);
}

void GetFileNameNoExt(char* Out, const char *In)
{
	SplitPath(In, 0, 0, Out, 0);
}

void GetFileExt(char* Out, const char *In)
{
	char Ext[_MAX_EXT] = "";
	SplitPath(In, 0, 0, 0, Ext);
	if( Ext[0] == '.' )
		strcpy(Out, &Ext[1]);
	else
		strcpy(Out, Ext);
}

// Remove last slash
void CorrectDir(char *Dir)
{
	size_t pos = strlen( Dir ) - 1;
	if( Dir[pos] == '\\' || Dir[pos] == '/' )
	{
		Dir[pos] = 0;
	}
}

void GetFileDir(char *Out, const char *In, bool RemoveLastSlash)
{	
	char Dir[_MAX_DIR];
	SplitPath(In, Out, Dir, 0, 0);
	strcat(Out, Dir);
	if( RemoveLastSlash )
	{
		CorrectDir(Out);
	}
}

void AttatchFileNameTail( char *szOutputName, const char *szTail, const char *szInputName )
{
	char drive[10];
	char dir[MAX_PATH];
	char fname[MAX_PATH];
	char ext[MAX_PATH];
	_splitpath(szInputName, drive, dir, fname, ext);

	sprintf(szOutputName, "%s%s%s%s%s", drive, dir, fname, szTail, ext);
}

void ChangeFileExt( char *szOutputName, const char *szNewExt, const char *szInputName )
{
	char drive[10];
	char dir[MAX_PATH];
	char fname[MAX_PATH];
	char ext[MAX_PATH];
	_splitpath(szInputName, drive, dir, fname, ext);

	if( szNewExt[0] == '.' )
		sprintf(szOutputName, "%s%s%s%s", drive, dir, fname, szNewExt);
	else
		sprintf(szOutputName, "%s%s%s.%s", drive, dir, fname, szNewExt);
}

bool IsFileExist(const char *szPath)
{
	FILE *fp;
	fp = fopen(szPath, "rb");
	
	if(fp == NULL)
		return false;

	fclose(fp);
	return true;
}

fpos_t GetFileSizeFromPath(const char *szPath)
{
	FILE *fp;
	fp = fopen(szPath, "rb");

	if(fp == NULL)
		return 0;

	fseek(fp, 0, SEEK_END);

	fpos_t pos;
	fgetpos(fp, &pos);

	fclose(fp);

	return pos;
}

bool CopyFileLine(FILE *fpOut, FILE* fpIn, size_t NumLine, char *szBuf, int BufSize)
{
	if( NumLine == 0 )
	{
		return true;
	}

	if( NumLine < 0 )
	{
		while( !feof( fpIn ) )
		{
			if( !fgets( szBuf, BufSize, fpIn ) )
			{
				if( !feof( fpIn ) )
				{
					return false;
				}
			}
			else
			{
				if( !fwrite( szBuf, 1, strlen(szBuf), fpOut ) )
				{
					return false;
				}
			}
		}
	}
	else
	{
		size_t i;
		for( i=0; i<NumLine; i++ )
		{
			if( !fgets( szBuf, BufSize, fpIn ) )
			{
				return false;
			}

			if( !fwrite( szBuf, 1, strlen(szBuf), fpOut ) )
			{
				return false;
			}
		}
	}

	return true;
}

// 파일명에서 서브폴더 자동생성..
// d:\\Test\\Sub\\File.dat => d:\\Test\\Sub\\ 폴더 생성
void CreateSubDirectoryFromFullPath(const char *fullpath)
{
	char *fname; 
	int n;
	char path[1024];

	fname = (char*)fullpath;

	while(1)
	{    
		fname = strchr(fname, '\\');
		if (fname == NULL) break;

		n = int(fname - fullpath);

		strncpy(path, fullpath, n);
		path[n] = 0;
		
		// _mkdir(path);
		CreateDirectory(path, NULL);

		fname++;
	} 	
}
