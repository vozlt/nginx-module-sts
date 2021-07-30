
/*
 * Copyright (C) YoungJoo Kim (vozlt)
 */


#include <ngx_config.h>

#include "ngx_http_stream_server_traffic_status_module.h"
#include "ngx_http_stream_server_traffic_status_shm.h"
#include "ngx_http_stream_server_traffic_status_filter.h"
#include "ngx_http_stream_server_traffic_status_display_json.h"
#include "ngx_http_stream_server_traffic_status_display.h"


u_char *
ngx_http_stream_server_traffic_status_display_set_main(ngx_http_request_t *r,
    u_char *buf)
{
    ngx_atomic_int_t                                   ap, hn, ac, rq, rd, wr, wa;
    ngx_http_stream_server_traffic_status_loc_conf_t  *stscf;
    ngx_http_stream_server_traffic_status_shm_info_t  *shm_info;

    stscf = ngx_http_get_module_loc_conf(r, ngx_http_stream_server_traffic_status_module);

    ap = *ngx_stat_accepted;
    hn = *ngx_stat_handled;
    ac = *ngx_stat_active;
    rq = *ngx_stat_requests;
    rd = *ngx_stat_reading;
    wr = *ngx_stat_writing;
    wa = *ngx_stat_waiting;

    shm_info = ngx_pcalloc(r->pool, sizeof(ngx_http_stream_server_traffic_status_shm_info_t));
    if (shm_info == NULL) {
        return buf;
    }

    ngx_http_stream_server_traffic_status_shm_info(r, shm_info);

    buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_MAIN,
                      &ngx_cycle->hostname, NGINX_VERSION, stscf->start_msec,
                      ngx_http_stream_server_traffic_status_current_msec(),
                      ac, rd, wr, wa, ap, hn, rq,
                      shm_info->name, shm_info->max_size,
                      shm_info->used_size, shm_info->used_node);

    return buf;
}


u_char *
ngx_http_stream_server_traffic_status_display_set_server_node(
    ngx_http_request_t *r,
    u_char *buf, ngx_str_t *key,
    ngx_http_stream_server_traffic_status_node_t *stsn)
{
    ngx_int_t                                          rc;
    ngx_str_t                                          tmp, dst, protocol;
    ngx_http_stream_server_traffic_status_loc_conf_t  *stscf;

    stscf = ngx_http_get_module_loc_conf(r, ngx_http_stream_server_traffic_status_module);

    tmp = *key;

    (void) ngx_http_stream_server_traffic_status_node_position_key(&tmp, 1);

    rc = ngx_http_stream_server_traffic_status_escape_json_pool(r->pool, &dst, &tmp);
    if (rc != NGX_OK) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                      "display_set_server_node::escape_json_pool() failed");
    }

    protocol.len = 3;
    protocol.data = (u_char *) (stsn->protocol == SOCK_DGRAM ? "UDP" : "TCP");

    buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_SERVER,
                      &dst, stsn->port, &protocol,
                      stsn->stat_connect_counter,
                      stsn->stat_in_bytes,
                      stsn->stat_out_bytes,
                      stsn->stat_1xx_counter,
                      stsn->stat_2xx_counter,
                      stsn->stat_3xx_counter,
                      stsn->stat_4xx_counter,
                      stsn->stat_5xx_counter,
                      stsn->stat_session_time_counter,
                      ngx_http_stream_server_traffic_status_node_time_queue_average(
                          &stsn->stat_session_times, stscf->average_method,
                          stscf->average_period),
                      ngx_http_stream_server_traffic_status_display_get_time_queue_times(r,
                          &stsn->stat_session_times),
                      ngx_http_stream_server_traffic_status_display_get_time_queue_msecs(r,
                          &stsn->stat_session_times),
                      ngx_http_stream_server_traffic_status_display_get_histogram_bucket_msecs(r,
                          &stsn->stat_session_buckets),
                      ngx_http_stream_server_traffic_status_display_get_histogram_bucket_counters(r,
                          &stsn->stat_session_buckets),
                      ngx_http_stream_server_traffic_status_max_integer,
                      stsn->stat_connect_counter_oc,
                      stsn->stat_in_bytes_oc,
                      stsn->stat_out_bytes_oc,
                      stsn->stat_1xx_counter_oc,
                      stsn->stat_2xx_counter_oc,
                      stsn->stat_3xx_counter_oc,
                      stsn->stat_4xx_counter_oc,
                      stsn->stat_5xx_counter_oc,
                      stsn->stat_session_time_counter_oc);

    return buf;
}


