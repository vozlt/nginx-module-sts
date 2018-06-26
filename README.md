Nginx stream server traffic status module
==========

[![License](http://img.shields.io/badge/license-BSD-brightgreen.svg)](https://github.com/vozlt/nginx-module-sts/blob/master/LICENSE)

Nginx stream server traffic status module

Table of Contents
=================

* [Version](#version)
* [Dependencies](#dependencies)
* [Screenshots](#screenshots)
* [Installation](#installation)
* [Synopsis](#synopsis)
* [Description](#description)
* [Control](#control)
  * [To get status of traffic zones on the fly](#to-get-status-of-traffic-zones-on-the-fly)
    * [To get fully zones](#to-get-fully-zones)
    * [To get group zones](#to-get-group-zones)
    * [To get each zones](#to-get-each-zones)
  * [To reset traffic zones on the fly](#to-reset-traffic-zones-on-the-fly)
    * [To reset fully zones](#to-reset-fully-zones)
    * [To reset group zones](#to-reset-group-zones)
    * [To reset each zones](#to-reset-each-zones)
  * [To delete traffic zones on the fly](#to-delete-traffic-zones-on-the-fly)
    * [To delete fully zones](#to-delete-fully-zones)
    * [To delete group zones](#to-delete-group-zones)
    * [To delete each zones](#to-delete-each-zones)
* [JSON](#json)
  * [Json used by status](#json-used-by-status)
  * [Json used by control](#json-used-by-control)
* [Variables](#variables)
* [Limit](#limit)
  * [To limit traffic for server](#to-limit-traffic-for-server)
  * [To limit traffic for filter](#to-limit-traffic-for-filter)
  * [To limit traffic for upstream](#to-limit-traffic-for-upstream)
* [Use cases](#use-cases)
  * [To calculate traffic for individual country using GeoIP](#to-calculate-traffic-for-individual-country-using-geoip)
* [Customizing](#customizing)
  * [To customize after the module installed](#to-customize-after-the-module-installed)
  * [To customize before the module installed](#to-customize-before-the-module-installed)
* [Directives](#directives)
  * [stream_server_traffic_status](#stream_server_traffic_status)
  * [stream_server_traffic_status_zone](#stream_server_traffic_status_zone)
  * [stream_server_traffic_status_display](#stream_server_traffic_status_display)
  * [stream_server_traffic_status_display_format](#stream_server_traffic_status_display_format)
  * [stream_server_traffic_status_display_jsonp](#stream_server_traffic_status_display_jsonp)
  * [stream_server_traffic_status_average_method](#stream_server_traffic_status_average_method)
  * [server_traffic_status](#server_traffic_status)
  * [server_traffic_status_zone](#server_traffic_status_zone)
  * [server_traffic_status_filter](#server_traffic_status_filter)
  * [server_traffic_status_filter_by_set_key](#server_traffic_status_filter_by_set_key)
  * [server_traffic_status_filter_check_duplicate](#server_traffic_status_filter_check_duplicate)
  * [server_traffic_status_limit](#server_traffic_status_limit)
  * [server_traffic_status_limit_traffic](#server_traffic_status_limit_traffic)
  * [server_traffic_status_limit_traffic_by_set_key](#server_traffic_status_limit_traffic_by_set_key)
  * [server_traffic_status_limit_check_duplicate](#server_traffic_status_limit_check_duplicate)
  * [server_traffic_status_average_method](#server_traffic_status_average_method)
  * [server_traffic_status_histogram_buckets](#server_traffic_status_histogram_buckets)
* [See Also](#see-also)
* [TODO](#todo)
* [Donation](#donation)
* [Author](#author)

## Version
This document describes nginx-module-sts `v0.1.0` released on 20 Feb 2017.

## Dependencies
* [nginx](http://nginx.org)
* [nginx-module-stream-sts](https://github.com/vozlt/nginx-module-stream-sts)

## Compatibility
* Nginx
  * 1.11.5 \<= (last tested: 1.15.0)

Earlier versions does not work.

## Screenshots
![nginx-module-sts screenshot](https://cloud.githubusercontent.com/assets/3648408/23112117/e8c56cda-f770-11e6-9c68-f57cbf4dd542.png "screenshot with deault")

## Installation

1. Clone the git repository.

  ```
  shell> git clone git://github.com/vozlt/nginx-module-sts.git
  ```
  ```
  shell> git clone git://github.com/vozlt/nginx-module-stream-sts.git
  ```

2. Add the module to the build configuration by adding
  ```
  --add-module=/path/to/nginx-module-sts
  --add-module=/path/to/nginx-module-stream-sts
  ```

3. Build the nginx binary.

4. Install the nginx binary.

## Synopsis

```Nginx
http {
    stream_server_traffic_status_zone;

    ...

    server {

        ...

        location /status {
            stream_server_traffic_status_display;
            stream_server_traffic_status_display_format html;
        }
    }
}

stream {
    server_traffic_status_zone;

    ...

    server {
        ...
    }
}
```

## Description
This is an Nginx module that provides access to stream server traffic status information.
This is a porting version of the [nginx-module-vts](https://github.com/vozlt/nginx-module-vts) to the NGINX "stream" subsystem so as to support the same features in [nginx-module-vts](https://github.com/vozlt/nginx-module-vts).
It contains the current status such as servers, upstreams, user-defined filter.


First of all, It is required both the directive `server_traffic_status_zone` in stream block and `stream_server_traffic_status_zone` in http block, and then if the directive `stream_server_traffic_status_display` is set, can be access to as follows:

* /status/format/json

* /status/format/html

* /status/format/jsonp

* /status/control

  * If you request `/status/format/json`, will respond with a JSON document containing the current activity data for using in live dashboards and third-party monitoring tools.

  * If you request `/status/format/html`, will respond with the built-in live dashboard in HTML that requests internally to `/status/format/json`.
 
  * If you request `/status/format/jsonp`, will respond with a JSONP callback function containing the current activity data for using in live dashboards and third-party monitoring tools. 

  * If you request `/status/control`, will respond with a JSON document after it reset or delete zones through a query string. See the [Control](#control).

JSON document contains as follows:

```Json
{
    "hostName": ...,
    "nginxVersion": ...,
    "loadMsec": ...,
    "nowMsec": ...,
    "connections": {
        "active":...,
        "reading":...,
        "writing":...,
        "waiting":...,
        "accepted":...,
        "handled":...,
        "requests":...
    },
    "streamServerZones": {
        "...":{
            "port":...,
            "protocol":...,
            "connectCounter":...,
            "inBytes":...,
            "outBytes":...,
            "responses":{
                "1xx":...,
                "2xx":...,
                "3xx":...,
                "4xx":...,
                "5xx":...,
            },
            "sessionMsec":...,
            "sessionMsecs":{
                "times":[...],
                "msecs":[...]
            },
        }
        ...
    },
    "streamFilterZones": {
        "...":{
            "...":{

                "port":...,
                "protocol":...,
                "connectCounter":...,
                "inBytes":...,
                "outBytes":...,
                "responses":{
                    "1xx":...,
                    "2xx":...,
                    "3xx":...,
                    "4xx":...,
                    "5xx":...,
                },
                "sessionMsec":...,
                "sessionMsecs":{
                    "times":[...],
                    "msecs":[...]
                },
            },
            ...
        },
        ...
    },
    "streamUpstreamZones": {
        "...":[
            {
                "server":...,
                "connectCounter":...,
                "inBytes":...,
                "outBytes":...,
                "responses":{
                    "1xx":...,
                    "2xx":...,
                    "3xx":...,
                    "4xx":...,
                    "5xx":...
                },
                "sessionMsec":...,
                "sessionMsecs":{
                    "times":[...],
                    "msecs":[...]
                },
                "uSessionMsec":...,
                "uSessionMsecs":{
                    "times":[...],
                    "msecs":[...]
                },
                "uConnectMsec":...,
                "uConnectMsecs":{
                    "times":[...],
                    "msecs":[...]
                },
                "uFirstByteMsec":...,
                "uFirstByteMsecs":{
                    "times":[...],
                    "msecs":[...]
                },
                "weight":...,
                "maxFails":...,
                "failTimeout":...,
                "backup":...,
                "down":...
            }
            ...
        ],
        ...
    }
}
```

* main
  * Basic version, uptime((nowMsec - loadMsec)/1000)
  * nowMsec, loadMsec is a millisecond.
* connections
  * Total connections and requests(same as stub_status_module in NGINX)
* streamServerZones
  * Traffic(in/out) and request and response counts and status(1xx,2xx...) hit ratio per each server zone
  * Total traffic(In/Out) and request and response counts(It zone name is `*`) and hit ratio
* streamFilterZones
  * Traffic(in/out) and request and response counts and status(1xx,2xx...) hit ratio per each server zone filtered through the `server_traffic_status_filter_by_set_key` directive
  * Total traffic(In/Out) and request and response counts(It zone name is `*`) and hit ratio filtered through the `server_traffic_status_filter_by_set_key` directive
* streamUpstreamZones
  * Traffic(in/out) and request and response counts per server in each upstream group
  * Current settings(weight, maxfails, failtimeout...) in nginx.conf

The directive `stream_server_traffic_status_display_format` sets the default ouput format that is one of json or html. (Default: json)

Traffic calculation as follows:

* streamServerZones
  * in += requested_bytes
  * out += sent_bytes
* streamFilterZones
  * in += requested_bytes via the filter
  * out += sent_bytes via the filter
* streamUpstreamZones
  * in += requested_bytes via the ServerZones
  * out += sent_bytes via the ServerZones

All calculations are working in log processing phase of Nginx.

`Caveats:` this module relies on nginx logging system(NGX_STREAM_LOG_PHASE:last phase of the nginx stream), so the traffic may be
in certain cirumstances different that real bandwidth traffic.
Websocket, canceled downloads may be cause of inaccuracies.
The working of the module doesn't matter at all whether the access_log directive "on" or "off".
Again, this module works well on "access_log off".

## Control
It is able to reset or delete traffic zones through a query string.
The request responds with a JSON document.

* URI Syntax
  * /*`{status_uri}`*/control?cmd=*`{command}`*&group=*`{group}`*&zone=*`{name}`*

```Nginx
http {

    stream_server_traffic_status_zone;

    ...

    server {

        server_name example.org;

        ...


        location /status {
            stream_server_traffic_status_display;
            stream_server_traffic_status_display_format html;
        }
    }                                                                                                                                                                                           }
}

stream {
    geoip_country    /usr/share/GeoIP/GeoIP.dat;

    server_traffic_status_zone;

    server_traffic_status_filter_by_set_key $geoip_country_code country::*;

    server {

        ...

    }

    ...

}
```

If it set as above, then the control uri is like `example.org/status/control`.

The available request arguments are as follows:
* **cmd**=\<`status`\|`reset`\|`delete`\>
  * status
    * It returns status of traffic zones to json format like `status/format/json`.
  * reset
    * It reset traffic zones without deleting nodes in shared memory.(= init to 0)
  * delete
    * It delete traffic zones in shared memory. when re-request recreated. 
* **group**=\<`server`\|`filter`\|`upstream@alone`\|`upstream@group`\|`*`\>
  * server
  * filter
  * upstream@alone
  * upstream@group
  * \*
* **zone**=*name*
  * server
    * *name*
  * filter
    * *filter_group*@*name*
  * upstream@group
    * *upstream_group*@*name*
  * upstream@alone
    * @*name*

### To get status of traffic zones on the fly
This is similar to the `status/format/json` except that it can get each zones.

#### To get fully zones
* It is exactly the same with the `status/format/json`.
  * /status/control?cmd=status&group=*

#### To get group zones
* streamServerZones
  * /status/control?cmd=status&group=server&zone=*
* streamFilterZones
  * /status/control?cmd=status&group=filter&zone=*
* streamUpstreamZones
  * /status/control?cmd=status&group=upstream@group&zone=*
* streamUpstreamZones::nogroups
  * /status/control?cmd=status&group=upstream@alone&zone=*

#### To get each zones
* single zone in streamServerZones
  * /status/control?cmd=status&group=server&zone=*`name`*
* single zone in streamFilterZones
  * /status/control?cmd=status&group=filter&zone=*`filter_group`*@*`name`*
* single zone in streamUpstreamZones
  * /status/control?cmd=status&group=upstream@group&zone=*`upstream_group`*@*`name`*
* single zone in streamUpstreamZones::nogroups
  * /status/control?cmd=status&group=upstream@alone&zone=*`name`*

### To reset traffic zones on the fly
It reset the values of specified zones to 0.

#### To reset fully zones
* /status/control?cmd=reset&group=*

#### To reset group zones
* streamServerZones
  * /status/control?cmd=reset&group=server&zone=*
* streamFilterZones
  * /status/control?cmd=reset&group=filter&zone=*
* streamUpstreamZones
  * /status/control?cmd=reset&group=upstream@group&zone=*
* streamUpstreamZones::nogroups
  * /status/control?cmd=reset&group=upstream@alone&zone=*

#### To reset each zones
* single zone in streamServerZones
  * /status/control?cmd=reset&group=server&zone=*`name`*
* single zone in streamFilterZones
  * /status/control?cmd=reset&group=filter&zone=*`filter_group`*@*`name`*
* single zone in streamUpstreamZones
  * /status/control?cmd=reset&group=upstream@group&zone=*`upstream_group`*@*`name`*
* single zone in streamUpstreamZones::nogroups
  * /status/control?cmd=reset&group=upstream@alone&zone=*`name`*

### To delete traffic zones on the fly
It delete the specified zones in shared memory.

#### To delete fully zones
* /status/control?cmd=delete&group=*

#### To delete group zones
* streamServerZones
  * /status/control?cmd=delete&group=server&zone=*
* streamFilterZones
  * /status/control?cmd=delete&group=filter&zone=*
* streamUpstreamZones
  * /status/control?cmd=delete&group=upstream@group&zone=*
* streamUpstreamZones::nogroups
  * /status/control?cmd=delete&group=upstream@alone&zone=*

#### To delete each zones
* single zone in streamServerZones
  * /status/control?cmd=delete&group=server&zone=*`name`*
* single zone in streamFilterZones
  * /status/control?cmd=delete&group=filter&zone=*`filter_group`*@*`name`*
* single zone in streamUpstreamZones
  * /status/control?cmd=delete&group=upstream@group&zone=*`upstream_group`*@*`name`*
* single zone in streamUpstreamZones::nogroups
  * /status/control?cmd=delete&group=upstream@alone&zone=*`name`*

## JSON
The following status information is provided in the JSON format:

### Json used by status
/*`{status_uri}`*/format/json

/*`{status_uri}`*/control?cmd=status&...

* hostName
  * Host name.
* nginxVersion
  * Version of the provided.
* loadMsec
  * Loaded process time in milliseconds.
* nowMsec
  * Current time in milliseconds
* connections
  * active
    * The current number of active client connections.
  * reading
    * The total number of reading client connections.
  * writing
    * The total number of writing client connections.
  * waiting
    * The total number of wating client connections.
  * accepted
    * The total number of accepted client connections.
  * handled
    * The total number of handled client connections.
  * requests
    * The total number of requested client connections.
* streamServerZones
  * connectCounter
    * The total number of client requests received from clients.
  * inBytes
    * The total number of bytes received from clients.
  * outBytes
    * The total number of bytes sent to clients.
  * responses
    * 1xx, 2xx, 3xx, 4xx, 5xx
      * The number of responses with status codes 1xx, 2xx, 3xx, 4xx, and 5xx.
  * sessionMsec
    * The average of request processing times in milliseconds.
  * sessionMsecs
    * times
      * The times in milliseconds at request processing times.
    * msecs
      * The request processing times in milliseconds.
* streamFilterZones
  * It provides the same fields with `streamServerZones` except that it included group names.
* streamUpstreamZones
  * server
    * An address of the server.
  * connectCounter
    * The total number of client connections forwarded to this server.
  * inBytes
    * The total number of bytes received from this server.
  * outBytes
    * The total number of bytes sent to this server.
  * responses
    * 1xx, 2xx, 3xx, 4xx, 5xx
      * The number of responses with status codes 1xx, 2xx, 3xx, 4xx, and 5xx.
  * sessionMsec
    * The average of request processing times including upstream in milliseconds.
  * sessionMsecs
    * times
      * The times in milliseconds at request processing times.
    * msecs
      * The request processing times including upstream in milliseconds.
  * uSessionMsec
    * The average of only upstream response processing times in milliseconds.
  * uSessionMsecs
    * times
      * The times in milliseconds at request processing times.
    * msecs
      * The only upstream response processing times in milliseconds.
  * uConnectMsec
    * The average of only upstream times to connect to the upstream server (1.11.4) in milliseconds.
  * uConnectMsecs
    * times
      * The times in milliseconds at request processing times.
    * msecs
      * The only upstream response processing times in milliseconds.
  * uFirstByteMsec
    * The average of only upstream times to receive the first byte of data (1.11.4) in milliseconds.
  * uFirstByteMsecs
    * times
      * The times in milliseconds at request processing times.
    * msecs
      * The only upstream response processing times in milliseconds.
  * weight
    * Current `weight` setting of the server.
  * maxFails
    * Current `max_fails` setting of the server.
  * failTimeout
    * Current `fail_timeout` setting of the server.
  * backup
    * Current `backup` setting of the server.
  * down
    * Current `down` setting of the server.

### Json used by control
/*`{status_uri}`*/control?cmd=reset&...

/*`{status_uri}`*/control?cmd=delete&...

* processingReturn
  * The result of true or false.
* processingCommandString
  * The requested command string.
* processingGroupString
  * The requested group string.
* processingZoneString
  * The requested zone string.
* processingCounts
  * The actual processing number.

## Variables
The following embedded variables are provided in `stream` block:

* **$sts_connect_counter**
  * The total number of client requests received from clients.
* **$sts_in_bytes**
  * The total number of bytes received from clients.
* **$sts_out_bytes**
  * The total number of bytes sent to clients.
* **$sts_1xx_counter**
  * The number of responses with status codes 1xx.
* **$sts_2xx_counter**
  * The number of responses with status codes 2xx.
* **$sts_3xx_counter**
  * The number of responses with status codes 3xx.
* **$sts_4xx_counter**
  * The number of responses with status codes 4xx.
* **$sts_5xx_counter**
  * The number of responses with status codes 5xx.
* **$sts_session_time**
  * The average of request processing times.

## Limit

It is able to limit total traffic per each server by using the directive
[`server_traffic_status_limit_traffic`](#server_traffic_status_limit_traffic).
It also is able to limit all traffic by using the directive
[`server_traffic_status_limit_traffic_by_set_key`](#server_traffic_status_limit_traffic_by_set_key).
When the limit is exceeded, the server will return the 503
(Service Temporarily Unavailable) error in reply to a request. 
The return code can be changeable.

### To limit traffic for server
```Nginx
stream {

    server_traffic_status_zone;

    ...

    server {

        listen 1981;

        server_traffic_status_limit_traffic in:64G;
        server_traffic_status_limit_traffic out:1024G;

        ...
    }
}
```

* Limit in/out total traffic on the `1981/tcp` to 64G and 1024G respectively.

### To limit traffic for filter
```Nginx
stream {
    geoip_country /usr/share/GeoIP/GeoIP.dat;

    server_traffic_status_zone;

    ...

    server {

        listen 1981;

        server_traffic_status_filter_by_set_key $geoip_country_code country::$server_addr;
        server_traffic_status_limit_traffic_by_set_key FG@country::$server_addr@US out:1024G;
        server_traffic_status_limit_traffic_by_set_key FG@country::$server_addr@CN out:2048G;

        ...

    }
}

```

* Limit total traffic of going into US and CN on the `example.org` to 1024G and 2048G respectively.

### To limit traffic for upstream
```Nginx
stream {

    server_traffic_status_zone;

    ...

    upstream backend {
        server 10.10.10.17:80;
        server 10.10.10.18:80;
    }

    server {

        listen 1981;

        server_traffic_status_limit_traffic_by_set_key UG@backend@10.10.10.17:80 in:512G;
        server_traffic_status_limit_traffic_by_set_key UG@backend@10.10.10.18:80 in:1024G;
        proxy_pass backend;

        ...

    }
}

```

* Limit total traffic of going into upstream backend on the `1981/tcp` to 512G and 1024G per each peer.

`Caveats:` Traffic is the cumulative transfer or counter, not a bandwidth.

## Use cases

It is able to calculate the user defined individual stats by using the directive `server_traffic_status_filter_by_set_key`.

### To calculate traffic for individual country using GeoIP
```Nginx
stream {
    geoip_country /usr/share/GeoIP/GeoIP.dat;

    server_traffic_status_zone;
    server_traffic_status_filter_by_set_key $geoip_country_code country::*;

    ...

    server {

        ...

        server_traffic_status_filter_by_set_key $geoip_country_code country::$server_addr:$server_port;

    }
}
```

* Calculate traffic for individual country of total server groups.
* Calculate traffic for individual country of each server groups.

Basically, country flags image is built-in in HTML.
The country flags image is enabled if the `country` string is included
in group name which is second argument of `server_traffic_status_filter_by_set_key` directive.


## Customizing
### To customize after the module installed
1. You need to change the `{{uri}}` string to your status uri in status.template.html as follows:
 ```
 shell> vi share/status.template.html
 ```
 ```
 var vtsStatusURI = "yourStatusUri/format/json", vtsUpdateInterval = 1000;
 ```

2. And then, customizing and copy status.template.html to server root directory as follows:
 ```
 shell> cp share/status.template.html /usr/share/nginx/html/status.html
 ```

4. Configure `nginx.conf`
 ```Nginx
    server {
        server_name example.org;
        root /usr/share/nginx/html;

        # Redirect requests for / to /status.html
        location = / {
            return 301 /status.html;
        }

        location = /status.html {}

        # Everything beginning /status (except for /status.html) is
        # processed by the status handler
        location /status {
            stream_server_traffic_status_display;
            stream_server_traffic_status_display_format json;
        }
    }

 ```

4. Access to your html.
 ```
 http://example.org/status.html
 ```

### To customize before the module installed
1. Modify `share/status.template.html` (Do not change `{{uri}}` string)

2. Recreate the `ngx_http_stream_server_traffic_status_module_html.h` as follows:
 ```
 shell> cd util
 shell> ./tplToDefine.sh ../share/status.template.html > ../src/ngx_http_stream_server_traffic_status_module_html.h
 ```

3. Add the module to the build configuration by adding
  ```
  --add-module=/path/to/nginx-module-sts
  --add-module=/path/to/nginx-module-stream-sts
  ```

4. Build the nginx binary.

5. Install the nginx binary.


## Directives

### stream_server_traffic_status

| -   | - |
| --- | --- |
| **Syntax**  | **stream_server_traffic_status** \<on\|off\> |
| **Default** | off |
| **Context** | http, server, location |

`Description:` Enables or disables the module working.
If you set `stream_server_traffic_status_zone` directive, is automatically enabled.

### stream_server_traffic_status_zone

| -   | - |
| --- | --- |
| **Syntax**  | **stream_server_traffic_status_zone** [shared:*name*] |
| **Default** | shared:stream_server_traffic_status |
| **Context** | http |

`Description:` Sets parameters for a shared memory zone specified by `server_traffic_status_zone`
directive in stream block.
`Caveats:` The `name` must be same as specified by `server_traffic_status_zone`.

### stream_server_traffic_status_display

| -   | - |
| --- | --- |
| **Syntax**  | **stream_server_traffic_status_display** |
| **Default** | - |
| **Context** | http, server, location |

`Description:` Enables or disables the module display handler.

### stream_server_traffic_status_display_format

| -   | - |
| --- | --- |
| **Syntax**  | **stream_server_traffic_status_display_format** \<json\|html\|jsonp\> |
| **Default** | json |
| **Context** | http, server, location |

`Description:` Sets the display handler's output format.
If you set `json`, will respond with a JSON document.
If you set `html`, will respond with the built-in live dashboard in HTML.
If you set `jsonp`, will respond with a JSONP callback function(default: *ngx_http_stream_server_traffic_status_jsonp_callback*).

### stream_server_traffic_status_display_jsonp

| -   | - |
| --- | --- |
| **Syntax**  | **stream_server_traffic_status_display_jsonp** *callback* |
| **Default** | ngx_http_stream_server_traffic_status_jsonp_callback |
| **Context** | http, server, location |

`Description:` Sets the callback name for the JSONP.

### stream_server_traffic_status_average_method

| -   | - |
| --- | --- |
| **Syntax**  | **stream_server_traffic_status_average_method** \<AMM\|WMA\> [*period*] |
| **Default** | AMM 60s |
| **Context** | http, server, location |

`Description:` Sets the method which is a formula that calculate the average of response processing times.
The *period* is an effective time of the values used for the average calculation.(Default: 60s)
If *period* set to 0, effective time is ignored.
In this case, the last average value is displayed even if there is no requests and after the elapse of time.
The corresponding values are `sessionMsec`, `uSessionMsec`, `uConnectMsec`, `uFirstByteMsec` in JSON.

* **AMM**
  * The AMM is the [arithmetic mean](https://en.wikipedia.org/wiki/Arithmetic_mean).
* **WMA**
  * THE WMA is the [weighted moving average](https://en.wikipedia.org/wiki/Moving_average#Weighted_moving_average).


### server_traffic_status

| -   | - |
| --- | --- |
| **Syntax**  | **server_traffic_status** \<on\|off\> |
| **Default** | off |
| **Context** | stream, server |

`Description:` Enables or disables the module working.
If you set `server_traffic_status_zone` directive, is automatically enabled.


### server_traffic_status_zone

| -   | - |
| --- | --- |
| **Syntax**  | **server_traffic_status_zone** [shared:*name:size*] |
| **Default** | shared:stream_server_traffic_status:1m |
| **Context** | stream |

`Description:` Sets parameters for a shared memory zone that will keep states for various keys.
The cache is shared between all worker processes.


### server_traffic_status_filter

| -   | - |
| --- | --- |
| **Syntax**  | **server_traffic_status_filter** \<on\|off\> |
| **Default** | on |
| **Context** | stream, server |

`Description:` Enables or disables the filter features.

### server_traffic_status_filter_by_set_key

| -   | - |
| --- | --- |
| **Syntax**  | **server_traffic_status_filter_by_set_key** *key* [*name*] |
| **Default** | - |
| **Context** | stream, server |

`Description:` Enables the keys by user defined variable.
The *key* is a key string to calculate traffic.
The *name* is a group string to calculate traffic.
The *key* and *name* can contain variables such as $host, $server_addr, $server_port.
The *name*'s group belongs to `streamFilterZones` if specified.
The *key*'s group belongs to `streamServerZones` if not specified second argument *name*.
The example with geoip module is as follows:

```Nginx
stream {

      ...

      server {
          listen 1981;
          server_traffic_status_filter_by_set_key $geoip_country_code country::$server_addr:$server_port;

          ...

      }
}
```

```Json
  ...
  "streamServerZones": {
  ...
  },
  "streamFilterZones": {
      "country::example.org": {
          "KR": {
              "port":...,
              "protocol":...,
              "connectCounter":...,
              "inBytes":...,
              "outBytes":...,
              "responses":{
                  "1xx":...,
                  "2xx":...,
                  "3xx":...,
                  "4xx":...,
                  "5xx":...,
              },
              "sessionMsec":...
              "sessionMsecs":{
                  "times":[...],
                  "msecs":[...]
              },
            },
          },
          "US": {
          ...
          },
          ...
      },
      ...
  },
  ...

```

### server_traffic_status_filter_check_duplicate

| -   | - |
| --- | --- |
| **Syntax**  | **server_traffic_status_filter_check_duplicate** \<on\|off\> |
| **Default** | on |
| **Context** | stream, server |

`Description:` Enables or disables the deduplication of server_traffic_status_filter_by_set_key.
It is processed only one of duplicate values(`key` + `name`) in each directives(stream, server) if this option is enabled.

### server_traffic_status_limit

| -   | - |
| --- | --- |
| **Syntax**  | **server_traffic_status_limit** \<on\|off\> |
| **Default** | on |
| **Context** | stream, server |

`Description:` Enables or disables the limit features.

### server_traffic_status_limit_traffic

| -   | - |
| --- | --- |
| **Syntax**  | **server_traffic_status_limit_traffic** *member*:*size* [*code*] |
| **Default** | - |
| **Context** | stream, server |

`Description:` Enables the traffic limit for specified *member*.
The *member* is a member string to limit traffic.
The *size* is a size(k/m/g) to limit traffic.
The *code* is a code to return in response to rejected requests.(Default: 503)

The available *`member`* strings are as follows:
* **connect**
  * The total number of client connects received from clients.
* **in**
  * The total number of bytes received from clients.
* **out**
  * The total number of bytes sent to clients.
* **1xx**
  * The number of responses with status codes 1xx.
* **2xx**
  * The number of responses with status codes 2xx.
* **3xx**
  * The number of responses with status codes 3xx.
* **4xx**
  * The number of responses with status codes 4xx.
* **5xx**
  * The number of responses with status codes 5xx.

### server_traffic_status_limit_traffic_by_set_key

| -   | - |
| --- | --- |
| **Syntax**  | **server_traffic_status_limit_traffic_by_set_key** *key* *member*:*size* [*code*] |
| **Default** | - |
| **Context** | stream, server |

`Description:` Enables the traffic limit for specified *key* and *member*.
The *key* is a key string to limit traffic.
The *member* is a member string to limit traffic.
The *size* is a size(k/m/g) to limit traffic.
The *code* is a code to return in response to rejected requests.(Default: 503)


The *`key`* syntax is as follows:
* *`group`*@[*`subgroup`*@]*`name`*

The available *`group`* strings are as follows:
* **NO**
  * The group of server.
* **UA**
  * The group of upstream alone.
* **UG**
  * The group of upstream group.(use *`subgroup`*)
* **FG**
  * The group of filter.(use *`subgroup`*)

The available *`member`* strings are as follows:
* **connect**
  * The total number of client requests received from clients.
* **in**
  * The total number of bytes received from clients.
* **out**
  * The total number of bytes sent to clients.
* **1xx**
  * The number of responses with status codes 1xx.
* **2xx**
  * The number of responses with status codes 2xx.
* **3xx**
  * The number of responses with status codes 3xx.
* **4xx**
  * The number of responses with status codes 4xx.
* **5xx**
  * The number of responses with status codes 5xx.

The *member* is the same as `server_traffic_status_limit_traffic` directive.

### server_traffic_status_limit_check_duplicate

| -   | - |
| --- | --- |
| **Syntax**  | **server_traffic_status_limit_check_duplicate** \<on\|off\> |
| **Default** | on |
| **Context** | stream, server |

`Description:` Enables or disables the deduplication of server_traffic_status_limit_by_set_key.
It is processed only one of duplicate values(`member` | `key` + `member`)
in each directives(stream, server) if this option is enabled.

### server_traffic_status_average_method

| -   | - |
| --- | --- |
| **Syntax**  | **server_traffic_status_average_method** \<AMM\|WMA\> [*period*] |
| **Default** | AMM 60s |
| **Context** | stream, server |

`Description:` Sets the method which is a formula that calculate the average of response processing times.
The *period* is an effective time of the values used for the average calculation.(Default: 60s)
If *period* set to 0, effective time is ignored.
In this case, the last average value is displayed even if there is no requests and after the elapse of time.
The corresponding value is only *$sts_session_time* variable.

* **AMM**
  * The AMM is the [arithmetic mean](https://en.wikipedia.org/wiki/Arithmetic_mean).
* **WMA**
  * THE WMA is the [weighted moving average](https://en.wikipedia.org/wiki/Moving_average#Weighted_moving_average).


`Caveats`: The *$sts_session_time* variable is the value calculated at the time of the last request.
It is not calculated when using variables.

### server_traffic_status_histogram_buckets

| -   | - |
| --- | --- |
| **Syntax**  | **server_traffic_status_histogram_buckets** *second* ... |
| **Default** | - |
| **Context** | stream |

`Description:` Sets the observe buckets to be used in the histograms.
By default, if you do not set this directive, it will not work.
The *second* can be expressed in decimal places with a minimum value of 0.001(1ms).
The maximum size of the buckets is 32. If this value is insufficient for you,
change the `NGX_STREAM_SERVER_TRAFFIC_STATUS_DEFAULT_BUCKET_LEN` in the
`nginx-mdule-stream-sts/src/ngx_stream_server_traffic_status_node.h`
and the `NGX_HTTP_STREAM_SERVER_TRAFFIC_STATUS_DEFAULT_BUCKET_LEN` in the
`nginx-module-sts/src/ngx_http_stream_server_traffic_status_node.h`.

For examples:
* **server_traffic_status_histogram_buckets** `0.005` `0.01` `0.05` `0.1` `0.5` `1` `5` `10`
  * The observe buckets are [5ms 10ms 50ms 1s 5s 10s].
* **server_traffic_status_histogram_buckets** `0.005` `0.01` `0.05` `0.1`
  * The observe buckets are [5ms 10ms 50ms 1s].

`Caveats:` By default, if you do not set this directive, the histogram statistics does not work.

## See Also
* [nginx-module-stream-sts](https://github.com/vozlt/nginx-module-stream-sts)
* [nginx-module-vts](https://github.com/vozlt/nginx-module-vts)

## TODO

## Donation
[![License](http://img.shields.io/badge/PAYPAL-DONATE-yellow.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=PWWSYKQ9VKH38&lc=KR&currency_code=USD&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted)

## Author
YoungJoo.Kim(김영주) [<vozltx@gmail.com>]
