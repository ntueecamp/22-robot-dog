/*
 *  Copyright (c) 2003-2010, Mark Borgerding. All rights reserved.
 *  This file is part of KISS FFT - https://github.com/mborgerding/kissfft
 *
 *  SPDX-License-Identifier: BSD-3-Clause
 *  See COPYING file for more information.
 */

#ifndef MY_KISS_FFT_H
#define MY_KISS_FFT_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#ifdef __cplusplus
extern "C++" {
#endif

/*
 ATTENTION!
 If you would like a :
 -- a utility that will handle the caching of fft objects
 -- real-only (no imaginary time component ) FFT
 -- a multi-dimensional FFT
 -- a command-line utility to perform ffts
 -- a command-line utility to perform fast-convolution filtering

 Then see kfc.h my_kiss_fftr.h my_kiss_fftnd.h fftutil.c kiss_fastfir.c
  in the tools/ directory.
*/

/* User may override MY_KISS_FFT_MALLOC and/or MY_KISS_FFT_FREE. */
#ifdef USE_SIMD
# include <xmmintrin.h>
# define my_kiss_fft_scalar __m128
# ifndef MY_KISS_FFT_MALLOC
#  define MY_KISS_FFT_MALLOC(nbytes) _mm_malloc(nbytes,16)
#  define MY_KISS_FFT_ALIGN_CHECK(ptr) 
#  define MY_KISS_FFT_ALIGN_SIZE_UP(size) ((size + 15UL) & ~0xFUL)
# endif
# ifndef MY_KISS_FFT_FREE
#  define MY_KISS_FFT_FREE _mm_free
# endif
#else
# define MY_KISS_FFT_ALIGN_CHECK(ptr)
# define MY_KISS_FFT_ALIGN_SIZE_UP(size) (size)
# ifndef MY_KISS_FFT_MALLOC
#  define MY_KISS_FFT_MALLOC malloc
# endif
# ifndef MY_KISS_FFT_FREE
#  define MY_KISS_FFT_FREE free
# endif
#endif


#ifdef FIXED_POINT
#include <stdint.h>
# if (FIXED_POINT == 32)
#  define my_kiss_fft_scalar int32_t
# else	
#  define my_kiss_fft_scalar int16_t
# endif
#else
# ifndef my_kiss_fft_scalar
/*  default is float */
#   define my_kiss_fft_scalar float
# endif
#endif

typedef struct {
    my_kiss_fft_scalar r;
    my_kiss_fft_scalar i;
}my_kiss_fft_cpx;

typedef struct my_kiss_fft_state* my_kiss_fft_cfg;

/* 
 *  my_kiss_fft_alloc
 *  
 *  Initialize a FFT (or IFFT) algorithm's cfg/state buffer.
 *
 *  typical usage:      my_kiss_fft_cfg mycfg=my_kiss_fft_alloc(1024,0,NULL,NULL);
 *
 *  The return value from fft_alloc is a cfg buffer used internally
 *  by the fft routine or NULL.
 *
 *  If lenmem is NULL, then my_kiss_fft_alloc will allocate a cfg buffer using malloc.
 *  The returned value should be free()d when done to avoid memory leaks.
 *  
 *  The state can be placed in a user supplied buffer 'mem':
 *  If lenmem is not NULL and mem is not NULL and *lenmem is large enough,
 *      then the function places the cfg in mem and the size used in *lenmem
 *      and returns mem.
 *  
 *  If lenmem is not NULL and ( mem is NULL or *lenmem is not large enough),
 *      then the function returns NULL and places the minimum cfg 
 *      buffer size in *lenmem.
 * */

my_kiss_fft_cfg my_kiss_fft_alloc(int nfft,int inverse_fft,void * mem,size_t * lenmem); 

/*
 * my_kiss_fft(cfg,in_out_buf)
 *
 * Perform an FFT on a complex input buffer.
 * for a forward FFT,
 * fin should be  f[0] , f[1] , ... ,f[nfft-1]
 * fout will be   F[0] , F[1] , ... ,F[nfft-1]
 * Note that each element is complex and can be accessed like
    f[k].r and f[k].i
 * */
void my_kiss_fft(my_kiss_fft_cfg cfg,const my_kiss_fft_cpx *fin,my_kiss_fft_cpx *fout);

/*
 A more generic version of the above function. It reads its input from every Nth sample.
 * */
void my_kiss_fft_stride(my_kiss_fft_cfg cfg,const my_kiss_fft_cpx *fin,my_kiss_fft_cpx *fout,int fin_stride);

/* If my_kiss_fft_alloc allocated a buffer, it is one contiguous 
   buffer and can be simply free()d when no longer needed*/
#define my_kiss_fft_free MY_KISS_FFT_FREE

/*
 Cleans up some memory that gets managed internally. Not necessary to call, but it might clean up 
 your compiler output to call this before you exit.
*/
void my_kiss_fft_cleanup(void);
	

/*
 * Returns the smallest integer k, such that k>=n and k has only "fast" factors (2,3,5)
 */
int my_kiss_fft_next_fast_size(int n);

/* for real ffts, we need an even size */
#define my_kiss_fftr_next_fast_size_real(n) \
        (my_kiss_fft_next_fast_size( ((n)+1)>>1)<<1)

#ifdef __cplusplus
} 
#endif

#endif