u_char *
ngx_http_stream_server_traffic_status_display_set_server(ngx_http_request_t *r,
    u_char *buf, ngx_rbtree_node_t *node)
{
    ngx_str_t                                          key;
    ngx_http_stream_server_traffic_status_ctx_t       *ctx;
    ngx_http_stream_server_traffic_status_node_t      *stsn, ostsn;
    ngx_http_stream_server_traffic_status_loc_conf_t  *stscf;

    ctx = ngx_http_get_module_main_conf(r, ngx_http_stream_server_traffic_status_module);

    stscf = ngx_http_get_module_loc_conf(r, ngx_http_stream_server_traffic_status_module);

    if (node != ctx->rbtree->sentinel) {
        stsn = (ngx_http_stream_server_traffic_status_node_t *) &node->color;

        if (stsn->stat_upstream.type == NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_UPSTREAM_NO) {
            key.data = stsn->data;
            key.len = stsn->len;

            ostsn = stscf->stats;

            buf = ngx_http_stream_server_traffic_status_display_set_server_node(r, buf, &key, stsn);

            /* calculates the sum */
            stscf->stats.stat_connect_counter +=stsn->stat_connect_counter;
            stscf->stats.stat_in_bytes += stsn->stat_in_bytes;
            stscf->stats.stat_out_bytes += stsn->stat_out_bytes;
            stscf->stats.stat_1xx_counter += stsn->stat_1xx_counter;
            stscf->stats.stat_2xx_counter += stsn->stat_2xx_counter;
            stscf->stats.stat_3xx_counter += stsn->stat_3xx_counter;
            stscf->stats.stat_4xx_counter += stsn->stat_4xx_counter;
            stscf->stats.stat_5xx_counter += stsn->stat_5xx_counter;
            stscf->stats.stat_session_time_counter += stsn->stat_session_time_counter;
            ngx_http_stream_server_traffic_status_node_time_queue_merge(
                &stscf->stats.stat_session_times,
                &stsn->stat_session_times);

            stscf->stats.stat_connect_counter_oc += stsn->stat_connect_counter_oc;
            stscf->stats.stat_in_bytes_oc += stsn->stat_in_bytes_oc;
            stscf->stats.stat_out_bytes_oc += stsn->stat_out_bytes_oc;
            stscf->stats.stat_1xx_counter_oc += stsn->stat_1xx_counter_oc;
            stscf->stats.stat_2xx_counter_oc += stsn->stat_2xx_counter_oc;
            stscf->stats.stat_3xx_counter_oc += stsn->stat_3xx_counter_oc;
            stscf->stats.stat_4xx_counter_oc += stsn->stat_4xx_counter_oc;
            stscf->stats.stat_5xx_counter_oc += stsn->stat_5xx_counter_oc;
            stscf->stats.stat_session_time_counter_oc += stsn->stat_session_time_counter_oc;

            ngx_http_stream_server_traffic_status_add_oc((&ostsn), (&stscf->stats));
        }

        buf = ngx_http_stream_server_traffic_status_display_set_server(r, buf, node->left);
        buf = ngx_http_stream_server_traffic_status_display_set_server(r, buf, node->right);
    }

    return buf;
}


u_char *
ngx_http_stream_server_traffic_status_display_set_filter_node(ngx_http_request_t *r,
    u_char *buf, ngx_http_stream_server_traffic_status_node_t *stsn)
{
    ngx_str_t  key;

    key.data = stsn->data;
    key.len = stsn->len;

    (void) ngx_http_stream_server_traffic_status_node_position_key(&key, 2);

    return ngx_http_stream_server_traffic_status_display_set_server_node(r, buf, &key, stsn);
}


