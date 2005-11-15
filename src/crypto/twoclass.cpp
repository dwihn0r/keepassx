/*
  Copyright (c) 2003/2004, Dominik Reichl <dominik.reichl@t-online.de>
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer. 
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  - Neither the name of ReichlSoft nor the names of its contributors may be
    used to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#include <qstring.h>
#include "twoclass.h"

static bool g_bInitialized = false;

CTwofish::CTwofish()
{
}

CTwofish::~CTwofish()
{
}

bool CTwofish::init(Q_UINT8 *pKey, unsigned long uKeyLen, Q_UINT8 *initVector)
{
	//ASSERT(pKey != NULL);
	if(pKey == NULL) return false;
	//ASSERT(uKeyLen != 0);
	if(uKeyLen == 0) return false;

	if(g_bInitialized == false)
	{
		Twofish_initialise();
		g_bInitialized = true;
	}

	Twofish_prepare_key((Twofish_Byte *)pKey, uKeyLen, &m_key);

	if(initVector != NULL) memcpy(m_pInitVector, initVector, 16);
	else memset(m_pInitVector, 0, 16);

	return true;
}

int CTwofish::padEncrypt(Q_UINT8 *pInput, int nInputOctets, Q_UINT8 *pOutBuffer)
{
	int i, numBlocks, padLen;
	Q_UINT8 block[16], *iv;

	//ASSERT((pInput != NULL) && (nInputOctets != NULL) && (pOutBuffer != NULL));
	if((pInput == NULL) || (nInputOctets <= 0) || (pOutBuffer == NULL)) return 0;

	numBlocks = nInputOctets / 16;

	iv = m_pInitVector;
	for(i = numBlocks; i > 0; i--)
	{
		((Q_UINT32*)block)[0] = ((Q_UINT32*)pInput)[0] ^ ((Q_UINT32*)iv)[0];
		((Q_UINT32*)block)[1] = ((Q_UINT32*)pInput)[1] ^ ((Q_UINT32*)iv)[1];
		((Q_UINT32*)block)[2] = ((Q_UINT32*)pInput)[2] ^ ((Q_UINT32*)iv)[2];
		((Q_UINT32*)block)[3] = ((Q_UINT32*)pInput)[3] ^ ((Q_UINT32*)iv)[3];

		Twofish_encrypt(&m_key, (Twofish_Byte *)block, (Twofish_Byte *)pOutBuffer);

		iv = pOutBuffer;
		pInput += 16;
		pOutBuffer += 16;
	}

	padLen = 16 - (nInputOctets - (16 * numBlocks));

	for (i = 0; i < 16 - padLen; i++)
	{
		block[i] = (Q_UINT8)(pInput[i] ^ iv[i]);
	}

	for (i = 16 - padLen; i < 16; i++)
	{
		block[i] = (Q_UINT8)((Q_UINT8)padLen ^ iv[i]);
	}

	Twofish_encrypt(&m_key, (Twofish_Byte *)block, (Twofish_Byte *)pOutBuffer);

	return 16 * (numBlocks + 1);
}

int CTwofish::padDecrypt(Q_UINT8 *pInput, int nInputOctets, Q_UINT8 *pOutBuffer)
{
	int i, numBlocks, padLen;
	Q_UINT8 block[16];
	Q_UINT32 iv[4];

	//ASSERT((pInput != NULL) && (nInputOctets != NULL) && (pOutBuffer != NULL));
	if((pInput == NULL) || (nInputOctets <= 0) || (pOutBuffer == NULL)) return 0;

	if((nInputOctets % 16) != 0) { /*ASSERT(FALSE);*/ return -1; }

	numBlocks = nInputOctets / 16;

	memcpy(iv, m_pInitVector, 16);

	for(i = numBlocks - 1; i > 0; i--)
	{
		Twofish_decrypt(&m_key, (Twofish_Byte *)pInput, (Twofish_Byte *)block);
		((Q_UINT32*)block)[0] ^= iv[0];
		((Q_UINT32*)block)[1] ^= iv[1];
		((Q_UINT32*)block)[2] ^= iv[2];
		((Q_UINT32*)block)[3] ^= iv[3];
		memcpy(iv, pInput, 16);
		memcpy(pOutBuffer, block, 16);
		pInput += 16;
		pOutBuffer += 16;
	}

	Twofish_decrypt(&m_key, (Twofish_Byte *)pInput, (Twofish_Byte *)block);
	((Q_UINT32*)block)[0] ^= iv[0];
	((Q_UINT32*)block)[1] ^= iv[1];
	((Q_UINT32*)block)[2] ^= iv[2];
	((Q_UINT32*)block)[3] ^= iv[3];
	padLen = block[15];
	if(padLen <= 0 || padLen > 16) return -1;
	for(i = 16 - padLen; i < 16; i++)
	{
		if(block[i] != padLen) return -1;
	}
	memcpy(pOutBuffer, block, 16 - padLen);

	return 16*numBlocks - padLen;
}
