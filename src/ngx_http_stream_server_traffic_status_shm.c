
/*
 * Copyright (C) YoungJoo Kim (vozlt)
 */


#include <ngx_config.h>

#include "ngx_http_stream_server_traffic_status_module.h"
#include "ngx_http_stream_server_traffic_status_shm.h"


void
ngx_http_stream_server_traffic_status_shm_info_node(ngx_http_request_t *r,
    ngx_http_stream_server_traffic_status_shm_info_t *shm_info,
    ngx_rbtree_node_t *node)
{
    ngx_uint_t                                     size;
    ngx_http_stream_server_traffic_status_ctx_t   *ctx;
    ngx_http_stream_server_traffic_status_node_t  *stsn;

    ctx = ngx_http_get_module_main_conf(r, ngx_http_stream_server_traffic_status_module);

    if (node != ctx->rbtree->sentinel) {
        stsn = (ngx_http_stream_server_traffic_status_node_t *) &node->color;

        size = offsetof(ngx_rbtree_node_t, color)
               + offsetof(ngx_http_stream_server_traffic_status_node_t, data)
               + stsn->len;

        shm_info->used_size += size;
        shm_info->used_node++;

        ngx_http_stream_server_traffic_status_shm_info_node(r, shm_info, node->left);
        ngx_http_stream_server_traffic_status_shm_info_node(r, shm_info, node->right);
    }
}


void
ngx_http_stream_server_traffic_status_shm_info(ngx_http_request_t *r,
    ngx_http_stream_server_traffic_status_shm_info_t *shm_info)
{
    ngx_http_stream_server_traffic_status_ctx_t  *ctx;

    ctx = ngx_http_get_module_main_conf(r, ngx_http_stream_server_traffic_status_module);

    ngx_memzero(shm_info, sizeof(ngx_http_stream_server_traffic_status_shm_info_t));

    shm_info->name = &ctx->shm_name;
    shm_info->max_size = ctx->shm_size;

    ngx_http_stream_server_traffic_status_shm_info_node(r, shm_info, ctx->rbtree->root);
}


ngx_int_t
ngx_http_stream_server_traffic_status_shm_init(ngx_http_request_t *r)
{
    ngx_shm_zone_t                                    *shm_zone;
    ngx_http_stream_server_traffic_status_ctx_t       *ctx, *sctx;
    ngx_http_stream_server_traffic_status_loc_conf_t  *stscf;

    ctx = ngx_http_get_module_main_conf(r, ngx_http_stream_server_traffic_status_module);

    stscf = ngx_http_get_module_loc_conf(r, ngx_http_stream_server_traffic_status_module);

    shm_zone = ngx_http_stream_server_traffic_status_shm_find_zone(r, &ctx->shm_name);
    if (shm_zone == NULL) {
        return NGX_ERROR;
    }

    sctx = shm_zone->data;

    stscf->shm_zone = shm_zone;
    ctx->rbtree = sctx->rbtree;
    ctx->filter_keys = sctx->filter_keys;
    ctx->limit_traffics = sctx->limit_traffics;
    ctx->limit_filter_traffics = sctx->limit_filter_traffics;
    ctx->shm_size = sctx->shm_size;
    ctx->upstream = sctx->upstream;

    return NGX_OK;
}


ngx_shm_zone_t *
ngx_http_stream_server_traffic_status_shm_find_zone(ngx_http_request_t *r, ngx_str_t *name)
{
    ngx_uint_t                 i;
    ngx_str_t                 *shm_name;
    ngx_shm_zone_t            *shm_zone;
    volatile ngx_list_part_t  *part;

    part = &ngx_cycle->shared_memory.part;
    shm_zone = part->elts;

    for (i = 0; /* void */ ; i++) {

        if (i >= part->nelts) {
            if (part->next == NULL) {
                break;
            }

            part = part->next;
            shm_zone = part->elts;
            i = 0;
        }

        if (name->len != shm_zone[i].shm.name.len) {
            continue;
        }

        shm_name = &shm_zone[i].shm.name;

        ngx_log_debug2(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                       "http stream sts shm_find_zone(): shm_name[%V], name[%V]",
                       shm_name, name);

        if (ngx_strncmp(name->data, shm_name->data, name->len) == 0) {
            return &shm_zone[i];
        }
    }

    return NULL;
}

/* vi:set ft=c ts=4 sw=4 et fdm=marker: */
