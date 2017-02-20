# vi:set ft=perl ts=4 sw=4 et fdm=marker:

use Test::Nginx::Socket;

plan tests => repeat_each() * blocks() * 4;
no_shuffle();
run_tests();

__DATA__

=== TEST 1: /status/control?cmd=delete&group=server&zone=*
--- main_config
    stream {
        server_traffic_status_zone;
        upstream backend {
            server localhost:1984;
        }
        server {
            listen 1985;
            server_traffic_status_filter_by_set_key $protocol protocol;
            proxy_pass backend;
        }
    }
--- http_config
    stream_server_traffic_status_zone;
--- config
    location /status {
        stream_server_traffic_status_display;
        stream_server_traffic_status_display_format json;
        access_log off;
    }
    location /stream {
        proxy_pass http://localhost:1985/return;
    }
--- user_files eval
[
    ['return/file.txt' => '{"return":"OK"}']
]
--- request eval
[
    'GET /stream/file.txt',
    "GET /status/control?cmd=delete&group=server&zone=*",
]
--- response_body_like eval
[
    'OK',
    '"processingCounts":[1-9]'
] 



=== TEST 2: /status/control?cmd=delete&group=filter&zone=*
--- main_config
    stream {
        server_traffic_status_zone;
        upstream backend {
            server localhost:1984;
        }
        server {
            listen 1985;
            server_traffic_status_filter_by_set_key $protocol protocol;
            proxy_pass backend;
        }
    }
--- http_config
    stream_server_traffic_status_zone;
--- config
    location /status {
        stream_server_traffic_status_display;
        stream_server_traffic_status_display_format json;
        access_log off;
    }
    location /stream {
        proxy_pass http://localhost:1985/return;
    }
--- user_files eval
[
    ['return/file.txt' => '{"return":"OK"}']
]
--- request eval
[
    'GET /stream/file.txt',
    'GET /status/control?cmd=delete&group=filter&zone=*',
]
--- response_body_like eval
[
    'OK',
    '"processingCounts":[1-9]'
]



=== TEST 3: /status/control?cmd=delete&group=upstream@group&zone=*
--- main_config
    stream {
        server_traffic_status_zone;
        upstream backend {
            server localhost:1984;
        }
        server {
            listen 1985;
            server_traffic_status_filter_by_set_key $protocol protocol;
            proxy_pass backend;
        }
    }
--- http_config
    stream_server_traffic_status_zone;
--- config
    location /status {
        stream_server_traffic_status_display;
        stream_server_traffic_status_display_format json;
        access_log off;
    }
    location /stream {
        proxy_pass http://localhost:1985/return;
    }
--- user_files eval
[
    ['return/file.txt' => '{"return":"OK"}']
]
--- request eval
[
    'GET /stream/file.txt',
    'GET /status/control?cmd=delete&group=upstream@group&zone=*',
]
--- response_body_like eval
[
    'OK',
    '"processingCounts":[1-9]'
]



=== TEST 4: /status/control?cmd=delete&group=upstream@alone&zone=*
--- main_config
    stream {
        server_traffic_status_zone;
        server {
            listen 1985;
            server_traffic_status_filter_by_set_key $protocol protocol;
            proxy_pass localhost:1984;
        }
    }
--- http_config
    stream_server_traffic_status_zone;
--- config
    location /status {
        stream_server_traffic_status_display;
        stream_server_traffic_status_display_format json;
        access_log off;
    }
    location /stream {
        proxy_pass http://localhost:1985/return;
    }
--- user_files eval
[
    ['return/file.txt' => '{"return":"OK"}']
]
--- request eval
[
    'GET /stream/file.txt',
    'GET /status/control?cmd=delete&group=upstream@alone&zone=*',
]
--- response_body_like eval
[
    'OK',
    '"processingCounts":[1-9]'
]