u_char *
ngx_http_stream_server_traffic_status_display_set_filter(ngx_http_request_t *r,
    u_char *buf, ngx_rbtree_node_t *node)
{
    ngx_str_t                                             key, filter;
    ngx_uint_t                                            i, j, n, rc;
    ngx_array_t                                          *filter_keys, *filter_nodes;
    ngx_http_stream_server_traffic_status_filter_key_t   *keys;
    ngx_http_stream_server_traffic_status_filter_node_t  *nodes;

    /* init array */
    filter_keys = NULL;
    filter_nodes = NULL;

    rc = ngx_http_stream_server_traffic_status_filter_get_keys(r, &filter_keys, node);

    if (filter_keys != NULL && rc == NGX_OK) {
        keys = filter_keys->elts;
        n = filter_keys->nelts;

        if (n > 1) {
            ngx_qsort(keys, (size_t) n,
                      sizeof(ngx_http_stream_server_traffic_status_filter_key_t),
                      ngx_http_stream_server_traffic_status_filter_cmp_keys);
        }

        ngx_memzero(&key, sizeof(ngx_str_t));

        for (i = 0; i < n; i++) {
            if (keys[i].key.len == key.len) {
                if (ngx_strncmp(keys[i].key.data, key.data, key.len) == 0) {
                    continue;
                }
            }
            key = keys[i].key;

            rc = ngx_http_stream_server_traffic_status_filter_get_nodes(r, &filter_nodes, &key, node);

            if (filter_nodes != NULL && rc == NGX_OK) {
                rc = ngx_http_stream_server_traffic_status_escape_json_pool(r->pool, &filter,
                                                                            &keys[i].key);
                if (rc != NGX_OK) {
                    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                                  "display_set_filter::escape_json_pool() failed");
                }

                buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_OBJECT_S,
                                  &filter);

                nodes = filter_nodes->elts;
                for (j = 0; j < filter_nodes->nelts; j++) {
                    buf = ngx_http_stream_server_traffic_status_display_set_filter_node(r, buf,
                              nodes[j].node);
                }

                buf--;
                buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_OBJECT_E);
                buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_NEXT);

                /* destory array to prevent duplication */
                if (filter_nodes != NULL) {
                    filter_nodes = NULL;
                }
            }

        }

        /* destory array */
        for (i = 0; i < n; i++) {
             if (keys[i].key.data != NULL) {
                 ngx_pfree(r->pool, keys[i].key.data);
             }
        }
        if (filter_keys != NULL) {
            filter_keys = NULL;
        }
    }

    return buf;
}


