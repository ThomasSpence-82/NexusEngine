#pragma once

#ifndef GLAD_H_
#define GLAD_H_

// Prevent Windows from including its own OpenGL headers
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif

// Include Windows headers first
#include <windows.h>

// Define our own OpenGL types and constants
typedef unsigned int GLenum;
typedef float GLfloat;
typedef int GLsizei;
typedef unsigned int GLbitfield;
typedef unsigned char GLubyte;
typedef char GLchar;
typedef int GLint;
typedef unsigned int GLuint;
typedef unsigned char GLboolean;

// OpenGL constants
#define GL_COLOR_BUFFER_BIT               0x00004000
#define GL_VENDOR                         0x1F00
#define GL_RENDERER                       0x1F01
#define GL_VERSION                        0x1F02

#ifdef __cplusplus
extern "C" {
	#endif

	// OpenGL 1.1 functions (always available)
	void APIENTRY nexus_glClear(GLbitfield mask);
	void APIENTRY nexus_glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	const GLubyte* APIENTRY nexus_glGetString(GLenum name);

	// Map our functions to standard names
	#define glClear nexus_glClear
	#define glClearColor nexus_glClearColor
	#define glGetString nexus_glGetString

	// Function to initialize OpenGL
	int gladLoadGL(void);

	#ifdef __cplusplus
}
#endif

#endif /* GLAD_H_ */