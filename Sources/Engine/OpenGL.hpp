// Credit: https://github.com/redblobgames/helloworld-sdl2-opengl-emscripten

#pragma once

#ifdef __EMSCRIPTEN__
#include <SDL2/SDL_opengles2.h>
#else
#include <GL/glew.h>
#endif

#include <string>
#include <string_view>

struct SDL_Surface;

struct NonCopyable
{
	NonCopyable() = default;
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
};

// Check for any OpenGL errors and print them
void GL_Errors(std::string_view message);

struct ShaderProgram : NonCopyable
{
	GLuint id;

	ShaderProgram(std::string vertexShader, std::string fragmentShader);
	~ShaderProgram();

private:
	void attachShader(GLenum type, const GLchar* source);
};

struct Texture : NonCopyable
{
	GLuint id;

	Texture(SDL_Surface* surface = nullptr);
	~Texture();

	void copyFromPixels(int width, int height, GLenum format, void* pixels);
	void copyFromSurface(SDL_Surface* surface);
};

struct VertexBuffer : NonCopyable
{
	GLuint id;

	VertexBuffer();
	~VertexBuffer();
};
