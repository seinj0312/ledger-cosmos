/*******************************************************************************
*   (c) ZondaX GmbH
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

#include <jsmn.h>
#include "json/json_parser.h"

const char whitespaces[] = {
        0x20,// space ' '
        0x0c, // form_feed '\f'
        0x0a, // line_feed, '\n'
        0x0d, // carriage_return, '\r'
        0x09, // horizontal_tab, '\t'
        0x0b // vertical_tab, '\v'
};

int8_t is_space(char c) {
    for (uint16_t i = 0; i < sizeof(whitespaces); i++) {
        if (whitespaces[i] == c) {
            return 1;
        }
    }
    return 0;
}

int8_t contains_whitespace(parsed_json_t *json, const char *transaction) {
    int start = 0;
    const int last_element_index = json->tokens[0].end;

    // Starting at token 1 because token 0 contains full tx
    for (int i = 1; i < json->numberOfTokens; i++) {
        if (json->tokens[i].type != JSMN_UNDEFINED) {
            const int end = json->tokens[i].start;
            for (int j = start; j < end; j++) {
                if (is_space(transaction[j]) == 1) {
                    return 1;
                }
            }
            start = json->tokens[i].end + 1;
        } else {
            return 0;
        }
    }
    while (start <= last_element_index && transaction[start] != '\0') {
        if (is_space(transaction[start])) {
            return 1;
        }
        start++;
    }
    return 0;
}

int8_t is_sorted(int16_t first_index, int16_t second_index,
                 parsed_json_t *json,
                 const char *transaction) {
#if DEBUG_SORTING
    char first[256];
    char second[256];

    int size =  parsed_tx->Tokens[first_index].end - parsed_tx->Tokens[first_index].start;
    strncpy(first, tx + parsed_tx->Tokens[first_index].start, size);
    first[size] = '\0';
    size =  parsed_tx->Tokens[second_index].end - parsed_tx->Tokens[second_index].start;
    strncpy(second, tx + parsed_tx->Tokens[second_index].start, size);
    second[size] = '\0';
#endif

    if (strcmp(transaction + json->tokens[first_index].start,
               transaction + json->tokens[second_index].start) <= 0) {
        return 1;
    }
    return 0;
}

int8_t dictionaries_sorted(parsed_json_t *json,
                           const char *transaction) {
    for (int i = 0; i < json->numberOfTokens; i++) {
        if (json->tokens[i].type == JSMN_OBJECT) {

            const int count = object_get_element_count(i, json);
            if (count > 1) {
                int prev_token_index = object_get_nth_key(i, 0, json);
                for (int j = 1; j < count; j++) {
                    int next_token_index = object_get_nth_key(i, j, json);
                    if (!is_sorted(prev_token_index, next_token_index, json, transaction)) {
                        return 0;
                    }
                    prev_token_index = next_token_index;
                }
            }
        }
    }
    return 1;
}

const char *tx_validate(parsed_json_t *json, const char *transaction) {
    if (contains_whitespace(json, transaction) == 1) {
        return "JSON Contains whitespace in the corpus";
    }

    if (dictionaries_sorted(json, transaction) != 1) {
        return "JSON Dictionaries are not sorted";
    }

    if (object_get_value(
        json,
        transaction, 0,
        "chain_id") == -1) {
        return "JSON Missing chain_id";
    }

    if (object_get_value(
        json,
        transaction, 0,
        "sequence") == -1) {
        return "JSON Missing sequence";
    }

    if (object_get_value(
        json,
        transaction, 0,
        "fee") == -1) {
        return "JSON Missing fee";
    }

    if (object_get_value(
        json,
        transaction, 0,
        "msgs") == -1) {
        return "JSON Missing msgs";
    }

    if (object_get_value(
        json,
        transaction, 0,
        "account_number") == -1) {
        return "JSON Missing account_number";
    }

    if (object_get_value(
        json,
        transaction, 0,
        "memo") == -1) {
        return "JSON Missing memo";
    }

    return NULL;
}