u_char *
ngx_http_stream_server_traffic_status_display_set_upstream_node(ngx_http_request_t *r,
     u_char *buf, ngx_stream_upstream_server_t *us,
#if nginx_version > 1007001
     ngx_http_stream_server_traffic_status_node_t *stsn
#else
     ngx_http_stream_server_traffic_status_node_t *stsn, ngx_str_t *name
#endif
     )
{
    ngx_int_t                                          rc;
    ngx_str_t                                          key;
    ngx_http_stream_server_traffic_status_loc_conf_t  *stscf;

    stscf = ngx_http_get_module_loc_conf(r, ngx_http_stream_server_traffic_status_module);

#if nginx_version > 1007001
    rc = ngx_http_stream_server_traffic_status_escape_json_pool(r->pool, &key, &us->name);
#else
    rc = ngx_http_stream_server_traffic_status_escape_json_pool(r->pool, &key, name);
#endif

    if (rc != NGX_OK) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                      "display_set_upstream_node::escape_json_pool() failed");
    }

    if (stsn != NULL) {
        buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_UPSTREAM,
                &key, stsn->stat_connect_counter,
                stsn->stat_in_bytes, stsn->stat_out_bytes,
                stsn->stat_1xx_counter, stsn->stat_2xx_counter,
                stsn->stat_3xx_counter, stsn->stat_4xx_counter,
                stsn->stat_5xx_counter,
                stsn->stat_session_time_counter,
                ngx_http_stream_server_traffic_status_node_time_queue_average(
                    &stsn->stat_session_times, stscf->average_method,
                    stscf->average_period),
                ngx_http_stream_server_traffic_status_display_get_time_queue_times(r,
                    &stsn->stat_session_times),
                ngx_http_stream_server_traffic_status_display_get_time_queue_msecs(r,
                    &stsn->stat_session_times),
                ngx_http_stream_server_traffic_status_display_get_histogram_bucket_msecs(r,
                    &stsn->stat_session_buckets),
                ngx_http_stream_server_traffic_status_display_get_histogram_bucket_counters(r,
                    &stsn->stat_session_buckets),
                stsn->stat_upstream.session_time_counter,
                ngx_http_stream_server_traffic_status_node_time_queue_average(
                    &stsn->stat_upstream.session_times, stscf->average_method,
                    stscf->average_period),
                ngx_http_stream_server_traffic_status_display_get_time_queue_times(r,
                    &stsn->stat_upstream.session_times),
                ngx_http_stream_server_traffic_status_display_get_time_queue_msecs(r,
                    &stsn->stat_upstream.session_times),
                ngx_http_stream_server_traffic_status_display_get_histogram_bucket_msecs(r,
                    &stsn->stat_upstream.session_buckets),
                ngx_http_stream_server_traffic_status_display_get_histogram_bucket_counters(r,
                    &stsn->stat_upstream.session_buckets),
                stsn->stat_upstream.connect_time_counter,
                ngx_http_stream_server_traffic_status_node_time_queue_average(
                    &stsn->stat_upstream.connect_times, stscf->average_method,
                    stscf->average_period),
                ngx_http_stream_server_traffic_status_display_get_time_queue_times(r,
                    &stsn->stat_upstream.connect_times),
                ngx_http_stream_server_traffic_status_display_get_time_queue_msecs(r,
                    &stsn->stat_upstream.connect_times),
                ngx_http_stream_server_traffic_status_display_get_histogram_bucket_msecs(r,
                    &stsn->stat_upstream.connect_buckets),
                ngx_http_stream_server_traffic_status_display_get_histogram_bucket_counters(r,
                    &stsn->stat_upstream.connect_buckets),
                stsn->stat_upstream.first_byte_time_counter,
                ngx_http_stream_server_traffic_status_node_time_queue_average(
                    &stsn->stat_upstream.first_byte_times, stscf->average_method,
                    stscf->average_period),
                ngx_http_stream_server_traffic_status_display_get_time_queue_times(r,
                    &stsn->stat_upstream.first_byte_times),
                ngx_http_stream_server_traffic_status_display_get_time_queue_msecs(r,
                    &stsn->stat_upstream.first_byte_times),
                ngx_http_stream_server_traffic_status_display_get_histogram_bucket_msecs(r,
                    &stsn->stat_upstream.first_byte_buckets),
                ngx_http_stream_server_traffic_status_display_get_histogram_bucket_counters(r,
                    &stsn->stat_upstream.first_byte_buckets),
                us->weight, us->max_fails,
                us->fail_timeout,
                ngx_http_stream_server_traffic_status_boolean_to_string(us->backup),
                ngx_http_stream_server_traffic_status_boolean_to_string(us->down),
                ngx_http_stream_server_traffic_status_max_integer,
                stsn->stat_connect_counter_oc, stsn->stat_in_bytes_oc,
                stsn->stat_out_bytes_oc, stsn->stat_1xx_counter_oc,
                stsn->stat_2xx_counter_oc, stsn->stat_3xx_counter_oc,
                stsn->stat_4xx_counter_oc, stsn->stat_5xx_counter_oc,
                stsn->stat_session_time_counter_oc, stsn->stat_u_session_time_counter_oc,
                stsn->stat_u_connect_time_counter_oc, stsn->stat_u_first_byte_time_counter_oc);

    } else {
        buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_UPSTREAM,
                &key, (ngx_atomic_uint_t) 0,
                (ngx_atomic_uint_t) 0, (ngx_atomic_uint_t) 0,
                (ngx_atomic_uint_t) 0, (ngx_atomic_uint_t) 0,
                (ngx_atomic_uint_t) 0, (ngx_atomic_uint_t) 0,
                (ngx_atomic_uint_t) 0,
                (ngx_atomic_uint_t) 0,
                (ngx_msec_t) 0,
                (u_char *) "", (u_char *) "",
                (u_char *) "", (u_char *) "",
                (ngx_atomic_uint_t) 0,
                (ngx_msec_t) 0,
                (u_char *) "", (u_char *) "",
                (u_char *) "", (u_char *) "",
                (ngx_atomic_uint_t) 0,
                (ngx_msec_t) 0,
                (u_char *) "", (u_char *) "",
                (u_char *) "", (u_char *) "",
                (ngx_atomic_uint_t) 0,
                (ngx_msec_t) 0,
                (u_char *) "", (u_char *) "",
                (u_char *) "", (u_char *) "",
                us->weight, us->max_fails,
                us->fail_timeout,
                ngx_http_stream_server_traffic_status_boolean_to_string(us->backup),
                ngx_http_stream_server_traffic_status_boolean_to_string(us->down),
                ngx_http_stream_server_traffic_status_max_integer,
                (ngx_atomic_uint_t) 0, (ngx_atomic_uint_t) 0,
                (ngx_atomic_uint_t) 0, (ngx_atomic_uint_t) 0,
                (ngx_atomic_uint_t) 0, (ngx_atomic_uint_t) 0,
                (ngx_atomic_uint_t) 0, (ngx_atomic_uint_t) 0,
                (ngx_atomic_uint_t) 0, (ngx_atomic_uint_t) 0,
                (ngx_atomic_uint_t) 0, (ngx_atomic_uint_t) 0);
    }

    return buf;
}


