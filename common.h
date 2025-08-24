#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#define UNREACHABLE() __builtin_unreachable()
#define UNLIKELY(condition) (__builtin_expect((condition), 0))
#define UNUSED __attribute__((unused))

#endif
