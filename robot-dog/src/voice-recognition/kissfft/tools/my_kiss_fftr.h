/*
 *  Copyright (c) 2003-2004, Mark Borgerding. All rights reserved.
 *  This file is part of KISS FFT - https://github.com/mborgerding/kissfft
 *
 *  SPDX-License-Identifier: BSD-3-Clause
 *  See COPYING file for more information.
 */

#ifndef MY_KISS_FTR_H
#define MY_KISS_FTR_H

#include "../my_kiss_fft.h"
//#ifdef __cplusplus
//extern "C++" {
//#endif

    
/* 
 
 Real optimized version can save about 45% cpu time vs. complex fft of a real seq.

 
 
 */

typedef struct my_kiss_fftr_state *my_kiss_fftr_cfg;


my_kiss_fftr_cfg my_kiss_fftr_alloc(int nfft,int inverse_fft,void * mem, size_t * lenmem);
/*
 nfft must be even

 If you don't care to allocate space, use mem = lenmem = NULL 
*/


void my_kiss_fftr(my_kiss_fftr_cfg cfg,const my_kiss_fft_scalar *timedata,my_kiss_fft_cpx *freqdata);
/*
 input timedata has nfft scalar points
 output freqdata has nfft/2+1 complex points
*/

void my_kiss_fftri(my_kiss_fftr_cfg cfg,const my_kiss_fft_cpx *freqdata,my_kiss_fft_scalar *timedata);
/*
 input freqdata has  nfft/2+1 complex points
 output timedata has nfft scalar points
*/

#define my_kiss_fftr_free MY_KISS_FFT_FREE

//#ifdef __cplusplus
//}
//#endif
#endif
