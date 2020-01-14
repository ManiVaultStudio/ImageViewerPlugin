#include "StackedRenderer.h"

#include <QDebug>

StackedRenderer::StackedRenderer(const std::uint32_t& zIndex) :
	Renderer(),
	_zIndex(zIndex),
	_modelViewProjection(),
	_shaderPrograms(),
	_textures()
{
}

void StackedRenderer::init()
{
	initializeOpenGLFunctions();
	
	initializeShaderPrograms();
	initializeTextures();
}

void StackedRenderer::setModelViewProjection(const QMatrix4x4& modelViewProjection)
{
	_modelViewProjection = modelViewProjection;

	_modelViewProjection.translate(0.f, 0.f, static_cast<float>(_zIndex));
}

void StackedRenderer::initializeShaderPrograms()
{
}

void StackedRenderer::initializeTextures()
{
}

void StackedRenderer::initializeFBOs()
{
}

std::shared_ptr<QOpenGLShaderProgram>& StackedRenderer::shaderProgram(const QString& name)
{
	return _shaderPrograms[name];
}

std::shared_ptr<QOpenGLTexture>& StackedRenderer::texture(const QString& name)
{
	return _textures[name];
}