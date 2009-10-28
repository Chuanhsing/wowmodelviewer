#ifndef RENDERTEXTURE_H
#define RENDERTEXTURE_H

// Video & graphics stuff
#include ".\glew\include\GL\glew.h"
#ifdef _WIN32
	#include ".\glew\include\GL\wglew.h"
#else
	#include <GL/glxew.h>
#endif
#include "OpenGLHeaders.h"
#include "video.h"

// WX
#include "wx/wxprec.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

class RenderTexture {
protected:
	HPBUFFERARB m_hPBuffer;
    HDC         m_hDC;
    HGLRC       m_hRC;

	HWND		canvas_hWnd;
	HDC         canvas_hDC;
    HGLRC       canvas_hRC;

	GLuint		m_frameBuffer;
	GLuint		m_depthRenderBuffer;

	GLuint		m_texID;
	GLenum		m_texFormat;

	bool		m_FBO;

public:

	int         nWidth;
    int         nHeight;

	RenderTexture() { 
		m_hPBuffer = NULL;
		m_hDC = NULL;
		m_hRC = NULL;
		m_texID = 0;

		nWidth = 0;
		nHeight = 0;

		m_FBO = false;
	}

	~RenderTexture(){
		if (m_hRC != NULL)
			Shutdown();
	}

	void Init(HWND hWnd, int width, int height, bool fboMode);
	void Shutdown();

	void BeginRender();
	void EndRender();

	void BindTexture();
	void ReleaseTexture();
	GLenum GetTextureFormat() { return m_texFormat; };

	void SwapBuffer() { ::SwapBuffers(m_hDC); };

	void InitGL();

};

#endif //RENDERTEXTURE_H

