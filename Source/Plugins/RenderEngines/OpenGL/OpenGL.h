#ifndef WMV_PLUGIN_RENDERER_OPENGL_H
#define WMV_PLUGIN_RENDERER_OPENGL_H

#include "../../../Engine/Kernel.h"

#ifdef OPENGL_PLUGIN_EXPORTS
  #define OPENGL_PLUGIN_API Q_DECL_EXPORT
#else
  #define OPENGL_PLUGIN_API Q_DECL_IMPORT
#endif

#endif