#include "task.h"

VOID Task :: setFlags (INT flags)
{
	this->flags = flags;
}

INT Task :: getFlags (VOID)
{
	return this->flags;
} 

VOID Task :: setOpCode (INT opCode)
{
	this->opCode = opCode;
}


INT Task :: getOpCode (VOID)
{
	return opCode;
}


TCHAR* Task :: getDstFolder (VOID)
{
	return this->dstFolder;
}

Task :: Task (DWORD pathMode)
{
	this->index = 0;
	this->curDstPath = (TCHAR*) fmAlloc (FM_MAX_PATH);
	this->curSrcPath = (TCHAR*) fmAlloc (FM_MAX_PATH);
	this->dstFolder  = (TCHAR*) fmAlloc (FM_MAX_PATH);
	this->srcFolder  = (TCHAR*) fmAlloc (FM_MAX_PATH);
	this->pathMode = pathMode;
	this->flags = 0;
}


VOID Task :: addFileName (TCHAR* path)
{	
	SIZE_T strLen = lstrlen (path) * sizeof (TCHAR) + sizeof (L'\0');
	TCHAR* pathSrc = (TCHAR*) fmAlloc (strLen); 
	lstrcpy (pathSrc, path);
	this->fileNames.push_back (pathSrc);
}


DWORD Task :: getFileNamesCount (VOID)
{
	return (DWORD) this->fileNames.size();
}

VOID Task :: setDstFolder (TCHAR* path)
{
	lstrcpy (this->dstFolder, path);
}

VOID Task :: setSrcFolder (TCHAR* path)
{
	lstrcpy (this->srcFolder, path);
}

TCHAR* Task :: getSrcFolder (VOID)
{
	return this->srcFolder;
}

TCHAR* Task :: getCurDstPath (VOID)
{
	return this->curDstPath;
}

TCHAR* Task :: getCurSrcPath (VOID)
{
	switch (this->pathMode)
	{
		case TPM_DEFAULT:
			return this->curSrcPath;
		case TPM_DROP:
			return this->fileNames [index - 1];
	}
	return NULL;
}

BOOL Task :: nextPath (VOID)
{
	if (this->index < this->fileNames.size())
	{
		wsprintf(this->curDstPath, L"%s%s", this->dstFolder, this->fileNames[index]);
		wsprintf(this->curSrcPath, L"%s%s", this->srcFolder, this->fileNames[index]);
		++ this->index;
		return TRUE;
	}
	return FALSE;
}

Task :: ~ Task ()
{
	this->clear ();
	fmFree ((HLOCAL*)&this->dstFolder);
	fmFree ((HLOCAL*)&this->srcFolder);
	fmFree ((HLOCAL*)&this->curDstPath);
	fmFree ((HLOCAL*)&this->curSrcPath);
}

VOID Task :: clear (VOID)
{
	SIZE_T count = this->fileNames.size();
	for (DWORD i = 0; i < count; ++ i)
	{
		fmFree ((HLOCAL*)&this->fileNames [i]);
		this->fileNames [i] = NULL;
	}
	this->index = 0;
	this->fileNames.clear();	
}



Progress* Task :: getProgress (VOID)
{
	return &this->progress;
}

void Task :: resetPath (VOID)
{
	this->index = 0;
}