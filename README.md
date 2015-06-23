lua-http
=========

HTTP protocol parser for lua.

---

## Dependencies

- http: https://github.com/mah0x211/http
- luarocks-fetch-gitrec: https://github.com/siffiejoe/luarocks-fetch-gitrec



## Create new http parser object

### parser, err = http.new( [maxheader:number] )

**Parameters**

- `maxheader:number`: maximum number of headers. `default 20`


**Returns**

1. `parser:userdata`: http parser object on success. or, a nil on failure.
2. `err:string`: error message.


## Parse HTTP Request

- rc = parser:parseRequest( req:table, hdr:table, str:string )
- rc = parser:parseRequestPtr( req:table, hdr:table, str:pointer, len:number )

parse HTTP request.


**Common Parameters**

- `req:table`: container table for request-line items.
- `hdr:table`: container table for headers.


**parseRequest Parameters**

- `str:string`: HTTP request string.


**parseRequestPtr Parameters**

- `str:pointer`: pointer of HTTP request string.
- `len:number`: bytes length.


**Returns**

1. `rc:number`: return code.


**Field names of request-line table**

- `method:number`: method number.
- `uri:string`: uri.
- `version:number`: version.


## Parse HTTP Response

- rc = parser:parseResponse( res:table, hdr:table, str:string )
- rc = parser:parseResponsePtr( res:table, hdr:table, str:pointer, len:number )

parse HTTP response.

**Common Parameters**

- `res:table`: container table for status-line items.
- `hdr:table`: container table for headers.

**parseResponse Parameters**

- `str:string`: HTTP response string.


**parseResponsePtr Parameters**

- `str:pointer`: pointer of HTTP request string.
- `len:number`: bytes length.


**Returns**

1. `rc:number`: return code.


**Field names of response-line table**

- `method:number`: version.
- `status:number`: status code (`1XX`, `2XX`, `3XX`, `4XX`, `5XX`).
- `reason:string`: reason-phrase string.


## Constants

**HTTP Method**

- `http.MGET`: GET
- `http.MHEAD`: HEAD
- `http.MPOST`: POST
- `http.MPUT`: PUT
- `http.MDELETE`: DELETE
- `http.MOPTIONS`: OPTIONS
- `http.MTRACE`: TRACE
- `http.MCONNECT`: CONNECT


**HTTP Version**

- `http.V09`: HTTP/0.9
- `http.V10`: HTTP/1.0
- `http.V11`: HTTP/1.1


**Return Code**

- `0`: success
- `-1`: probably, data structure is corrupted
- `-2`: need more bytes
- `-3`: method not implemented
- `-4`: invalid uri string
- `-5`: uri-length too large
- `-6`: version not support
- `-7`: invalid line format
- `-8`: invalid header format
- `-9`: too many headers
- `-10`: header-length too large
- `-11`: invalid status code  
    this code is returned only from the `parseResponse` and `parseResponsePtr`


## Usage

```lua
local reqstr = 
    "GET /wp-content/uploa<ds/2010/03/hello-kitty-darth-vader-pink.jpg HTTP/1.1\r\n" ..
    "Host: www.kittyhell.com\r\n" ..
    "User-Agent: Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10.6; ja-JP-mac; rv:1.9.2.3) Gecko/20100401 Firefox/3.6.3 Pathtraq/0.9\r\n" ..
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n" ..
    "Accept-Language: ja,en-us;q=0.7,en;q=0.3\r\n" ..
    "Accept-Encoding: gzip,deflate\r\n" ..
    "Accept-Charset: Shift_JIS,utf-8;q=0.7,*;q=0.7\r\n" ..
    "Keep-Alive: 115\r\n" ..
    "Connection: keep-alive\r\n" ..
    "Cookie: wp_ozh_wsa_visits=2; wp_ozh_wsa_visit_lasttime=xxxxxxxxxx; " ..
    "__utma=xxxxxxxxx.xxxxxxxxxx.xxxxxxxxxx.xxxxxxxxxx.xxxxxxxxxx.x; " ..
    "__utmz=xxxxxxxxx.xxxxxxxxxx.x.x.utmccn=(referral)|utmcsr=reader.livedoor.com|utmcct=/reader/|utmcmd=referral\r\n" ..
    "\r\n";
local parser = require('http').new();
local req = {
	header = {}	
};
local rc = parse:parseRequest( req, req.header, reqstr );
```
