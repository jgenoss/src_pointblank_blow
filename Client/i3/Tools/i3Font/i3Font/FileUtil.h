#pragma once

void SplitPath(const char *path, char *drive, char *dir, char *fname, char *ext );

void GetFileName(char *Out, const char *In);

void GetFileNameNoExt(char* Out, const char *In);

void GetFileExt(char* Out, const char *In);

void GetFileDir(char *Out, const char *In, bool RemoveLastSlash = true);

void CorrectDir(char *Dir);

void AttatchFileNameTail( char *szOutputName, const char *szTail, const char *szInputName );

void ChangeFileExt( char *szOutputName, const char *szNewExt, const char *szInputName );

bool IsFileExist(const char *szPath);

fpos_t GetFileSizeFromPath(const char *szPath);

bool CopyFileLine(FILE *fpOut, FILE* fpIn, size_t NumLine, char *szBuf, int BufSize);

void CreateSubDirectoryFromFullPath(const char *fullpath);