u_char *
ngx_http_stream_server_traffic_status_display_set_upstream_alone(ngx_http_request_t *r,
    u_char *buf, ngx_rbtree_node_t *node)
{
    unsigned                                       type;
    ngx_str_t                                      key;
    ngx_stream_upstream_server_t                   us;
    ngx_http_stream_server_traffic_status_ctx_t   *ctx;
    ngx_http_stream_server_traffic_status_node_t  *stsn;

    ctx = ngx_http_get_module_main_conf(r, ngx_http_stream_server_traffic_status_module);

    type = NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_UPSTREAM_UA;

    if (node != ctx->rbtree->sentinel) {
        stsn = (ngx_http_stream_server_traffic_status_node_t *) &node->color;

        if (stsn->stat_upstream.type == type) {
            key.len = stsn->len;
            key.data = stsn->data;

            (void) ngx_http_stream_server_traffic_status_node_position_key(&key, 1);

#if nginx_version > 1007001
            us.name = key;
#endif
            us.weight = 0;
            us.max_fails = 0;
            us.fail_timeout = 0;
            us.down = 0;
            us.backup = 0;

#if nginx_version > 1007001
            buf = ngx_http_stream_server_traffic_status_display_set_upstream_node(r, buf, &us, stsn);
#else
            buf = ngx_http_stream_server_traffic_status_display_set_upstream_node(r, buf, &us, stsn, &key);
#endif
        }

        buf = ngx_http_stream_server_traffic_status_display_set_upstream_alone(r, buf, node->left);
        buf = ngx_http_stream_server_traffic_status_display_set_upstream_alone(r, buf, node->right);
    }

    return buf;
}


