#include "Renderer.hpp"
#include "OpenGL.hpp"

#include <SDL2/SDL.h>

#include <cmath> // floor

struct Attributes
{
	GLfloat corner[2];   // Location of corner relative to center, in world coords
	GLfloat position[2]; // Location of center in world coordinates
	GLfloat texcoord[2]; // Texture s,t of this corner
	GLfloat color[4];
};

struct RendererImpl
{
	Atlas& atlas;

	std::vector<Attributes> vertices;
	std::vector<GLuint> indices;

	ShaderProgram shader;
	Texture texture;

	VertexBuffer vboAttributes;
	VertexBuffer vboIndex;

	// Uniforms
	GLint uTexture;
	GLint uProjection;

	// Attributes per sprite
	GLint aCorner;
	GLint aPosition;
	GLint aTexcoord;
	GLint aColor;

	RendererImpl(Atlas& atlas);
};

Renderer::Renderer(Atlas& atlas)
	: self(new RendererImpl(atlas))
{
}

Renderer::~Renderer()
{
}

// Shader program for drawing sprites
namespace
{

constexpr GLchar vertexShader[] = R"(
attribute vec2 aCorner;
attribute vec2 aPosition;
attribute vec2 aTexcoord;
attribute vec4 aColor;
varying vec2 vTexcoord;
varying vec4 vColor;
uniform mat4 uProjection;

void main()
{
	vec2 worldCoords = aCorner + aPosition;
	gl_Position = uProjection * vec4(worldCoords, 0.0, 1.0);
	vTexcoord = aTexcoord;
	vColor = aColor;
}
)";

constexpr GLchar fragmentShader[] = R"(
uniform sampler2D uTexture;
varying vec2 vTexcoord;
varying vec4 vColor;

void main()
{
	gl_FragColor = vColor * texture2D(uTexture, vTexcoord);
}
)";

}

RendererImpl::RendererImpl(Atlas& atlas)
	: atlas(atlas)
	, shader(vertexShader, fragmentShader)
{
	uTexture    = glGetUniformLocation(shader.id, "uTexture");
	uProjection = glGetUniformLocation(shader.id, "uProjection");
	aTexcoord   = glGetAttribLocation(shader.id, "aTexcoord");
	aCorner     = glGetAttribLocation(shader.id, "aCorner");
	aPosition   = glGetAttribLocation(shader.id, "aPosition");
	aColor      = glGetAttribLocation(shader.id, "aColor");

	texture.copyFromSurface(atlas.getSurface());
}

void Renderer::clearSprites()
{
	self->vertices.clear();
	self->indices.clear();
}

void Renderer::setSprites(const std::vector<Sprite>& sprites)
{
	auto& vertices = self->vertices;
	auto& indices = self->indices;

	vertices.resize(sprites.size() * 4);
	for (std::size_t i = 0; i < sprites.size(); ++i)
	{
		const Sprite& S = sprites[i];
		const SpriteLocation& loc = self->atlas.getLocation(S.id);

		auto* vertex = &vertices[i * 4];

		vertex[0].corner[0] = loc.x0 * S.scaleX;
		vertex[0].corner[1] = loc.y0 * S.scaleY;
		vertex[0].texcoord[0] = loc.s0;
		vertex[0].texcoord[1] = loc.t0;

		vertex[1].corner[0] = loc.x1 * S.scaleX;
		vertex[1].corner[1] = loc.y0 * S.scaleY;
		vertex[1].texcoord[0] = loc.s1;
		vertex[1].texcoord[1] = loc.t0;

		vertex[2].corner[0] = loc.x0 * S.scaleX;
		vertex[2].corner[1] = loc.y1 * S.scaleY;
		vertex[2].texcoord[0] = loc.s0;
		vertex[2].texcoord[1] = loc.t1;

		vertex[3].corner[0] = loc.x1 * S.scaleX;
		vertex[3].corner[1] = loc.y1 * S.scaleY;
		vertex[3].texcoord[0] = loc.s1;
		vertex[3].texcoord[1] = loc.t1;

		for (int j = 0; j < 4; ++j)
		{
			vertex[j].position[0] = S.x - loc.x0 * S.scaleX;
			vertex[j].position[1] = S.y - loc.y0 * S.scaleY;
			vertex[j].color[0] = S.r;
			vertex[j].color[1] = S.g;
			vertex[j].color[2] = S.b;
			vertex[j].color[3] = S.a;
		}
	}

	constexpr GLuint cornerIndex[6] = { 0, 1, 2, 2, 1, 3 };

	std::size_t i = indices.size();
	indices.resize(sprites.size() * 6);
	for (; i < indices.size(); ++i)
	{
		const int j = i / 6;
		indices[i] = j * 4 + cornerIndex[i % 6];
	}
}

