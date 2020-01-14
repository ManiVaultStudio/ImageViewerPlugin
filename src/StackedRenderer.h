#pragma once

#include "Common.h"

#include "renderers/Renderer.h"

#include <QMatrix4x4>
#include <QMap>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFramebufferObject>

class StackedRenderer : public hdps::Renderer
{
public:
	StackedRenderer(const std::uint32_t& zIndex);
	
public:
	void init() override;
	void destroy() override;

	void setModelViewProjection(const QMatrix4x4& modelViewProjection);

protected:
	virtual bool isInitialized() const = 0;
	virtual void createShaderPrograms();
	virtual void createTextures();
	virtual void createVBOs();
	virtual void createVAOs();
	virtual void createFBOs();

	std::shared_ptr<QOpenGLShaderProgram>& shaderProgram(const QString& name);
	std::shared_ptr<QOpenGLTexture>& texture(const QString& name);
	std::shared_ptr<const QOpenGLTexture> texture(const QString& name) const;
	std::shared_ptr<QOpenGLBuffer>& vbo(const QString& name);
	std::shared_ptr<QOpenGLVertexArrayObject>& vao(const QString& name);
	std::shared_ptr<QOpenGLFramebufferObject>& fbo(const QString& name);

protected:
	std::uint32_t												_zIndex;
	QMatrix4x4													_modelViewProjection;
	QMap<QString, std::shared_ptr<QOpenGLShaderProgram>>		_shaderPrograms;
	QMap<QString, std::shared_ptr<QOpenGLTexture>>				_textures;
	QMap<QString, std::shared_ptr<QOpenGLBuffer>>				_vbos;
	QMap<QString, std::shared_ptr<QOpenGLVertexArrayObject>>	_vaos;
	QMap<QString, std::shared_ptr<QOpenGLFramebufferObject>>	_fbos;
};