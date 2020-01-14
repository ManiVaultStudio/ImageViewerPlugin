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
	
	createShaderPrograms();
	createTextures();
	createVBOs();
	createVAOs();
	createFBOs();
}

void StackedRenderer::destroy()
{

}

void StackedRenderer::setModelViewProjection(const QMatrix4x4& modelViewProjection)
{
	_modelViewProjection = modelViewProjection;

	_modelViewProjection.translate(0.f, 0.f, static_cast<float>(_zIndex));
}

void StackedRenderer::createShaderPrograms()
{
}

void StackedRenderer::createTextures()
{
}

void StackedRenderer::createVBOs()
{

}

void StackedRenderer::createVAOs()
{

}

void StackedRenderer::createFBOs()
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

std::shared_ptr<const QOpenGLTexture> StackedRenderer::texture(const QString& name) const
{
	return _textures[name];
}

std::shared_ptr<QOpenGLBuffer>& StackedRenderer::vbo(const QString& name)
{
	return _vbos[name];
}

std::shared_ptr<QOpenGLVertexArrayObject>& StackedRenderer::vao(const QString& name)
{
	return _vaos[name];
}

std::shared_ptr<QOpenGLFramebufferObject>& StackedRenderer::fbo(const QString& name)
{
	return _fbos[name];
}