void Renderer::render(SDL_Window* window)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(self->shader.id);
	GL_Errors("glUseProgram");

	// TODO: Camera class
	constexpr float cameraX = 0.f;
	constexpr float cameraY = 0.f;
	constexpr float cameraScaleX = 1.f;
	constexpr float cameraScaleY = 1.f;

	int windowW, windowH;
	SDL_GetWindowSize(window, &windowW, &windowH);
	int drawableW, drawableH;
	SDL_GL_GetDrawableSize(window, &drawableW, &drawableH);

	glViewport(0, 0, drawableW, drawableH);
	const float L = std::floor(cameraX);
	const float R = std::floor(cameraX + windowW / cameraScaleX);
	const float T = std::floor(cameraY);
	const float B = std::floor(cameraY + windowH / cameraScaleY);
	const float orthoProjection[4][4] =
	{
		{ 2.0f/(R-L),   0.0f,         0.0f,   0.0f },
		{ 0.0f,         2.0f/(T-B),   0.0f,   0.0f },
		{ 0.0f,         0.0f,        -1.0f,   0.0f },
		{ (R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f },
	};
	glUniformMatrix4fv(self->uProjection, 1, GL_FALSE, &orthoProjection[0][0]);
	//

	// Textures have an id and also a register (0 in this
	// case). We have to bind register 0 to the texture id:
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, self->texture.id);
	// and then we have to tell the shader which register (0) to use:
	glUniform1i(self->uTexture, 0);
	// It might be ok to hard-code the register number inside the shader.

	// Tell the shader program where to find each of the input variables
	// ("attributes") in its vertex shader input.
	glBindBuffer(GL_ARRAY_BUFFER, self->vboAttributes.id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Attributes) * self->vertices.size(), self->vertices.data(), GL_STREAM_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->vboIndex.id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * self->indices.size(), self->indices.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(self->aCorner, 2, GL_FLOAT, GL_FALSE, sizeof(Attributes), reinterpret_cast<GLvoid*>(offsetof(Attributes, corner)));
	glVertexAttribPointer(self->aPosition, 2, GL_FLOAT, GL_FALSE, sizeof(Attributes), reinterpret_cast<GLvoid*>(offsetof(Attributes, position)));
	glVertexAttribPointer(self->aTexcoord, 2, GL_FLOAT, GL_FALSE, sizeof(Attributes), reinterpret_cast<GLvoid*>(offsetof(Attributes, texcoord)));
	glVertexAttribPointer(self->aColor, 4, GL_FLOAT, GL_FALSE, sizeof(Attributes), reinterpret_cast<GLvoid*>(offsetof(Attributes, color)));
	GL_Errors("glVertexAttribPointer");

	// Run the shader program. Enable the vertex attribs just while
	// running this program. Which ones are enabled is global state, and
	// we don't want to interfere with any other shader programs we want
	// to run elsewhere.
	glEnableVertexAttribArray(self->aCorner);
	glEnableVertexAttribArray(self->aPosition);
	glEnableVertexAttribArray(self->aTexcoord);
	glEnableVertexAttribArray(self->aColor);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->vboIndex.id);
	glDrawElements(GL_TRIANGLES, self->indices.size(), GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(self->aColor);
	glDisableVertexAttribArray(self->aTexcoord);
	glDisableVertexAttribArray(self->aPosition);
	glDisableVertexAttribArray(self->aCorner);
	GL_Errors("glDrawElements");

	glDisable(GL_BLEND);
}
