
/*
 * Copyright (C) YoungJoo Kim (vozlt)
 */


#ifndef _NGX_HTTP_STREAM_STS_SHM_H_INCLUDED_
#define _NGX_HTTP_STREAM_STS_SHM_H_INCLUDED_


typedef struct {
    ngx_str_t   *name;
    ngx_uint_t   max_size;
    ngx_uint_t   used_size;
    ngx_uint_t   used_node;
} ngx_http_stream_server_traffic_status_shm_info_t;


void ngx_http_stream_server_traffic_status_shm_info_node(ngx_http_request_t *r,
    ngx_http_stream_server_traffic_status_shm_info_t *shm_info,
    ngx_rbtree_node_t *node);
void ngx_http_stream_server_traffic_status_shm_info(ngx_http_request_t *r,
    ngx_http_stream_server_traffic_status_shm_info_t *shm_info);

ngx_int_t ngx_http_stream_server_traffic_status_shm_init(ngx_http_request_t *r);
ngx_shm_zone_t *ngx_http_stream_server_traffic_status_shm_find_zone(ngx_http_request_t *r,
    ngx_str_t *name);


#endif /* _NGX_HTTP_STREAM_STS_SHM_H_INCLUDED_ */

/* vi:set ft=c ts=4 sw=4 et fdm=marker: */
