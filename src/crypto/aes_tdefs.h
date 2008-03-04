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
 The unsigned integer types defined here are of the form uint_<nn>t where 
 <nn> is the length of the type; for example, the unsigned 32-bit type is 
 'uint_32t'.  These are NOT the same as the 'C99 integer types' that are 
 defined in the inttypes.h and stdint.h headers since attempts to use these 
 types have shown that support for them is still highly variable.  However, 
 since the latter are of the form uint<nn>_t, a regular expression search 
 and replace (in VC++ search on 'uint_{:z}t' and replace with 'uint\1_t') 
 can be used to convert the types used here to the C99 standard types.
*/

#ifndef TDEFS_H
#define TDEFS_H
#if defined(__cplusplus)
extern "C"
{
#endif

#include <limits.h>

#if UCHAR_MAX == 0xff
  typedef unsigned char     uint_8t;
#else
# error Please define uint_8t as an 8-bit unsigned integer type in tdefs.h
#endif

#if USHRT_MAX == 0xffff
  typedef   unsigned short  uint_16t;
#else
# error Please define uint_16t as a 16-bit unsigned short type in tdefs.h
#endif

#if UINT_MAX == 0xffffffff
  typedef	unsigned int    uint_32t;
#elif ULONG_MAX == 0xfffffffful
  typedef	unsigned long   uint_32t;
#elif defined( _CRAY )
# error This code needs 32-bit data types, which Cray machines do not provide
#else
# error Please define uint_32t as a 32-bit unsigned integer type in tdefs.h
#endif

#if defined( NEED_UINT_64T )
#  define li_64(h)    0x##h##ull
#  if defined( _MSC_VER )
#     if _MSC_VER < 1310
        typedef unsigned __int64    uint_64t;
#       undef  li_64
#       define li_64(h) 0x##h##ui64
#     else
        typedef unsigned long long  uint_64t;
#     endif
#  elif defined( __BORLANDC__ ) && !defined( __MSDOS__ )
     typedef __int64 uint_64t;
#  elif defined( __sun ) && defined(ULONG_MAX) && ULONG_MAX == 0xfffffffful
     typedef unsigned long long    uint_64t;
#  elif defined( ULONG_LONG_MAX ) && ULONG_LONG_MAX == 0xffffffffffffffffull
     typedef unsigned long long    uint_64t;
#  elif defined( ULLONG_MAX ) && ULLONG_MAX == 0xffffffffffffffffull
     typedef unsigned long long    uint_64t;
#  elif defined( ULONG_MAX ) && ULONG_MAX == 0xfffffffffffffffful
     typedef unsigned long         uint_64t;
#  elif defined( UINT_MAX ) && UINT_MAX == 0xffffffffffffffff
     typedef unsigned int          uint_64t;
#  else
#    error Please define uint_64t as an unsigned 64 bit type in tdefs.h
#  endif
#endif

#if defined( DLL_EXPORT )
#  if defined( _MSC_VER ) || defined ( __INTEL_COMPILER )
#    define void_ret    __declspec( dllexport ) void __stdcall
#    define int_ret     __declspec( dllexport ) int  __stdcall
#  elif defined( __GNUC__ )
#    define void_ret    __declspec( __dllexport__ ) void
#    define int_ret     __declspec( __dllexport__ ) int
#  else
#    error Use of the DLL is only available on the Microsoft, Intel and GCC compilers
#  endif
#elif defined( DLL_IMPORT ) 
#  if defined( _MSC_VER ) || defined ( __INTEL_COMPILER )
#    define void_ret    __declspec( dllimport ) void __stdcall
#    define int_ret     __declspec( dllimport ) int  __stdcall
#  elif defined( __GNUC__ )
#    define void_ret    __declspec( __dllimport__ ) void
#    define int_ret     __declspec( __dllimport__ ) int
#  else
#    error Use of the DLL is only available on the Microsoft, Intel and GCC compilers
#  endif
#elif defined( __WATCOMC__ )
#  define void_ret  void __cdecl
#  define int_ret   int  __cdecl
#else
#  define void_ret  void
#  define int_ret   int
#endif

#if defined(__cplusplus)
}
#endif
#endif
