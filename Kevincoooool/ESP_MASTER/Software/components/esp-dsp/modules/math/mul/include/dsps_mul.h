// Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _dsps_mul_H_
#define _dsps_mul_H_
#include "dsp_err.h"


#ifdef __cplusplus
extern "C"
{
#endif


/**@{*/
/**
 * @brief   Multiply two arrays
 * 
 * The function multiply one input array to another and store result to other array
 * out[i*step_out] = input1[i*step1] * input2[i*step2]; i=[0..len)
 * The implementation use ANSI C and could be compiled and run on any platform
 *
 * @param[in] input1: input array 1
 * @param[in] input2: input array 2
 * @param output: output array
 * @param len: amount of operations for arrays
 * @param step1: step over input array 1 (by default should be 1)
 * @param step2: step over input array 2 (by default should be 1)
 * @param step_out: step over output array (by default should be 1)
 * 
 * @return
 *      - ESP_OK on success
 *      - One of the error codes from DSP library
 */
esp_err_t dsps_mul_f32_ansi(const float *input1, const float *input2, float *output, int len, int step1, int step2, int step_out);
esp_err_t dsps_mul_f32_ae32(const float *input1, const float *input2, float *output, int len, int step1, int step2, int step_out);
/**@}*/

#ifdef __cplusplus
}
#endif


#ifdef CONFIG_DSP_OPTIMIZED
#define dsps_mul_f32 dsps_mul_f32_ae32
#endif
#ifdef CONFIG_DSP_ANSI
#define dsps_mul_f32 dsps_mul_f32_ansi
#endif

#endif // _dsps_mul_H_