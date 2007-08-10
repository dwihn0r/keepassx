 /**************************************************************************
 *                                                                         *
 *   Copyright (C) 2007 by Tarek Saidi <tarek.saidi@arcor.de>              *
 *   Copyright (c) 2003 Dr Brian Gladman, Worcester, UK                    *
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

 This file contains the definitions required to use AES (Rijndael) in C++.
*/

#ifndef _AESCPP_H
#define _AESCPP_H

#include "aes.h"

#if defined( AES_ENCRYPT )

class AESencrypt
{
public:
    aes_encrypt_ctx cx[1];
    AESencrypt(void) { gen_tabs(); };
#ifdef  AES_128
    AESencrypt(const unsigned char key[])
        {   aes_encrypt_key128(key, cx); }
    aes_rval key128(const unsigned char key[])
        {   return aes_encrypt_key128(key, cx); }
#endif
#ifdef  AES_192
    aes_rval key192(const unsigned char key[])
        {   return aes_encrypt_key192(key, cx); }
#endif
#ifdef  AES_256
    aes_rval key256(const unsigned char key[])
        {   return aes_encrypt_key256(key, cx); }
#endif
#ifdef  AES_VAR
    aes_rval key(const unsigned char key[], int key_len)
        {   return aes_encrypt_key(key, key_len, cx); }
#endif
    aes_rval encrypt(const unsigned char in[], unsigned char out[]) const
        {   return aes_encrypt(in, out, cx);  }
#ifndef AES_MODES
    aes_rval ecb_encrypt(const unsigned char in[], unsigned char out[], int nb) const
        {   while(nb--)
            {   aes_encrypt(in, out, cx), in += AES_BLOCK_SIZE, out += AES_BLOCK_SIZE; }
        }
#endif
#ifdef AES_MODES
    aes_rval mode_reset(void)   { return aes_mode_reset(cx); }

    aes_rval ecb_encrypt(const unsigned char in[], unsigned char out[], int nb) const
        {   return aes_ecb_encrypt(in, out, nb, cx);  }

    aes_rval cbc_encrypt(const unsigned char in[], unsigned char out[], int nb, 
                                    unsigned char iv[]) const
        {   return aes_cbc_encrypt(in, out, nb, iv, cx);  }

    aes_rval cfb_encrypt(const unsigned char in[], unsigned char out[], int nb, 
                                    unsigned char iv[])
        {   return aes_cfb_encrypt(in, out, nb, iv, cx);  }

    aes_rval cfb_decrypt(const unsigned char in[], unsigned char out[], int nb, 
                                    unsigned char iv[])
        {   return aes_cfb_decrypt(in, out, nb, iv, cx);  }

    aes_rval ofb_crypt(const unsigned char in[], unsigned char out[], int nb, 
                                    unsigned char iv[])
        {   return aes_ofb_crypt(in, out, nb, iv, cx);  }

    typedef void ctr_fn(unsigned char ctr[]);

    aes_rval ctr_crypt(const unsigned char in[], unsigned char out[], int nb, 
                                    unsigned char iv[], ctr_fn cf)
        {   return aes_ctr_crypt(in, out, nb, iv, cf, cx);  }

#endif

};

#endif

#if defined( AES_DECRYPT )

class AESdecrypt
{
public:
    aes_decrypt_ctx cx[1];
    AESdecrypt(void) { gen_tabs(); };
#ifdef  AES_128
    AESdecrypt(const unsigned char key[])
            { aes_decrypt_key128(key, cx); }
    aes_rval key128(const unsigned char key[])
            { return aes_decrypt_key128(key, cx); }
#endif
#ifdef  AES_192
    aes_rval key192(const unsigned char key[])
            { return aes_decrypt_key192(key, cx); }
#endif
#ifdef  AES_256
    aes_rval key256(const unsigned char key[])
            { return aes_decrypt_key256(key, cx); }
#endif
#ifdef  AES_VAR
    aes_rval key(const unsigned char key[], int key_len)
            { return aes_decrypt_key(key, key_len, cx); }
#endif
    aes_rval decrypt(const unsigned char in[], unsigned char out[]) const
        {   return aes_decrypt(in, out, cx);  }
#ifndef AES_MODES
    aes_rval ecb_decrypt(const unsigned char in[], unsigned char out[], int nb) const
        {   while(nb--)
            {   aes_decrypt(in, out, cx), in += AES_BLOCK_SIZE, out += AES_BLOCK_SIZE; }
        }
#endif
#ifdef AES_MODES

    aes_rval ecb_decrypt(const unsigned char in[], unsigned char out[], int nb) const
        {   return aes_ecb_decrypt(in, out, nb, cx);  }

    aes_rval cbc_decrypt(const unsigned char in[], unsigned char out[], int nb, 
                                    unsigned char iv[]) const
        {   return aes_cbc_decrypt(in, out, nb, iv, cx);  }
#endif
};

#endif

#endif
