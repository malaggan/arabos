// -*- mode: c++; -*-
#pragma once

#include <types.h>
#include <mm.h> // for placement new. // TODO: move new operators to <new.h>
#include <lib.h> // for assert and printk

#include <iterator.hh>

extern "C"
char *dirname (char *path) {
    if(!path || !*path)
	return strdup(".");

    // remove trailing slashes (they are not part of the path) // WARN: modifies argument
    char *last = path + strnlen(path, 10000) - 1;
    while(last!=path && *last == '/') *last-- = 0;

    // find previous slash
    while(last!=path && *last != '/') last--;

    // either string is all made of slashes, or there is no slashes at all
    if(last == path) return strdup(".");

    // last points at the last slash
    return strndup(path, last-path); // TODO: test
}

extern "C"
char *basename (char* path) {
    if(!path || !*path)
	return strdup(".");

    // remove trailing slashes (they are not part of the path) // WARN: modifies argument
    char *last = path + strnlen(path, 10000) - 1;
    while(last!=path && *last == '/') *last-- = 0;

    // string is all made of slashes,
    if(last == path) return strdup("/");

    // find previous slash
    while(last!=path && *last != '/') last--;

    //  there is no slashes at all
    if(last == path) return strdup(path);

    // last points at the last slash
    return strdup(last + 1); // TODO: test
}

