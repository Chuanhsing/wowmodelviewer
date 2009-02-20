#ifndef OPENGLHEADERS_H
#define OPENGLHEADERS_H

#include ".\glew\include\GL\glew.h"

#ifdef _WIN32
	#include ".\glew\include\GL\wglew.h"
#else
	#include <GL/glxew.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

// opengl
#ifdef __WXMAC__ //Mac
#	ifdef __DARWIN__
#		include <OpenGL/gl.h>
#		include <OpenGL/glaux.h>
#		include <OpenGL/glu.h>
#	else
#		include <gl.h>
#		include <glu.h>
#		include <glaux.h>
#	endif
#else
#	include <GL/gl.h>
#	include <GL/glu.h>
// #	include <GL/glut.h>
#	ifdef _WIN32
#		include <GL/glaux.h>
//#		include "wglext.h"		//WGL extensions
//#		include "glext.h"		//GL extensions
#	else
#		include <stdarg.h>
#		include <GL/glext.h>
#		include <GL/glx.h>
#	endif
#endif


#define GL_BUFFER_OFFSET(i) ((char *)(0) + (i))

#endif