u_char *
ngx_http_stream_server_traffic_status_display_set_upstream_group(ngx_http_request_t *r,
    u_char *buf)
{
    size_t                                         len;
    u_char                                        *p, *o, *s;
    uint32_t                                       hash;
    unsigned                                       type, zone;
    ngx_int_t                                      rc;
    ngx_str_t                                      key, dst;
    ngx_uint_t                                     i, j, k;
    ngx_rbtree_node_t                             *node;
    ngx_stream_upstream_server_t                  *us, usn;
#if (NGX_STREAM_UPSTREAM_ZONE)
    ngx_stream_upstream_rr_peer_t                 *peer;
    ngx_stream_upstream_rr_peers_t                *peers;
#endif
    ngx_stream_upstream_srv_conf_t                *uscf, **uscfp;
    ngx_stream_upstream_main_conf_t               *umcf;
    ngx_http_stream_server_traffic_status_ctx_t   *ctx;
    ngx_http_stream_server_traffic_status_node_t  *stsn;

    ctx = ngx_http_get_module_main_conf(r, ngx_http_stream_server_traffic_status_module);
    umcf = ctx->upstream;
    uscfp = umcf->upstreams.elts;

    len = 0;
    for (i = 0; i < umcf->upstreams.nelts; i++) {
        uscf = uscfp[i];
        len = ngx_max(uscf->host.len, len);
    }

    dst.len = len + sizeof("@[ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255]:65535") - 1;
    dst.data = ngx_pnalloc(r->pool, dst.len);
    if (dst.data == NULL) {
        return buf;
    }

    p = dst.data;

    for (i = 0; i < umcf->upstreams.nelts; i++) {

        uscf = uscfp[i];

        /* groups */
        if (uscf->servers && !uscf->port) {
            us = uscf->servers->elts;

            type = NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_UPSTREAM_UG;

            o = buf;

            buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_ARRAY_S,
                              &uscf->host);
            s = buf;

            zone = 0;

#if (NGX_STREAM_UPSTREAM_ZONE)
            if (uscf->shm_zone == NULL) {
                goto not_supported;
            }

            zone = 1;

            peers = uscf->peer.data;

            ngx_stream_upstream_rr_peers_rlock(peers);

            for (peer = peers->peer; peer ; peer = peer->next) {
                p = ngx_cpymem(p, uscf->host.data, uscf->host.len);
                *p++ = NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_KEY_SEPARATOR;
                p = ngx_cpymem(p, peer->name.data, peer->name.len);

                dst.len = uscf->host.len + sizeof("@") - 1 + peer->name.len;

                rc = ngx_http_stream_server_traffic_status_node_generate_key(r->pool, &key, &dst, type);
                if (rc != NGX_OK) {
                    ngx_stream_upstream_rr_peers_unlock(peers);
                    return buf;
                }

                hash = ngx_crc32_short(key.data, key.len);
                node = ngx_http_stream_server_traffic_status_node_lookup(ctx->rbtree, &key, hash);

                usn.weight = peer->weight;
                usn.max_fails = peer->max_fails;
                usn.fail_timeout = peer->fail_timeout;
                usn.backup = 0;
                usn.down = peer->down;

#if nginx_version > 1007001
                usn.name = peer->name;
#endif

                if (node != NULL) {
                    stsn = (ngx_http_stream_server_traffic_status_node_t *) &node->color;
#if nginx_version > 1007001
                    buf = ngx_http_stream_server_traffic_status_display_set_upstream_node(r, buf, &usn, stsn);
#else
                    buf = ngx_http_stream_server_traffic_status_display_set_upstream_node(r, buf, &usn, stsn, &peer->name);
#endif

                } else {
#if nginx_version > 1007001
                    buf = ngx_http_stream_server_traffic_status_display_set_upstream_node(r, buf, &usn, NULL);
#else
                    buf = ngx_http_stream_server_traffic_status_display_set_upstream_node(r, buf, &usn, NULL, &peer->name);
#endif
                }

                p = dst.data;
            }

            ngx_stream_upstream_rr_peers_unlock(peers);

not_supported:

#endif

            for (j = 0; j < uscf->servers->nelts; j++) {
                usn = us[j];

                if (zone && usn.backup != 1) {
                    continue;
                }

                /* for all A records */
                for (k = 0; k < usn.naddrs; k++) {
                    p = ngx_cpymem(p, uscf->host.data, uscf->host.len);
                    *p++ = NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_KEY_SEPARATOR;
                    p = ngx_cpymem(p, usn.addrs[k].name.data, usn.addrs[k].name.len);

                    dst.len = uscf->host.len + sizeof("@") - 1 + usn.addrs[k].name.len;

                    rc = ngx_http_stream_server_traffic_status_node_generate_key(r->pool, &key, &dst, type);
                    if (rc != NGX_OK) {
                        return buf;
                    }

                    hash = ngx_crc32_short(key.data, key.len);
                    node = ngx_http_stream_server_traffic_status_node_lookup(ctx->rbtree, &key, hash);

#if nginx_version > 1007001
                    usn.name = usn.addrs[k].name;
#endif

                    if (node != NULL) {
                        stsn = (ngx_http_stream_server_traffic_status_node_t *) &node->color;
#if nginx_version > 1007001
                        buf = ngx_http_stream_server_traffic_status_display_set_upstream_node(r, buf, &usn, stsn);
#else
                        buf = ngx_http_stream_server_traffic_status_display_set_upstream_node(r, buf, &usn, stsn, &usn.addrs[k].name);
#endif

                    } else {
#if nginx_version > 1007001
                        buf = ngx_http_stream_server_traffic_status_display_set_upstream_node(r, buf, &usn, NULL);
#else
                        buf = ngx_http_stream_server_traffic_status_display_set_upstream_node(r, buf, &usn, NULL, &usn.addrs[k].name);
#endif
                    }

                    p = dst.data;
                }
            }

            if (s == buf) {
                buf = o;

            } else {
                buf--;
                buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_ARRAY_E);
                buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_NEXT);
            }
        }
    }

    /* alones */
    o = buf;

    ngx_str_set(&key, "::nogroups");

    buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_ARRAY_S, &key);

    s = buf;

    buf = ngx_http_stream_server_traffic_status_display_set_upstream_alone(r, buf, ctx->rbtree->root);

    if (s == buf) {
        buf = o;

    } else {
        buf--;
        buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_ARRAY_E);
        buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_NEXT);
    }

    return buf;
}


