#include "OpenGL.hpp"

#include <SDL2/SDL.h>

#include <iostream>
#include <vector>

void GL_Errors(std::string_view message)
{
#ifndef __EMSCRIPTEN__
	while (true)
	{
		const GLenum err = glGetError();
		if (err == GL_NO_ERROR)
			break;

		std::cerr << message << ": " << err << '\n';
	}
#endif
}

ShaderProgram::ShaderProgram(std::string vertexShader, std::string fragmentShader)
{
	id = glCreateProgram();
	if (id == 0)
		GL_Errors("glCreateProgram");

#ifdef GL_ES_VERSION_2_0
	// WebGL requires precision specifiers but OpenGL 2.1 disallows
	// them, so I define the shader without it and then add it here.
	fragmentShader.insert(0, "precision mediump float;\n");
#endif

	attachShader(GL_VERTEX_SHADER, vertexShader.c_str());
	attachShader(GL_FRAGMENT_SHADER, fragmentShader.c_str());
	glLinkProgram(id);

	GLint linkStatus;
	glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);
	if (!linkStatus)
	{
		GLint logLength;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<GLchar> log(logLength);
		glGetProgramInfoLog(id, logLength, nullptr, log.data());
		for (const auto& ch : log)
			std::cerr << ch;
		std::cerr << '\n';
		GL_Errors("glLinkProgram");
	}
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(id);
}

void ShaderProgram::attachShader(GLenum type, const GLchar* source)
{
	GLuint shaderId = glCreateShader(type);
	if (shaderId == 0)
		GL_Errors("glCreateShader");

	glShaderSource(shaderId, 1, &source, nullptr);
	glCompileShader(shaderId);

	GLint compileStatus;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
	if (!compileStatus)
	{
		GLint logLength;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<GLchar> log(logLength);
		glGetShaderInfoLog(shaderId, logLength, nullptr, log.data());
		for (const auto& ch : log)
			std::cerr << ch;
		std::cerr << '\n';
		GL_Errors("glCompileShader");
	}

	glAttachShader(id, shaderId);
	glDeleteShader(shaderId);
	GL_Errors("glAttachShader");
}

Texture::Texture(SDL_Surface* surface)
{
	glGenTextures(1, &id);

	if (surface)
		copyFromSurface(surface);
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

void Texture::copyFromPixels(int width, int height, GLenum format, void* pixels)
{
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// NOTE: these parameters will allow non-power-of-two texture sizes
	// in WebGL, which we want to use for underlay and overlay images.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
	GL_Errors("glTexImage2D");
}

void Texture::copyFromSurface(SDL_Surface* surface)
{
	copyFromPixels(surface->w, surface->h,
				   surface->format->BytesPerPixel == 1 ? GL_ALPHA :
				   surface->format->BytesPerPixel == 3 ? GL_RGB :
				   GL_RGBA, // TODO: check for other formats
				   surface->pixels);
}

VertexBuffer::VertexBuffer()
{
	glGenBuffers(1, &id);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &id);
}
