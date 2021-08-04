/**
 * @file character_data.c
 * @author
 * @date 2021/07/02
 * @brief The complementation of character data.
 *
 * Copyright (C) 2021 FMSoft <https://www.fmsoft.cn>
 *
 * This file is a part of PurC (short for Purring Cat), an HVML interpreter.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * This implementation of HTML parser is derived from Lexbor
 * <https://github.com/lexbor/lexbor>, which is licensed under the Apache
 * License, Version 2.0:
 *
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "purc.h"
#include "config.h"
#include "private/instance.h"
#include "private/errors.h"
#include "private/edom.h"


pcedom_character_data_t *
pcedom_character_data_interface_create(pcedom_document_t *document)
{
    pcedom_character_data_t *element;

    element = pcutils_mraw_calloc(document->mraw,
                                 sizeof(pcedom_character_data_t));
    if (element == NULL) {
        return NULL;
    }

    pcedom_node_t *node = pcedom_interface_node(element);

    node->owner_document = document;
    node->type = PCEDOM_NODE_TYPE_UNDEF;

    return element;
}

pcedom_character_data_t *
pcedom_character_data_interface_destroy(pcedom_character_data_t *character_data)
{
    pcutils_str_destroy(&character_data->data,
                       pcedom_interface_node(character_data)->owner_document->text,
                       false);

    return pcutils_mraw_free(
        pcedom_interface_node(character_data)->owner_document->mraw,
        character_data);
}

/* TODO: oh, need to... https://dom.spec.whatwg.org/#concept-cd-replace */
unsigned int
pcedom_character_data_replace(pcedom_character_data_t *ch_data,
                               const unsigned char *data, size_t len,
                               size_t offset, size_t count)
{
    UNUSED_PARAM(offset);
    UNUSED_PARAM(count);

    if (ch_data->data.data == NULL) {
        pcutils_str_init(&ch_data->data, ch_data->node.owner_document->text, len);
        if (ch_data->data.data == NULL) {
            pcinst_set_error (PURC_ERROR_OUT_OF_MEMORY);
            return PCHTML_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }
    else if (pcutils_str_size(&ch_data->data) < len) {
        const unsigned char *data;

        data = pcutils_str_realloc(&ch_data->data,
                                  ch_data->node.owner_document->text, (len + 1));
        if (data == NULL) {
            pcinst_set_error (PURC_ERROR_OUT_OF_MEMORY);
            return PCHTML_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    memcpy(ch_data->data.data, data, sizeof(unsigned char) * len);

    ch_data->data.data[len] = 0x00;
    ch_data->data.length = len;

    return PCHTML_STATUS_OK;
}
