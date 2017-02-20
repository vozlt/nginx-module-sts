# vi:set ft=perl ts=4 sw=4 et fdm=marker:

use Test::Nginx::Socket;

add_response_body_check(
    sub {
        my ($block, $body, $req_idx, $repeated_req_idx, $dry_run) = @_;
        system("echo '$body' | python -m json.tool > /dev/null") == 0 or
        bail_out "JSON Syntax error($body)";
    }
);

plan tests => repeat_each() * blocks() * 6;
no_shuffle();
run_tests();

__DATA__

=== TEST 1: check_json_syntax
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
    'GET /status/format/json',
    'GET /stream/file.txt',
    'GET /status/format/json'
]
--- response_body_like eval
[
    'nginxVersion',
    'OK',
    'streamFilterZones'
]
