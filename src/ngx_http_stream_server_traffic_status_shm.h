
/*
 * Copyright (C) YoungJoo Kim (vozlt)
 */


#ifndef _NGX_HTTP_STREAM_STS_SHM_H_INCLUDED_
#define _NGX_HTTP_STREAM_STS_SHM_H_INCLUDED_


ngx_int_t ngx_http_stream_server_traffic_status_shm_init(ngx_http_request_t *r);
ngx_shm_zone_t *ngx_http_stream_server_traffic_status_shm_find_zone(ngx_http_request_t *r,
    ngx_str_t *name);


#endif /* _NGX_HTTP_STREAM_STS_SHM_H_INCLUDED_ */

/* vi:set ft=c ts=4 sw=4 et fdm=marker: */
