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

#include "persist_json_subs.h"

int neu_json_decode_subscriptions_req(char *                         buf,
                                      neu_json_subscriptions_req_t **result)
{
    int                           ret      = 0;
    void *                        json_obj = NULL;
    neu_json_subscriptions_req_t *req =
        calloc(1, sizeof(neu_json_subscriptions_req_t));
    if (req == NULL) {
        return -1;
    }

    json_obj = neu_json_decode_new(buf);

    req->n_subscription =
        neu_json_decode_array_size_by_json(json_obj, "subscriptions");
    if (req->n_subscription < 0) {
        goto decode_fail;
    }

    req->subscriptions = calloc(
        req->n_subscription, sizeof(neu_json_subscriptions_req_subscription_t));
    neu_json_subscriptions_req_subscription_t *p_subscription =
        req->subscriptions;
    for (int i = 0; i < req->n_subscription; i++) {
        neu_json_elem_t subscription_elems[] = { {
                                                     .name = "sub_adapter_name",
                                                     .t    = NEU_JSON_STR,
                                                 },
                                                 {
                                                     .name = "src_adapter_name",
                                                     .t    = NEU_JSON_STR,
                                                 },
                                                 {
                                                     .name = "read_interval",
                                                     .t    = NEU_JSON_INT,
                                                 },
                                                 {
                                                     .name =
                                                         "group_config_name",
                                                     .t = NEU_JSON_STR,
                                                 } };
        ret                                  = neu_json_decode_array_by_json(
            json_obj, "subscriptions", i,
            NEU_JSON_ELEM_SIZE(subscription_elems), subscription_elems);
        if (ret != 0) {
            goto decode_fail;
        }

        p_subscription->sub_adapter_name  = subscription_elems[0].v.val_str;
        p_subscription->src_adapter_name  = subscription_elems[1].v.val_str;
        p_subscription->read_interval     = subscription_elems[2].v.val_int;
        p_subscription->group_config_name = subscription_elems[3].v.val_str;
        p_subscription++;
    }

    *result = req;
    goto decode_exit;

decode_fail:
    if (req->subscriptions != NULL) {
        free(req->subscriptions);
    }
    if (req != NULL) {
        free(req);
    }
    ret = -1;

decode_exit:
    if (json_obj != NULL) {
        neu_json_decode_free(json_obj);
    }
    return ret;
}

void neu_json_decode_subscriptions_req_free(neu_json_subscriptions_req_t *req)
{

    neu_json_subscriptions_req_subscription_t *p_subscription =
        req->subscriptions;
    for (int i = 0; i < req->n_subscription; i++) {
        free(p_subscription->sub_adapter_name);
        free(p_subscription->src_adapter_name);
        free(p_subscription->group_config_name);
        p_subscription++;
    }

    free(req->subscriptions);

    free(req);
}

int neu_json_encode_subscriptions_resp(void *json_object, void *param)
{
    int                            ret = 0;
    neu_json_subscriptions_resp_t *resp =
        (neu_json_subscriptions_resp_t *) param;

    void *subscription_array = neu_json_array();
    neu_json_subscriptions_resp_subscription_t *p_subscription =
        resp->subscriptions;
    for (int i = 0; i < resp->n_subscription; i++) {
        neu_json_elem_t subscription_elems[] = {
            {
                .name      = "sub_adapter_name",
                .t         = NEU_JSON_STR,
                .v.val_str = p_subscription->sub_adapter_name,
            },
            {
                .name      = "src_adapter_name",
                .t         = NEU_JSON_STR,
                .v.val_str = p_subscription->src_adapter_name,
            },
            {
                .name      = "read_interval",
                .t         = NEU_JSON_INT,
                .v.val_int = p_subscription->read_interval,
            },
            {
                .name      = "group_config_name",
                .t         = NEU_JSON_STR,
                .v.val_str = p_subscription->group_config_name,
            }
        };
        subscription_array =
            neu_json_encode_array(subscription_array, subscription_elems,
                                  NEU_JSON_ELEM_SIZE(subscription_elems));
        p_subscription++;
    }

    neu_json_elem_t resp_elems[] = { {
        .name         = "subscriptions",
        .t            = NEU_JSON_OBJECT,
        .v.val_object = subscription_array,
    } };
    ret = neu_json_encode_field(json_object, resp_elems,
                                NEU_JSON_ELEM_SIZE(resp_elems));

    return ret;
}