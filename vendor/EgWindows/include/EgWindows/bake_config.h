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

#ifndef EGWINDOWS_BAKE_CONFIG_H
#define EGWINDOWS_BAKE_CONFIG_H

/* Headers of public dependencies */
#include <egmath.h>

/* Convenience macro for exporting symbols */
#ifndef EgWindows_STATIC
#if defined(EgWindows_EXPORTS) && (defined(_MSC_VER) || defined(__MINGW32__))
  #define EGWINDOWS_API __declspec(dllexport)
#elif defined(EgWindows_EXPORTS)
  #define EGWINDOWS_API __attribute__((__visibility__("default")))
#elif defined(_MSC_VER)
  #define EGWINDOWS_API __declspec(dllimport)
#else
  #define EGWINDOWS_API
#endif
#else
  #define EGWINDOWS_API
#endif

#endif

