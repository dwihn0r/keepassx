/***************************************************************************
 *   Implementation of the SHA-1 algorithm                                 *
 *   by Dominik Reichl <dominik.reichl@t-online.de> (no copyright)         *
 *                                                                         *
 *   Copyright (C) 2007 by Tarek Saidi                                     *
 *   tarek.saidi@arcor.de                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


/*
	Version 1.5 - 2005-01-01
	- 64-bit compiler compatibility added
	- Made variable wiping optional (define SHA1_WIPE_VARIABLES)
	- Removed unnecessary variable initializations
	- ROL32 improvement for the Microsoft compiler (using _rotl)

	======== Test Vectors (from FIPS PUB 180-1) ========

	SHA1("abc") =
		A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D

	SHA1("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") =
		84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1

	SHA1(A million repetitions of "a") =
		34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/

#ifndef ___SHA1_HDR___
#define ___SHA1_HDR___

#include <stdio.h>  // Needed for file access
#include <memory.h> // Needed for memset and memcpy
#include <string.h> // Needed for strcat and strcpy


// If you're compiling big endian, just comment out the following line


// #define or #undef this, if you want the CSHA1 class to wipe all
// temporary variables after processing
#define SHA1_WIPE_VARIABLES

/////////////////////////////////////////////////////////////////////////////
// Define 8- and 32-bit variables

#ifndef quint_32
 #define quint_8 unsigned char
 #if (ULONG_MAX == 0xFFFFFFFF)
  #define quint_32 unsigned long
 #else
  #define quint_32 unsigned int
 #endif
#endif


/////////////////////////////////////////////////////////////////////////////
// Declare SHA1 workspace

typedef union
{
	quint_8  c[64];
	quint_32 l[16];
} SHA1_WORKSPACE_BLOCK;

class CSHA1
{
public:
	// Two different formats for ReportHash(...)
	enum
	{
		REPORT_HEX = 0,
		REPORT_DIGIT = 1
	};

	// Constructor and Destructor
	CSHA1();
	~CSHA1();

	quint_32 m_state[5];
	quint_32 m_count[2];
	quint_8  m_buffer[64];
	quint_8  m_digest[20];

	void Reset();

	// Update the hash value
	void Update(unsigned char* data, int len);
	bool HashFile(char *szFileName);

	// Finalize hash and report
	void Final();
	void ReportHash(char *szReport, unsigned char uReportType = REPORT_HEX);
	void GetHash(quint_8 *puDest);

private:
	// Private SHA-1 transformation
	void Transform(quint_32 *state, quint_8 *buffer);

	// Member variables
	quint_8 m_workspace[64];
	SHA1_WORKSPACE_BLOCK *m_block; // SHA1 pointer to the byte array above
};

#endif
