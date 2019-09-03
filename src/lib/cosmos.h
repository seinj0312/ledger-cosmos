/*******************************************************************************
*  (c) 2019 ZondaX GmbH
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#define BIP32_LEN_DEFAULT       5

#define BIP32_0_DEFAULT     (0x80000000 | 0x2c)
#define BIP32_1_DEFAULT     (0x80000000 | 0x76)
#define BIP32_2_DEFAULT     (0x80000000 | 0)
#define BIP32_3_DEFAULT     (0)
#define BIP32_4_DEFAULT     (0)

#ifdef __cplusplus
}
#endif
