/**
 * NEURON IIoT System for Industry 4.0
 * Copyright (C) 2020-2021 EMQ Technologies Co., Ltd All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 **/

/*
 * DO NOT EDIT THIS FILE MANUALLY!
 * It was automatically generated by `json-autotype`.
 */

#include <stdlib.h>
#include <string.h>

#include "json/json.h"

#include "json/neu_json_mqtt.h"

int neu_json_decode_mqtt_req(char *buf, neu_json_mqtt_t **result)
{
    int              ret = 0;
    neu_json_mqtt_t *req = calloc(1, sizeof(neu_json_mqtt_t));

    neu_json_elem_t elems_uuid[] = { {
        .name = "uuid",
        .t    = NEU_JSON_STR,
    } };

    neu_json_elem_t elems_command[] = { {
        .name = "command",
        .t    = NEU_JSON_STR,
    } };

    ret = neu_json_decode(buf, NEU_JSON_ELEM_SIZE(elems_uuid), elems_uuid);
    if (ret == 0) {
        req->uuid = elems_uuid[0].v.val_str;
    } else {
        goto decode_fail;
    }

    ret =
        neu_json_decode(buf, NEU_JSON_ELEM_SIZE(elems_command), elems_command);
    if (ret == 0) {
        req->command = elems_command[0].v.val_str;
    } else {
        req->command = strdup("");
    }

    *result = req;
    return 0;

decode_fail:
    if (req != NULL) {
        free(req);
    }
    return -1;
}

void neu_json_decode_mqtt_req_free(neu_json_mqtt_t *req)
{
    if (NULL == req) {
        return;
    }

    if (NULL != req->uuid) {
        free(req->uuid);
    }

    if (NULL != req->command) {
        free(req->command);
    }

    free(req);
}

int neu_json_encode_mqtt_resp(void *json_object, void *param)
{
    int              ret  = 0;
    neu_json_mqtt_t *resp = (neu_json_mqtt_t *) param;

    neu_json_elem_t resp_elems[] = { {
        .name      = "uuid",
        .t         = NEU_JSON_STR,
        .v.val_str = resp->uuid,
    } };

    ret = neu_json_encode_field(json_object, resp_elems,
                                NEU_JSON_ELEM_SIZE(resp_elems));

    return ret;
}