u_char *
ngx_http_stream_server_traffic_status_display_set(ngx_http_request_t *r,
    u_char *buf)
{
    u_char                                            *o, *s;
    ngx_str_t                                          stats;
    ngx_rbtree_node_t                                 *node;
    ngx_http_stream_server_traffic_status_ctx_t       *ctx;
    ngx_http_stream_server_traffic_status_loc_conf_t  *stscf;

    ctx = ngx_http_get_module_main_conf(r, ngx_http_stream_server_traffic_status_module);

    stscf = ngx_http_get_module_loc_conf(r, ngx_http_stream_server_traffic_status_module);

    node = ctx->rbtree->root;

    /* init stats */
    ngx_memzero(&stscf->stats, sizeof(stscf->stats));
    ngx_http_stream_server_traffic_status_node_time_queue_init(&stscf->stats.stat_session_times);

    /* main & connections */
    buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_S);

    buf = ngx_http_stream_server_traffic_status_display_set_main(r, buf);

    /* serverZones */
    buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_SERVER_S);

    buf = ngx_http_stream_server_traffic_status_display_set_server(r, buf, node);

    ngx_str_set(&stats, "*");

    buf = ngx_http_stream_server_traffic_status_display_set_server_node(r, buf, &stats,
                                                                        &stscf->stats);

    buf--;
    buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_E);
    buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_NEXT);

    /* filterZones */
    ngx_memzero(&stscf->stats, sizeof(stscf->stats));

    o = buf;

    buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_FILTER_S);

    s = buf;

    buf = ngx_http_stream_server_traffic_status_display_set_filter(r, buf, node);

    if (s == buf) {
        buf = o;

    } else {
        buf--;
        buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_E);
        buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_NEXT);
    }

    /* upstreamZones */
    o = buf;

    buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_UPSTREAM_S);

    s = buf;

    buf = ngx_http_stream_server_traffic_status_display_set_upstream_group(r, buf);

    if (s == buf) {
        buf = o;
        buf--;

    } else {
        buf--;
        buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_E);
    }

    buf = ngx_sprintf(buf, NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_JSON_FMT_E);

    return buf;
}

/* vi:set ft=c ts=4 sw=4 et fdm=marker: */
