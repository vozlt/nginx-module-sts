
/*
 * Copyright (C) YoungJoo Kim (vozlt)
 */


#ifndef _NGX_HTTP_STREAM_STS_MODULE_H_INCLUDED_
#define _NGX_HTTP_STREAM_STS_MODULE_H_INCLUDED_


#include <nginx.h>
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <ngx_stream.h>

#include "ngx_http_stream_server_traffic_status_string.h"
#include "ngx_http_stream_server_traffic_status_shm.h"
#include "ngx_http_stream_server_traffic_status_node.h"

#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_UPSTREAM_NO          0
#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_UPSTREAM_UA          1
#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_UPSTREAM_UG          2
#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_UPSTREAM_FG          3

#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_UPSTREAMS            \
    (u_char *) "NO\0UA\0UG\0FG\0"

#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_NODE_NONE            0
#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_NODE_FIND            1

#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_KEY_SEPARATOR        (u_char) 0x1f

#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_FORMAT_NONE          0
#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_FORMAT_JSON          1
#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_FORMAT_HTML          2
#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_FORMAT_JSONP         3

#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_DEFAULT_SHM_NAME                 \
    "stream_server_traffic_status"

#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_DEFAULT_JSONP                    \
    "ngx_http_stream_server_traffic_status_jsonp_callback"

#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_S           "{"
#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_OBJECT_S    "\"%V\":{"
#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_ARRAY_S     "\"%V\":["

#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_ARRAY_E     "]"
#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_OBJECT_E    "}"
#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_E           "}"
#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_NEXT        ","

#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_MAIN                    \
    "\"hostName\":\"%V\","                                                     \
    "\"nginxVersion\":\"%s\","                                                 \
    "\"loadMsec\":%M,"                                                         \
    "\"nowMsec\":%M,"                                                          \
    "\"connections\":{"                                                        \
    "\"active\":%uA,"                                                          \
    "\"reading\":%uA,"                                                         \
    "\"writing\":%uA,"                                                         \
    "\"waiting\":%uA,"                                                         \
    "\"accepted\":%uA,"                                                        \
    "\"handled\":%uA,"                                                         \
    "\"requests\":%uA"                                                         \
    "},"

#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_SERVER_S                \
    "\"streamServerZones\":{"

#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_SERVER "\"%V\":{"       \
    "\"port\":%ui,"                                                            \
    "\"protocol\":\"%V\","                                                     \
    "\"connectCounter\":%uA,"                                                  \
    "\"inBytes\":%uA,"                                                         \
    "\"outBytes\":%uA,"                                                        \
    "\"responses\":{"                                                          \
    "\"1xx\":%uA,"                                                             \
    "\"2xx\":%uA,"                                                             \
    "\"3xx\":%uA,"                                                             \
    "\"4xx\":%uA,"                                                             \
    "\"5xx\":%uA"                                                              \
    "},"                                                                       \
    "\"sessionMsec\":%M,"                                                      \
    "\"sessionMsecs\":{"                                                       \
    "\"times\":[%s],"                                                          \
    "\"msecs\":[%s]"                                                           \
    "},"                                                                       \
    "\"overCounts\":{"                                                         \
    "\"maxIntegerSize\":%s,"                                                   \
    "\"connectCounter\":%uA,"                                                  \
    "\"inBytes\":%uA,"                                                         \
    "\"outBytes\":%uA,"                                                        \
    "\"1xx\":%uA,"                                                             \
    "\"2xx\":%uA,"                                                             \
    "\"3xx\":%uA,"                                                             \
    "\"4xx\":%uA,"                                                             \
    "\"5xx\":%uA"                                                              \
    "}"                                                                        \
    "},"

#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_FILTER_S                \
    "\"streamFilterZones\":{"

#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_UPSTREAM_S              \
    "\"streamUpstreamZones\":{"
