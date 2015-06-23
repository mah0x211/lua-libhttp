/*
 *  Copyright 2015 Masatoshi Teruya. All rights reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a 
 *  copy of this software and associated documentation files (the "Software"), 
 *  to deal in the Software without restriction, including without limitation 
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 *  and/or sell copies of the Software, and to permit persons to whom the 
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 *  DEALINGS IN THE SOFTWARE.
 *
 *  lhttp.c
 *  lua-http
 *
 *  Created by Masatoshi Teruya on 15/06/08.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <lua.h>
#include <lauxlib.h>
#include "http.h"

#define MODULE_MT   "http"

#define DEFAULT_MAX_URILEN  3072
#define DEFAULT_MAX_HDRLEN  4352
#define DEFAULT_MAX_HDR     20


// helper macros
#define lstate_setmetatable(L,tname) do{ \
    luaL_getmetatable( L, tname ); \
    lua_setmetatable( L, -2 ); \
}while(0)


#define lstate_fn2tbl(L,k,v) do{ \
    lua_pushstring(L,k); \
    lua_pushcfunction(L,v); \
    lua_rawset(L,-3); \
}while(0)


#define lstate_num2tbl(L,k,v) do{ \
    lua_pushstring(L,k); \
    lua_pushnumber(L,v); \
    lua_rawset(L,-3); \
}while(0)


#define lstate_str2tbl(L,k,v,vl) do{ \
    lua_pushstring(L,k); \
    lua_pushstring(L,v,vl); \
    lua_rawset(L,-3); \
}while(0)


#define lstate_strl2tbl(L,k,v,vl) do{ \
    lua_pushstring(L,k); \
    lua_pushlstring(L,v,vl); \
    lua_rawset(L,-3); \
}while(0)

#define lstate_strll2tbl(L,k,kl,v,vl) do{ \
    lua_pushlstring(L,k,kl); \
    lua_pushlstring(L,v,vl); \
    lua_rawset(L,-3); \
}while(0)


typedef struct {
    http_t *r;
    uint16_t maxurilen;
    uint16_t maxhdrlen;
} lhttp_t;


static inline int parse_request( lua_State *L, lhttp_t *h, char *buf, size_t len )
{
    http_t *r = h->r;
    int rc = 0;
    
    luaL_checktype( L, 2, LUA_TTABLE );
    luaL_checktype( L, 3, LUA_TTABLE );
    rc = http_req_parse( r, buf, len, h->maxurilen, h->maxhdrlen );
    if( rc == HTTP_SUCCESS )
    {
        uint8_t i = 0;
        uint8_t nheader = r->nheader;
        uintptr_t key = 0;
        uint16_t klen = 0;
        uintptr_t val = 0;
        uint16_t vlen = 0;
        
        // export to table
        lua_settop( L, 3 );
        // add to header table
        for(; i < nheader; i++ ){
            http_getheader_at( r, &key, &klen, &val, &vlen, i );
            lstate_strll2tbl( L, buf + key, klen, buf + val, vlen );
        }
        lua_pop( L, 1 );
        lstate_num2tbl( L, "version", http_version( r ) );
        lstate_num2tbl( L, "method", http_method( r ) );
        lstate_strl2tbl( L, "uri", buf + r->msg, r->msglen );
        // initialize
        http_init( r );
    }
    
    // add status
    lua_pushinteger( L, rc );
    
    return 1;
}


static int parse_request_lua( lua_State *L )
{
    lhttp_t *h = luaL_checkudata( L, 1, MODULE_MT );
    size_t len = 0;
    char *buf = (char*)luaL_checklstring( L, 4, &len );
    
    return parse_request( L, h, buf, len );
}


static int parse_request_ptr_lua( lua_State *L )
{
    lhttp_t *h = luaL_checkudata( L, 1, MODULE_MT );
    char *buf = (char*)lua_topointer( L, 4 );
    size_t len = luaL_checkinteger( L, 5 );
    
    
    return parse_request( L, h, buf, len );
}


static inline int parse_response( lua_State *L, lhttp_t *h, char *buf, 
                                  size_t len )
{
    http_t *r = h->r;
    int rc = 0;
    
    luaL_checktype( L, 2, LUA_TTABLE );
    luaL_checktype( L, 3, LUA_TTABLE );
    rc = http_res_parse( r, buf, len, h->maxhdrlen );
    if( rc == HTTP_SUCCESS )
    {
        uint8_t i = 0;
        uint8_t nheader = r->nheader;
        uintptr_t key = 0;
        uint16_t klen = 0;
        uintptr_t val = 0;
        uint16_t vlen = 0;
        
        // export to table
        lua_settop( L, 3 );
        // add to header table
        for(; i < nheader; i++ ){
            http_getheader_at( r, &key, &klen, &val, &vlen, i );
            lstate_strll2tbl( L, buf + key, klen, buf + val, vlen );
        }
        lua_pop( L, 1 );
        lstate_num2tbl( L, "version", http_version( r ) );
        lstate_num2tbl( L, "status", http_status( r ) );
        lstate_strl2tbl( L, "reason", buf + r->msg, r->msglen );
        // initialize
        http_init( r );
    }
    
    // add status
    lua_pushinteger( L, rc );
    
    return 1;
}


static int parse_response_lua( lua_State *L )
{
    lhttp_t *h = luaL_checkudata( L, 1, MODULE_MT );
    size_t len = 0;
    char *buf = (char*)luaL_checklstring( L, 4, &len );
    
    return parse_response( L, h, buf, len );
}


static int parse_response_ptr_lua( lua_State *L )
{
    lhttp_t *h = luaL_checkudata( L, 1, MODULE_MT );
    char *buf = (char*)lua_topointer( L, 4 );
    size_t len = luaL_checkinteger( L, 5 );
    
    return parse_response( L, h, buf, len );
}


static int init_lua( lua_State *L )
{
    lhttp_t *h = luaL_checkudata( L, 1, MODULE_MT );
    
    http_init( h->r );
    
    return 0;
}


static int tostring_lua( lua_State *L )
{
    lua_pushfstring( L, MODULE_MT ": %p", lua_touserdata( L, 1 ) );
    return 1;
}


static int gc_lua( lua_State *L )
{
    http_free( ((lhttp_t*)lua_touserdata( L, 1 ))->r );
    return 0;
}


static int new_lua( lua_State *L )
{
    lhttp_t *h = NULL;
    int maxurilen = luaL_optint( L, 1, DEFAULT_MAX_URILEN );
    int maxhdrlen = luaL_optint( L, 2, DEFAULT_MAX_HDRLEN );
    int maxhdr = luaL_optint( L, 3, DEFAULT_MAX_HDR );
    
    if( maxurilen > UINT16_MAX ){
        return luaL_argerror( L, 1, "maxurilen must be less than UINT16_MAX" );
    }
    else if( maxhdrlen > UINT16_MAX ){
        return luaL_argerror( L, 2, "maxhdrlen must be less than UINT16_MAX" );
    }
    else if( maxhdr > UINT8_MAX ){
        return luaL_argerror( L, 3, "maxhdr must be less than UINT8_MAX" );
    }
    else if( ( h = lua_newuserdata( L, sizeof( lhttp_t ) ) ) &&
             ( h->r = http_alloc( maxhdr ) ) ){
        h->maxurilen = maxurilen;
        h->maxhdrlen = maxhdrlen;
        luaL_getmetatable( L, MODULE_MT );
        lua_setmetatable( L, -2 );
        return 1;
    }
    
    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}


LUALIB_API int luaopen_http( lua_State *L )
{
    struct luaL_Reg mmethods[] = {
        { "__gc", gc_lua },
        { "__tostring", tostring_lua },
        { NULL, NULL }
    };
    struct luaL_Reg methods[] = {
        // method
        { "init", init_lua },
        { "parseRequest", parse_request_lua },
        { "parseRequestPtr", parse_request_ptr_lua },
        { "parseResponse", parse_response_lua },
        { "parseResponsePtr", parse_response_ptr_lua },
        { NULL, NULL }
    };
    struct luaL_Reg *ptr = mmethods;
    
    // create metatable
    luaL_newmetatable( L, MODULE_MT );
    // metamethods
    do {
        lstate_fn2tbl( L, ptr->name, ptr->func );
        ptr++;
    } while( ptr->name );
    // methods
    ptr = methods;
    lua_pushstring( L, "__index" );
    lua_newtable( L );
    do {
        lstate_fn2tbl( L, ptr->name, ptr->func );
        ptr++;
    } while( ptr->name );
    lua_rawset( L, -3 );
    // remove metatable from stack
    lua_pop( L, 1 );
    
    
    // register allocator
    lua_createtable( L, 0, 1 );
    lstate_fn2tbl( L, "new", new_lua );
    // constants
    // methods
    lstate_num2tbl( L, "MGET", HTTP_MGET );
    lstate_num2tbl( L, "MHEAD", HTTP_MHEAD );
    lstate_num2tbl( L, "MPOST", HTTP_MPOST );
    lstate_num2tbl( L, "MPUT", HTTP_MPUT );
    lstate_num2tbl( L, "MDELETE", HTTP_MDELETE );
    lstate_num2tbl( L, "MOPTIONS", HTTP_MOPTIONS );
    lstate_num2tbl( L, "MTRACE", HTTP_MTRACE );
    lstate_num2tbl( L, "MCONNECT", HTTP_MCONNECT );
    // versions
    lstate_num2tbl( L, "V09", HTTP_V09 );
    lstate_num2tbl( L, "V10", HTTP_V10 );
    lstate_num2tbl( L, "V11", HTTP_V11 );

    return 1;
}


