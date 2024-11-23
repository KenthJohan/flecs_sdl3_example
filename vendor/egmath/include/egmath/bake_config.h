/*
                                   )
                                  (.)
                                  .|.
                                  | |
                              _.--| |--._
                           .-';  ;`-'& ; `&.
                          \   &  ;    &   &_/
                           |"""---...---"""|
                           \ | | | | | | | /
                            `---.|.|.|.---'

 * This file is generated by bake.lang.c for your convenience. Headers of
 * dependencies will automatically show up in this file. Include bake_config.h
 * in your main project file. Do not edit! */

#ifndef EGMATH_BAKE_CONFIG_H
#define EGMATH_BAKE_CONFIG_H

/* Headers of public dependencies */
/* No dependencies */

/* Convenience macro for exporting symbols */
#ifndef egmath_STATIC
#if defined(egmath_EXPORTS) && (defined(_MSC_VER) || defined(__MINGW32__))
  #define EGMATH_API __declspec(dllexport)
#elif defined(egmath_EXPORTS)
  #define EGMATH_API __attribute__((__visibility__("default")))
#elif defined(_MSC_VER)
  #define EGMATH_API __declspec(dllimport)
#else
  #define EGMATH_API
#endif
#else
  #define EGMATH_API
#endif

#endif

