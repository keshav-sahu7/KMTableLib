#ifndef KMTABLELIB_KMT_DEBUG_H
#define KMTABLELIB_KMT_DEBUG_H
#include <cstdio>

#define KM_DEBUG(text, ...) std::printf("line [%d] : " text "\n", __LINE__, __VA_ARGS__)

#endif // KMTABLELIB_KMT_DEBUG_H