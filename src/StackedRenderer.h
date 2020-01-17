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

/**
 * Stacked renderer class
 * This renderer class is used to render content in layers using OpenGL
 */
class StackedRenderer : public QObject, public hdps::Renderer
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * @param depth Depth (layer) to render content at
	 */
	StackedRenderer(const float& depth);
	
public:
	/** Initializes the renderer */
	void init() override;

	/** Destroys the renderer */
	void destroy() override;

	/** Return whether the renderer is initialized */
	virtual bool isInitialized() const = 0;

public:
	float depth() const;
	void setDepth(const float& depth);
	QMatrix4x4 modelViewProjection() const;
	void setModelViewProjection(const QMatrix4x4& modelViewProjection);

protected:
	virtual void createShaderPrograms();
	virtual void createTextures();
	virtual void createVBOs();
	virtual void createVAOs();

	QSharedPointer<QOpenGLShaderProgram> shaderProgram(const QString& name);
	QSharedPointer<const QOpenGLShaderProgram> shaderProgram(const QString& name) const;
	QSharedPointer<QOpenGLTexture> texture(const QString& name);
	QSharedPointer<const QOpenGLTexture> texture(const QString& name) const;
	QSharedPointer<QOpenGLBuffer> vbo(const QString& name);
	QSharedPointer<QOpenGLVertexArrayObject> vao(const QString& name);
	QSharedPointer<const QOpenGLVertexArrayObject> vao(const QString& name) const;
	QSharedPointer<QOpenGLFramebufferObject> fbo(const QString& name);
	QSharedPointer<const QOpenGLFramebufferObject> fbo(const QString& name) const;

protected:
	float														_depth;					/*! Depth (layer) at which to display the rendered content */
	QMatrix4x4													_modelViewProjection;	/*! Model-view-projection matrix */
	QMap<QString, QSharedPointer<QOpenGLShaderProgram>>			_shaderPrograms;		/*! OpenGL Shader program map */
	QMap<QString, QSharedPointer<QOpenGLTexture>>				_textures;				/*! OpenGL texture map */
	QMap<QString, QSharedPointer<QOpenGLBuffer>>				_vbos;					/*! OpenGL Vertex Buffer Object (VBO) map */
	QMap<QString, QSharedPointer<QOpenGLVertexArrayObject>>		_vaos;					/*! OpenGL Vertex Attribute Object (VAO) map */
	QMap<QString, QSharedPointer<QOpenGLFramebufferObject>>		_fbos;					/*! OpenGL frame buffer object map */
};