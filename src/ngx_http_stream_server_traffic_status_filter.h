
/*
 * Copyright (C) YoungJoo Kim (vozlt)
 */


#ifndef _NGX_HTTP_STREAM_STS_FILTER_H_INCLUDED_
#define _NGX_HTTP_STREAM_STS_FILTER_H_INCLUDED_


typedef struct {
    ngx_http_complex_value_t                      filter_key;
    ngx_http_complex_value_t                      filter_name;
} ngx_http_stream_server_traffic_status_filter_t;


typedef struct {
    ngx_str_t                                      key;
} ngx_http_stream_server_traffic_status_filter_key_t;


typedef struct {
    uint32_t                                       hash;
    ngx_uint_t                                     index;
} ngx_http_stream_server_traffic_status_filter_uniq_t;


typedef struct {
    ngx_http_stream_server_traffic_status_node_t  *node;
} ngx_http_stream_server_traffic_status_filter_node_t;


int ngx_libc_cdecl ngx_http_stream_server_traffic_status_filter_cmp_keys(
    const void *one, const void *two);
ngx_int_t ngx_http_stream_server_traffic_status_filter_get_keys(
    ngx_http_request_t *r, ngx_array_t **filter_keys,
    ngx_rbtree_node_t *node);
ngx_int_t ngx_http_stream_server_traffic_status_filter_get_nodes(
    ngx_http_request_t *r, ngx_array_t **filter_nodes,
    ngx_str_t *name, ngx_rbtree_node_t *node);


#endif /* _NGX_HTTP_STREAM_STS_FILTER_H_INCLUDED_ */

/* vi:set ft=c ts=4 sw=4 et fdm=marker: */