#define NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_UPSTREAM                \
    "{\"server\":\"%V\","                                                      \
    "\"connectCounter\":%uA,"                                                  \
    "\"inBytes\":%uA,"                                                         \
    "\"outBytes\":%uA,"                                                        \
    "\"responses\":{"                                                          \
    "\"1xx\":%uA,"                                                             \
    "\"2xx\":%uA,"                                                             \
    "\"3xx\":%uA,"                                                             \
    "\"4xx\":%uA,"                                                             \
    "\"5xx\":%uA"                                                              \
    "},"                                                                       \
    "\"sessionMsec\":%M,"                                                      \
    "\"sessionMsecs\":{"                                                       \
    "\"times\":[%s],"                                                          \
    "\"msecs\":[%s]"                                                           \
    "},"                                                                       \
    "\"uSessionMsec\":%M,"                                                     \
    "\"uSessionMsecs\":{"                                                      \
    "\"times\":[%s],"                                                          \
    "\"msecs\":[%s]"                                                           \
    "},"                                                                       \
    "\"uConnectMsec\":%M,"                                                     \
    "\"uConnectMsecs\":{"                                                      \
    "\"times\":[%s],"                                                          \
    "\"msecs\":[%s]"                                                           \
    "},"                                                                       \
    "\"uFirstByteMsec\":%M,"                                                   \
    "\"uFirstByteMsecs\":{"                                                    \
    "\"times\":[%s],"                                                          \
    "\"msecs\":[%s]"                                                           \
    "},"                                                                       \
    "\"weight\":%ui,"                                                          \
    "\"maxFails\":%ui,"                                                        \
    "\"failTimeout\":%T,"                                                      \
    "\"backup\":%s,"                                                           \
    "\"down\":%s,"                                                             \
    "\"overCounts\":{"                                                         \
    "\"maxIntegerSize\":%s,"                                                   \
    "\"connectCounter\":%uA,"                                                  \
    "\"inBytes\":%uA,"                                                         \
    "\"outBytes\":%uA,"                                                        \
    "\"1xx\":%uA,"                                                             \
    "\"2xx\":%uA,"                                                             \
    "\"3xx\":%uA,"                                                             \
    "\"4xx\":%uA,"                                                             \
    "\"5xx\":%uA"                                                              \
    "}"                                                                        \
    "},"

#define ngx_http_stream_server_traffic_status_add_oc(o, c) {                   \
    if (o->stat_connect_counter > c->stat_connect_counter) {                   \
        c->stat_connect_counter_oc++;                                          \
    }                                                                          \
    if (o->stat_in_bytes > c->stat_in_bytes) {                                 \
        c->stat_in_bytes_oc++;                                                 \
    }                                                                          \
    if (o->stat_out_bytes > c->stat_out_bytes) {                               \
        c->stat_out_bytes_oc++;                                                \
    }                                                                          \
    if (o->stat_1xx_counter > c->stat_1xx_counter) {                           \
        c->stat_1xx_counter_oc++;                                              \
    }                                                                          \
    if (o->stat_2xx_counter > c->stat_2xx_counter) {                           \
        c->stat_2xx_counter_oc++;                                              \
    }                                                                          \
    if (o->stat_3xx_counter > c->stat_3xx_counter) {                           \
        c->stat_3xx_counter_oc++;                                              \
    }                                                                          \
    if (o->stat_4xx_counter > c->stat_4xx_counter) {                           \
        c->stat_4xx_counter_oc++;                                              \
    }                                                                          \
    if (o->stat_5xx_counter > c->stat_5xx_counter) {                           \
        c->stat_5xx_counter_oc++;                                              \
    }                                                                          \
}

#define ngx_http_stream_server_traffic_status_group_to_string(n) (u_char *) (  \
    (n > 3)                                                                    \
    ? NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_UPSTREAMS                          \
    : NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_UPSTREAMS + 3 * n                  \
)

#define ngx_http_stream_server_traffic_status_max_integer                      \
    (NGX_ATOMIC_T_LEN < 12)                                                    \
    ? "4294967295"                                                             \
    : "18446744073709551615"

#define ngx_http_stream_server_traffic_status_boolean_to_string(b)             \
    (b) ? "true" : "false"


/* must be the same as ngx_stream_server_traffic_status_ctx_t */
typedef struct {
    ngx_rbtree_t                                   *rbtree;

    /* array of ngx_http_stream_server_traffic_status_filter_t */
    ngx_array_t                                    *filter_keys;

    /* array of ngx_http_stream_server_traffic_status_limit_t */
    ngx_array_t                                    *limit_traffics;

    /* array of ngx_http_stream_server_traffic_status_limit_t */
    ngx_array_t                                    *limit_filter_traffics;

    ngx_flag_t                                      enable;
    ngx_flag_t                                      filter_check_duplicate;
    ngx_flag_t                                      limit_check_duplicate;

    ngx_stream_upstream_main_conf_t                *upstream;
    ngx_str_t                                       shm_name;
    ssize_t                                         shm_size;
} ngx_http_stream_server_traffic_status_ctx_t;


typedef struct {
    ngx_shm_zone_t                                 *shm_zone;
    ngx_flag_t                                      enable;

    ngx_str_t                                       shm_name;
    ngx_http_stream_server_traffic_status_node_t    stats;
    ngx_msec_t                                      start_msec;
    ngx_flag_t                                      format;
    ngx_str_t                                       jsonp;

    ngx_rbtree_node_t                             **node_caches;
} ngx_http_stream_server_traffic_status_loc_conf_t;


ngx_msec_t ngx_http_stream_server_traffic_status_current_msec(void);

extern ngx_module_t ngx_http_stream_server_traffic_status_module;


#endif /* _NGX_HTTP_STREAM_STS_MODULE_H_INCLUDED_ */

/* vi:set ft=c ts=4 sw=4 et fdm=marker: */
