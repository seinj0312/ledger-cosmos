/*******************************************************************************
*   (c) 2016 Ledger
*   (c) 2018 ZondaX GmbH
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
#include "signature.h"

#include "cx.h"

#define SIGNATURE_LENGTH 100
#define DERIVATION_PATH_MAX_DEPTH 10

uint8_t signature[SIGNATURE_LENGTH];
uint8_t length = 0;
uint32_t bip32_derivation_path[DERIVATION_PATH_MAX_DEPTH];
uint8_t bip32_derivation_path_length;
uint32_t default_bip32_derivation_path[] = {44, 60, 0, 0};

void signature_reset()
{
    os_memset((void*)signature, 0, sizeof(signature));
    length = 0;
    os_memset((void*)bip32_derivation_path, 0, sizeof(bip32_derivation_path));
    signature_set_derivation_path(
            default_bip32_derivation_path,
            (sizeof(default_bip32_derivation_path) / default_bip32_derivation_path[0]));
}

void signature_set_derivation_path(uint32_t* path, uint32_t path_size)
{
    os_memmove(bip32_derivation_path, path, path_size * sizeof(path[0]));
    bip32_derivation_path_length = path_size;
}

void signature_create(uint8_t* message, uint16_t message_length)
{
    cx_sha256_t sha2;
    uint8_t hashMessage[32];
    cx_sha256_init(&sha2);

    cx_hash((cx_hash_t *)&sha2, 0, message, message_length, NULL);
    cx_hash((cx_hash_t *)&sha2, CX_LAST, message, 0, hashMessage);

    uint8_t privateKeyData[32];
    cx_ecfp_private_key_t privateKey;

    os_perso_derive_node_bip32(
            CX_CURVE_256K1,
            bip32_derivation_path,
            bip32_derivation_path_length,
            privateKeyData,
            NULL);

    cx_ecfp_init_private_key(CX_CURVE_256K1, privateKeyData, 32, &privateKey);
    os_memset(privateKeyData, 0, sizeof(privateKeyData));

    unsigned int info = 0;
    length =
            cx_ecdsa_sign(&privateKey, CX_RND_RFC6979 | CX_LAST, CX_SHA256,
                          hashMessage,
                          sizeof(hashMessage),
                          signature,
                          &info);

    os_memset(&privateKey, 0, sizeof(privateKey));
}

uint8_t* signature_get()
{
    return signature;
}

uint16_t signature_length()
{
    return length;
}