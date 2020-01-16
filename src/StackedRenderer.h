#pragma once

#include <memory>

#include "Common.h"

#include "renderers/Renderer.h"

#include <QMatrix4x4>
#include <QMap>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFramebufferObject>

class StackedRenderer : public QObject, public hdps::Renderer
{
	Q_OBJECT

public:
	StackedRenderer(const float& depth);
	
public:
	void init() override;
	void destroy() override;

public:
	float depth() const;
	void setDepth(const float& depth);
	QMatrix4x4 modelViewProjection() const;
	void setModelViewProjection(const QMatrix4x4& modelViewProjection);

protected:
	virtual bool isInitialized() const = 0;
	virtual void createShaderPrograms();
	virtual void createTextures();
	virtual void createVBOs();
	virtual void createVAOs(); 

	std::shared_ptr<QOpenGLShaderProgram> shaderProgram(const QString& name);
	std::shared_ptr<const QOpenGLShaderProgram> shaderProgram(const QString& name) const;
	std::shared_ptr<QOpenGLTexture> texture(const QString& name);
	std::shared_ptr<const QOpenGLTexture> texture(const QString& name) const;
	std::shared_ptr<QOpenGLBuffer> vbo(const QString& name);
	std::shared_ptr<QOpenGLVertexArrayObject> vao(const QString& name);
	std::shared_ptr<const QOpenGLVertexArrayObject> vao(const QString& name) const;
	std::shared_ptr<QOpenGLFramebufferObject> fbo(const QString& name);
	std::shared_ptr<const QOpenGLFramebufferObject> fbo(const QString& name) const;

protected:
	float														_depth;
	QMatrix4x4													_modelViewProjection;
	QMap<QString, std::shared_ptr<QOpenGLShaderProgram>>		_shaderPrograms;
	QMap<QString, std::shared_ptr<QOpenGLTexture>>				_textures;
	QMap<QString, std::shared_ptr<QOpenGLBuffer>>				_vbos;
	QMap<QString, std::shared_ptr<QOpenGLVertexArrayObject>>	_vaos;
	QMap<QString, std::shared_ptr<QOpenGLFramebufferObject>>	_fbos;
};