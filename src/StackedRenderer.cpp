#include "StackedRenderer.h"

#include <QDebug>

StackedRenderer::StackedRenderer(const float& depth) :
	Renderer(),
	_depth(depth),
	_modelViewProjection(),
	_shaderPrograms(),
	_textures(),
	_vbos(),
	_vaos(),
	_fbos()
{
}

void StackedRenderer::init()
{
	initializeOpenGLFunctions();
	
	createShaderPrograms();
	createTextures();
	createVBOs();
	createVAOs();
}

void StackedRenderer::destroy()
{
	for (auto texture : _textures)
	{
		texture->release();
		texture->destroy();
	}

	for (auto vbo : _vbos)
	{
		vbo->release();
		vbo->destroy();
	}

	for (auto vao : _vaos)
	{
		vao->release();
		vao->destroy();
	}
}

float StackedRenderer::depth() const
{
	return _depth;
}

void StackedRenderer::setDepth(const float& depth)
{
	if (depth == _depth)
		return;

	_depth = depth;
}

QMatrix4x4 StackedRenderer::modelViewProjection() const
{
	return _modelViewProjection;
}

void StackedRenderer::setModelViewProjection(const QMatrix4x4& modelViewProjection)
{
	_modelViewProjection = modelViewProjection;

	_modelViewProjection.translate(0.f, 0.f, static_cast<float>(_depth));
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

QSharedPointer<QOpenGLShaderProgram> StackedRenderer::shaderProgram(const QString& name)
{
	return _shaderPrograms[name];
}

QSharedPointer<const QOpenGLShaderProgram> StackedRenderer::shaderProgram(const QString& name) const
{
	return _shaderPrograms[name];
}

QSharedPointer<QOpenGLTexture> StackedRenderer::texture(const QString& name)
{
	return _textures[name];
}

QSharedPointer<const QOpenGLTexture> StackedRenderer::texture(const QString& name) const
{
	return _textures[name];
}

QSharedPointer<QOpenGLBuffer> StackedRenderer::vbo(const QString& name)
{
	return _vbos[name];
}

QSharedPointer<QOpenGLVertexArrayObject> StackedRenderer::vao(const QString& name)
{
	return _vaos[name];
}

QSharedPointer<const QOpenGLVertexArrayObject> StackedRenderer::vao(const QString& name) const
{
	return _vaos[name];
}

QSharedPointer<QOpenGLFramebufferObject> StackedRenderer::fbo(const QString& name)
{
	return _fbos[name];
}

QSharedPointer<const QOpenGLFramebufferObject> StackedRenderer::fbo(const QString& name) const
{
	return _fbos[name];
}