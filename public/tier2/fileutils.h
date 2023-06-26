//===== Copyright © 2005-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: A higher level link library for general use in the game and tools.
//
//===========================================================================//


#ifndef FILEUTILS_H
#define FILEUTILS_H

#if defined( _WIN32 )
#pragma once
#endif

#ifndef TIER2_H
#include "tier2/tier2.h"
#endif

#ifndef FILESYSTEM_H
#include "filesystem.h"
#endif

#include "tier0/platform.h"

// Builds a directory which is a subdirectory of the current mod
void GetModSubdirectory( const char *pSubDir, char *pBuf, int nBufLen );

// Builds a directory which is a subdirectory of the current mod's *content*
void GetModContentSubdirectory( const char *pSubDir, char *pBuf, int nBufLen );

// Builds a list of all files under a directory with a particular extension
void AddFilesToList( CUtlVector< CUtlString > &list, const char *pDirectory, const char *pPath, const char *pExtension );

// Returns the search path as a list of paths
void GetSearchPath( CUtlVector< CUtlString > &path, const char *pPathID );

// Generates a .360 file if it doesn't exist or is out of sync with the pc source file
#define UOC_FAIL		-1
#define UOC_NOT_CREATED	0
#define UOC_CREATED		1
typedef bool ( *CreateCallback_t )( const char *pSourceName, const char *pTargetName, const char *pPathID, void *pExtraData );
int UpdateOrCreate( const char *pSourceName, char *pTargetName, int targetLen, const char *pPathID, CreateCallback_t pfnCreate, bool bForce = false, void *pExtraData = NULL );

char *CreateX360Filename( const char *pSourceName, char *pTargetName, int targetLen );

// simple file classes. File I/O mode (text/binary, read/write) is based upon the subclass chosen.
// classes with the word Required on them abort with a message if the file can't be opened.
// destructores close the file handle, or it can be explicitly closed with the Close() method.

class CBaseFile
{
public:
	FileHandle_t m_FileHandle;

	CBaseFile(void)
	{
		m_FileHandle = FILESYSTEM_INVALID_HANDLE;
	}

	~CBaseFile( void )
	{
		Close();
	}

	void Close( void )
	{
		if ( m_FileHandle != FILESYSTEM_INVALID_HANDLE )
			g_pFullFileSystem->Close( m_FileHandle );
		m_FileHandle = FILESYSTEM_INVALID_HANDLE;
	}

	void Open( char const *fname, char const *modes )
	{
		Close();
		m_FileHandle = g_pFullFileSystem->Open( fname, modes );
	}

	char *ReadLine( char *pOutput, int maxChars )
	{
		return g_pFullFileSystem->ReadLine( pOutput, maxChars, m_FileHandle );
	}

	int Read( void* pOutput, int size )
	{
		return g_pFullFileSystem->Read( pOutput, size, m_FileHandle );
	}

	void MustRead( void* pOutput, int size )
	{
		int ret=Read( pOutput, size );
		if (ret != size )
			Error("failed to read %d bytes\n");
	}
	
	int Write( void const* pInput, int size)
	{
		return g_pFullFileSystem->Write( pInput, size, m_FileHandle );
	}


	// {Get|Put}{Int|Float} read and write ints and floats from a file in x86 order, swapping on
	// input for big-endian systems.
	void PutInt( int n )
	{
		int n1=LittleDWord( n );
		Write(&n1, sizeof( n1 ) );
	}

	int GetInt( void )
	{
		int ret;
		MustRead( &ret, sizeof( ret ));
		return LittleDWord( ret );
	}

	float GetFloat( void )
	{
		float ret;
		MustRead( &ret, sizeof( ret ));
		LittleFloat( &ret, &ret );
		return ret;
	}
	void PutFloat( float f )
	{
		LittleFloat( &f, &f );
		Write( &f, sizeof( f ) );
	}

	bool IsOk( void )
	{
		return ( m_FileHandle != FILESYSTEM_INVALID_HANDLE) &&
			( g_pFullFileSystem->IsOk( m_FileHandle ) );
	}
};

class COutputFile : public CBaseFile
{
public:
	void Open( char const *pFname )
	{
		CBaseFile::Open( pFname, "wb" );
	}

	COutputFile( char const *pFname ) : CBaseFile()
	{
		Open( pFname );
	}

	COutputFile( void ) : CBaseFile()
	{
	}
};

class COutputTextFile : public CBaseFile
{
public:
	void Open( char const *pFname )
	{
		CBaseFile::Open( pFname, "w" );
	}

	COutputTextFile( char const *pFname ) : CBaseFile()
	{
		Open( pFname );
	}

	COutputTextFile( void ) : CBaseFile()
	{
	}
};

class CInputFile : public CBaseFile
{
public:
	void Open( char const *pFname )
	{
		CBaseFile::Open( pFname, "rb" );
	}

	CInputFile( char const *pFname ) : CBaseFile()
	{
		Open( pFname );
	}
	CInputFile( void ) : CBaseFile()
	{
	}
};

class CInputTextFile : public CBaseFile
{
public:
	void Open( char const *pFname )
	{
		CBaseFile::Open( pFname, "r" );
	}

	CInputTextFile( char const *pFname ) : CBaseFile()
	{
		Open( pFname );
	}
	CInputTextFile( void ) : CBaseFile()
	{
	}
};

class CRequiredInputTextFile : public CBaseFile
{
public:
	void Open( char const *pFname )
	{
		CBaseFile::Open( pFname, "r" );
		if ( ! IsOk() )
		{
			Error("error opening required file %s\n", pFname );
		}
	}

	CRequiredInputTextFile( char const *pFname ) : CBaseFile()
	{
		Open( pFname );
	}
	CRequiredInputTextFile( void ) : CBaseFile()
	{
	}
};

class CRequiredInputFile : public CBaseFile
{
public:
	void Open( char const *pFname )
	{
		CBaseFile::Open( pFname, "rb" );
		if ( ! IsOk() )
		{
			Error("error opening required file %s\n", pFname );
		}
	}

	CRequiredInputFile( char const *pFname ) : CBaseFile()
	{
		Open( pFname );
	}
	CRequiredInputFile( void ) : CBaseFile()
	{
	}
};

#endif // FILEUTILS_H

