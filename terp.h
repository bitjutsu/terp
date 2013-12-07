#ifndef __TERP_H__
#define __TERP_H__

#include "khash.h"

// setup hashmap
KHASH_MAP_INIT_STR(32, Element *)

typedef struct tagState {
  khash_t(32) *h;
} State;

#endif
