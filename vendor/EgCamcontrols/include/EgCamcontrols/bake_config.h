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

#ifndef EGCAMCONTROLS_BAKE_CONFIG_H
#define EGCAMCONTROLS_BAKE_CONFIG_H

/* Headers of public dependencies */
#include <egmath.h>

/* Convenience macro for exporting symbols */
#ifndef EgCamcontrols_STATIC
#if defined(EgCamcontrols_EXPORTS) && (defined(_MSC_VER) || defined(__MINGW32__))
  #define EGCAMCONTROLS_API __declspec(dllexport)
#elif defined(EgCamcontrols_EXPORTS)
  #define EGCAMCONTROLS_API __attribute__((__visibility__("default")))
#elif defined(_MSC_VER)
  #define EGCAMCONTROLS_API __declspec(dllimport)
#else
  #define EGCAMCONTROLS_API
#endif
#else
  #define EGCAMCONTROLS_API
#endif

#endif

