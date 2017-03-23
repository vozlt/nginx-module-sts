
/*
 * Copyright (C) YoungJoo Kim (vozlt)
 */


#include <ngx_config.h>

#include "ngx_http_stream_server_traffic_status_module.h"
#include "ngx_http_stream_server_traffic_status_node.h"


ngx_int_t
ngx_http_stream_server_traffic_status_node_generate_key(ngx_pool_t *pool,
    ngx_str_t *buf, ngx_str_t *dst, unsigned type)
{
    size_t   len;
    u_char  *p;

    len = ngx_strlen(ngx_http_stream_server_traffic_status_group_to_string(type));

    buf->len = len + sizeof("@") - 1 + dst->len;
    buf->data = ngx_pcalloc(pool, buf->len);
    if (buf->data == NULL) {
        *buf = *dst;
        return NGX_ERROR;
    }

    p = buf->data;

    p = ngx_cpymem(p, ngx_http_stream_server_traffic_status_group_to_string(type), len);
    *p++ = NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_KEY_SEPARATOR;
    p = ngx_cpymem(p, dst->data, dst->len);

    return NGX_OK;
}


ngx_int_t
ngx_http_stream_server_traffic_status_node_position_key(ngx_str_t *buf, size_t pos)
{
    size_t   n, c, len;
    u_char  *p, *s;

    n = buf->len + 1;
    c = len = 0;
    p = s = buf->data;

    while (--n) {
        if (*p == NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_KEY_SEPARATOR) {
            if (pos == c) {
                break;
            }
            s = (p + 1);
            c++;
        }
        p++;
        len = (p - s);
    }

    if (pos > c || len == 0) {
        return NGX_ERROR;
    }

    buf->data = s;
    buf->len = len;

    return NGX_OK;
}


ngx_rbtree_node_t *
ngx_http_stream_server_traffic_status_find_node(ngx_http_request_t *r,
    ngx_str_t *key, unsigned type, uint32_t key_hash)
{
    uint32_t                                           hash;
    ngx_rbtree_node_t                                 *node;
    ngx_http_stream_server_traffic_status_ctx_t       *ctx;
    ngx_http_stream_server_traffic_status_loc_conf_t  *stscf;

    ctx = ngx_http_get_module_main_conf(r, ngx_http_stream_server_traffic_status_module);
    stscf = ngx_http_get_module_loc_conf(r, ngx_http_stream_server_traffic_status_module);

    hash = key_hash;

    if (hash == 0) {
        hash = ngx_crc32_short(key->data, key->len);
    }

    if (stscf->node_caches[type] != NULL) {
        if (stscf->node_caches[type]->key == hash) {
            node = stscf->node_caches[type];
            goto found;
        }
    }

    node = ngx_http_stream_server_traffic_status_node_lookup(ctx->rbtree, key, hash);

found:

    return node;
}


ngx_rbtree_node_t *
ngx_http_stream_server_traffic_status_node_lookup(ngx_rbtree_t *rbtree, ngx_str_t *key,
    uint32_t hash)
{
    ngx_int_t                                      rc;
    ngx_rbtree_node_t                             *node, *sentinel;
    ngx_http_stream_server_traffic_status_node_t  *stsn;

    node = rbtree->root;
    sentinel = rbtree->sentinel;

    while (node != sentinel) {

        if (hash < node->key) {
            node = node->left;
            continue;
        }

        if (hash > node->key) {
            node = node->right;
            continue;
        }

        /* hash == node->key */

        stsn = (ngx_http_stream_server_traffic_status_node_t *) &node->color;

        rc = ngx_memn2cmp(key->data, stsn->data, key->len, (size_t) stsn->len);
        if (rc == 0) {
            return node;
        }

        node = (rc < 0) ? node->left : node->right;
    }

    return NULL;
}


void
ngx_http_stream_server_traffic_status_node_zero(ngx_http_stream_server_traffic_status_node_t *stsn)
{
    stsn->stat_connect_counter = 0;
    stsn->stat_in_bytes = 0;
    stsn->stat_out_bytes = 0;
    stsn->stat_1xx_counter = 0;
    stsn->stat_2xx_counter = 0;
    stsn->stat_3xx_counter = 0;
    stsn->stat_4xx_counter = 0;
    stsn->stat_5xx_counter = 0;

    stsn->stat_session_time = 0;

    stsn->stat_connect_counter_oc = 0;
    stsn->stat_in_bytes_oc = 0;
    stsn->stat_out_bytes_oc = 0;
    stsn->stat_1xx_counter_oc = 0;
    stsn->stat_2xx_counter_oc = 0;
    stsn->stat_3xx_counter_oc = 0;
    stsn->stat_4xx_counter_oc = 0;
    stsn->stat_5xx_counter_oc = 0;
}

/* vi:set ft=c ts=4 sw=4 et fdm=marker: */
