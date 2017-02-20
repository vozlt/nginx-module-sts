
/*
 * Copyright (C) YoungJoo Kim (vozlt)
 */


#include "ngx_http_stream_server_traffic_status_module.h"
#include "ngx_http_stream_server_traffic_status_filter.h"


int ngx_libc_cdecl
ngx_http_stream_server_traffic_status_filter_cmp_keys(const void *one, const void *two)
{
    ngx_http_stream_server_traffic_status_filter_key_t *first =
                            (ngx_http_stream_server_traffic_status_filter_key_t *) one;
    ngx_http_stream_server_traffic_status_filter_key_t *second =
                            (ngx_http_stream_server_traffic_status_filter_key_t *) two;

    return (int) ngx_strcmp(first->key.data, second->key.data);
}


ngx_int_t
ngx_http_stream_server_traffic_status_filter_get_keys(ngx_http_request_t *r,
    ngx_array_t **filter_keys, ngx_rbtree_node_t *node)
{
    ngx_int_t                                            rc;
    ngx_str_t                                            key;
    ngx_http_stream_server_traffic_status_ctx_t         *ctx;
    ngx_http_stream_server_traffic_status_node_t        *stsn;
    ngx_http_stream_server_traffic_status_filter_key_t  *keys;

    ctx = ngx_http_get_module_main_conf(r, ngx_http_stream_server_traffic_status_module);

    if (node != ctx->rbtree->sentinel) {
        stsn = (ngx_http_stream_server_traffic_status_node_t *) &node->color;

        if (stsn->stat_upstream.type == NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_UPSTREAM_FG) {
            key.data = stsn->data;
            key.len = stsn->len;

            rc = ngx_http_stream_server_traffic_status_node_position_key(&key, 1);
            if (rc != NGX_OK) {
                goto next;
            }

            if (*filter_keys == NULL) {
                *filter_keys = ngx_array_create(r->pool, 1,
                                  sizeof(ngx_http_stream_server_traffic_status_filter_key_t));

                if (*filter_keys == NULL) {
                    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                                  "filter_get_keys::ngx_array_create() failed");
                    return NGX_ERROR;
                }
            }

            keys = ngx_array_push(*filter_keys);
            if (keys == NULL) {
                ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                              "filter_get_keys::ngx_array_push() failed");
                return NGX_ERROR;
            }

            keys->key.len = key.len;
            /* 1 byte for terminating '\0' for ngx_strcmp() */
            keys->key.data = ngx_pcalloc(r->pool, key.len + 1);
            if (keys->key.data == NULL) {
                ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                              "filter_get_keys::ngx_pcalloc() failed");
            }

            ngx_memcpy(keys->key.data, key.data, key.len);
        }
next:
        rc = ngx_http_stream_server_traffic_status_filter_get_keys(r, filter_keys, node->left);
        if (rc != NGX_OK) {
            return rc;
        }

        rc = ngx_http_stream_server_traffic_status_filter_get_keys(r, filter_keys, node->right);
        if (rc != NGX_OK) {
            return rc;
        }
    }

    return NGX_OK;
}


ngx_int_t
ngx_http_stream_server_traffic_status_filter_get_nodes(ngx_http_request_t *r,
    ngx_array_t **filter_nodes, ngx_str_t *name,
    ngx_rbtree_node_t *node)
{
    ngx_int_t                                             rc;
    ngx_str_t                                             key;
    ngx_http_stream_server_traffic_status_ctx_t          *ctx;
    ngx_http_stream_server_traffic_status_node_t         *stsn;
    ngx_http_stream_server_traffic_status_filter_node_t  *nodes;

    ctx = ngx_http_get_module_main_conf(r, ngx_http_stream_server_traffic_status_module);

    if (node != ctx->rbtree->sentinel) {
        stsn = (ngx_http_stream_server_traffic_status_node_t *) &node->color;

        if (stsn->stat_upstream.type == NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_UPSTREAM_FG) {
            key.data = stsn->data;
            key.len = stsn->len;

            rc = ngx_http_stream_server_traffic_status_node_position_key(&key, 1);
            if (rc != NGX_OK) {
                goto next;
            }

            if (name->len != key.len) {
                goto next;
            }

            if (ngx_strncmp(name->data, key.data, key.len) != 0) {
                goto next;
            }

            if (*filter_nodes == NULL) {
                *filter_nodes = ngx_array_create(r->pool, 1,
                                    sizeof(ngx_http_stream_server_traffic_status_filter_node_t));

                if (*filter_nodes == NULL) {
                    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                                  "filter_get_nodes::ngx_array_create() failed");
                    return NGX_ERROR;
                }
            }

            nodes = ngx_array_push(*filter_nodes);
            if (nodes == NULL) {
                ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                              "filter_get_nodes::ngx_array_push() failed");
                return NGX_ERROR;
            }

            nodes->node = stsn;
        }
next:
        rc = ngx_http_stream_server_traffic_status_filter_get_nodes(r, filter_nodes, name,
                                                                    node->left);
        if (rc != NGX_OK) {
            return rc;
        }

        rc = ngx_http_stream_server_traffic_status_filter_get_nodes(r, filter_nodes, name,
                                                                    node->right);
        if (rc != NGX_OK) {
            return rc;
        }
    }

    return NGX_OK;
}

/* vi:set ft=c ts=4 sw=4 et fdm=marker: */
