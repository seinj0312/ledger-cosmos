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

#include "transaction.h"
#include "view.h"

// TODO: We are currently limited by amount of SRAM (4K)
// In order to parse longer messages we may have to consider moving
// this buffer to FLASH
#define TRANSACTION_JSON_BUFFER_SIZE 650

parsed_json_t parsed_transaction;

char transaction_buffer[TRANSACTION_JSON_BUFFER_SIZE];

uint32_t transaction_buffer_current_position = 0;

void transaction_reset()
{
    transaction_buffer_current_position = 0;
    os_memset((void *) &parsed_transaction, 0, sizeof(parsed_json_t));
}

void transaction_append(unsigned char *buffer, uint32_t length)
{
    if (transaction_buffer_current_position + length >= TRANSACTION_JSON_BUFFER_SIZE) {
        THROW(APDU_CODE_OUTPUT_BUFFER_TOO_SMALL);
    }
    os_memmove(transaction_buffer + transaction_buffer_current_position, buffer, length);
    transaction_buffer_current_position += length;

    // Zero terminate the buffer
    transaction_buffer[transaction_buffer_current_position+1] = '\0';
}

uint32_t transaction_get_buffer_length()
{
    return transaction_buffer_current_position;
}

uint8_t *transaction_get_buffer()
{
    return (uint8_t *) transaction_buffer;
}

void transaction_parse()
{
    ParseJson(&parsed_transaction, transaction_buffer);
    // TODO: Verify is valid. Sorted / whitespaces, etc.
}

parsed_json_t *transaction_get_parsed()
{
    return &parsed_transaction;
}

int transaction_get_info(
    char *name,
    char *value,
    int index)
{
    int currentIndex = 0;
    for (int i = 0; i < parsed_transaction.NumberOfInputs; i++) {
        if (index == currentIndex) {
            os_memmove((char *) name, "Input address", sizeof("Input address"));

            unsigned int addressSize =
                parsed_transaction.Tokens[parsed_transaction.Inputs[i].Address].end -
                    parsed_transaction.Tokens[parsed_transaction.Inputs[i].Address].start;

            view_scrolling_total_size = addressSize;
            const char *addressPtr =
                transaction_buffer +
                    parsed_transaction.Tokens[parsed_transaction.Inputs[i].Address].start;

            if (view_scrolling_step < addressSize) {
                os_memmove(
                    (char *) value,
                    addressPtr + view_scrolling_step,
                    addressSize < MAX_CHARS_PER_LINE ? addressSize : MAX_CHARS_PER_LINE);
                value[addressSize] = '\0';
            }
            return currentIndex;
        }
        currentIndex++;
        for (int j = 0; j < parsed_transaction.Inputs[i].NumberOfCoins; j++) {
            if (index == currentIndex) {
                os_memmove((char *) name, "Coin", sizeof("Coin"));

                int coinSize =
                    parsed_transaction.Tokens[parsed_transaction.Inputs[i].Coins[j].Denum].end -
                        parsed_transaction.Tokens[parsed_transaction.Inputs[i].Coins[j].Denum].start;

                const char *coinPtr =
                    transaction_buffer +
                        parsed_transaction.Tokens[parsed_transaction.Inputs[i].Coins[j].Denum].start;

                os_memmove((char *) value, coinPtr, coinSize);
                value[coinSize] = '\0';
                return currentIndex;
            }
            currentIndex++;
            if (index == currentIndex) {
                os_memmove((char *) name, "Amount", sizeof("Amount"));

                int coinAmountSize =
                    parsed_transaction.Tokens[parsed_transaction.Inputs[i].Coins[j].Amount].end -
                        parsed_transaction.Tokens[parsed_transaction.Inputs[i].Coins[j].Amount].start;

                const char *coinAmountPtr =
                    transaction_buffer +
                        parsed_transaction.Tokens[parsed_transaction.Inputs[i].Coins[j].Amount].start;

                os_memmove((char *) value, coinAmountPtr, coinAmountSize);
                value[coinAmountSize] = '\0';
                return currentIndex;
            }
            currentIndex++;
        }
    }
    for (int i = 0; i < parsed_transaction.NumberOfOutputs; i++) {
        if (index == currentIndex) {
            os_memmove((char *) name, "Output address", sizeof("Output address"));

            unsigned int addressSize =
                parsed_transaction.Tokens[parsed_transaction.Outputs[i].Address].end -
                    parsed_transaction.Tokens[parsed_transaction.Outputs[i].Address].start;

            const char *addressPtr =
                transaction_buffer +
                    parsed_transaction.Tokens[parsed_transaction.Outputs[i].Address].start;

            view_scrolling_total_size = addressSize;

            if (view_scrolling_step < addressSize) {
                os_memmove(
                    (char *) value,
                    addressPtr + view_scrolling_step,
                    addressSize < MAX_CHARS_PER_LINE ? addressSize : MAX_CHARS_PER_LINE);
                value[addressSize] = '\0';
            }
            return currentIndex;
        }
        currentIndex++;
        for (int j = 0; j < parsed_transaction.Outputs[i].NumberOfCoins; j++) {
            if (index == currentIndex) {
                os_memmove((char *) name, "Coin", sizeof("Coin"));

                int coinSize =
                    parsed_transaction.Tokens[parsed_transaction.Outputs[i].Coins[j].Denum].end -
                        parsed_transaction.Tokens[parsed_transaction.Outputs[i].Coins[j].Denum].start;

                const char *coinPtr =
                    transaction_buffer +
                        parsed_transaction.Tokens[parsed_transaction.Outputs[i].Coins[j].Denum].start;

                os_memmove((char *) value, coinPtr, coinSize);
                value[coinSize] = '\0';
                return currentIndex;
            }
            currentIndex++;
            if (index == currentIndex) {
                os_memmove((char *) name, "Amount", sizeof("Amount"));

                int coinAmountSize =
                    parsed_transaction.Tokens[parsed_transaction.Outputs[i].Coins[j].Amount].end -
                        parsed_transaction.Tokens[parsed_transaction.Outputs[i].Coins[j].Amount].start;

                const char *coinAmountPtr =
                    transaction_buffer +
                        parsed_transaction.Tokens[parsed_transaction.Outputs[i].Coins[j].Amount].start;

                os_memmove((char *) value, coinAmountPtr, coinAmountSize);
                value[coinAmountSize] = '\0';
                return currentIndex;
            }
            currentIndex++;
        }
    }
    return currentIndex;
}

