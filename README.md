lua-libhttp
=========

HTTP protocol parser for lua.

---

## Dependencies

- libhttp: https://github.com/mah0x211/libhttp
- luarocks-fetch-gitrec: https://github.com/siffiejoe/luarocks-fetch-gitrec


## Installation

```sh
luarocks install libhttp --from=http://mah0x211.github.io/rocks/
```


## Create new libhttp parser object

### parser, err = libhttp.new( [maxheader:number] )

**Parameters**

- `maxheader:number`: maximum number of headers. `default 20`


**Returns**

1. `parser:userdata`: libhttp parser object on success. or, a nil on failure.
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

- `version:number`: version.
- `status:number`: status code (`1XX`, `2XX`, `3XX`, `4XX`, `5XX`).
- `reason:string`: reason-phrase string.


## Constants

**HTTP Method**

- `libhttp.MGET`: GET
- `libhttp.MHEAD`: HEAD
- `libhttp.MPOST`: POST
- `libhttp.MPUT`: PUT
- `libhttp.MDELETE`: DELETE
- `libhttp.MOPTIONS`: OPTIONS
- `libhttp.MTRACE`: TRACE
- `libhttp.MCONNECT`: CONNECT


**HTTP Version**

- `libhttp.V09`: HTTP/0.9
- `libhttp.V10`: HTTP/1.0
- `libhttp.V11`: HTTP/1.1


**Return Code**

- `libhttp.SUCCESS`: success
- `libhttp.ERROR`: probably, data structure is corrupted
- `libhttp.EAGAIN`: need more bytes
- `libhttp.EMETHOD`: method not implemented
- `libhttp.EBADURI`: invalid uri string
- `libhttp.EURILEN`: uri-length too large
- `libhttp.EVERSION`: version not support
- `libhttp.ELINEFMT`: invalid line format
- `libhttp.EHDRFMT`: invalid header format
- `libhttp.ENHDR`: too many headers
- `libhttp.EHDRLEN`: header-length too large


following code is returned only from the `parseResponse` and `parseResponsePtr`

- `libhttp.ESTATUS`: invalid status code
- `libhttp.EREASON`: invalid reason-phrase


## Usage

```lua
local reqstr =
    "GET /wp-content/uploads/2010/03/hello-kitty-darth-vader-pink.jpg HTTP/1.1\r\n" ..
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
local parser = require('libhttp').new();
local req = {
	header = {}
};
local rc = parse:parseRequest( req, req.header, reqstr );
```
