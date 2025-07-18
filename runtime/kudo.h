#pragma once

// Kudo Runtime Utils

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef int32_t kudo_int;
typedef size_t  kudo_usize;
typedef bool    kudo_bool;
typedef struct {
    char* data;
    kudo_usize len;
} kudo_str;
