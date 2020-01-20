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
	/** Return the drawing depth */
	float depth() const;

	/**
	 * Set drawing depth
	 * @param depth Depth (layer) at which to draw
	 */
	void setDepth(const float& depth);

	/** Returns the model-view-projection matrix */
	QMatrix4x4 modelViewProjection() const;

	/**
	 * Sets the model-view-projection matrix
	 * @param modelViewProjection Model-view-projection matrix
	 */
	void setModelViewProjection(const QMatrix4x4& modelViewProjection);

protected:
	/** Create the necessary OpenGL shader programs */
	virtual void createShaderPrograms();

	/** Create the necessary OpenGL texture */
	virtual void createTextures();

	/** Create the necessary OpenGL Vertex Buffer Objects (VBO) */
	virtual void createVBOs();

	/** Create the necessary OpenGL Vertex Array Objects (VAO) */
	virtual void createVAOs();

	/** Lookup OpenGL shader program by name */
	QSharedPointer<QOpenGLShaderProgram> shaderProgram(const QString& name);

	/** Lookup OpenGL shader program by name */
	QSharedPointer<const QOpenGLShaderProgram> shaderProgram(const QString& name) const;

	/** Lookup OpenGL texture by name */
	QSharedPointer<QOpenGLTexture> texture(const QString& name);

	/** Lookup OpenGL texture by name */
	QSharedPointer<const QOpenGLTexture> texture(const QString& name) const;

	/** Lookup OpenGL Vertex Buffer Object (VBO) by name */
	QSharedPointer<QOpenGLBuffer> vbo(const QString& name);

	/** Lookup OpenGL Vertex Buffer Object (VBO) by name */
	QSharedPointer<const QOpenGLBuffer> vbo(const QString& name) const;

	/** Lookup OpenGL Vertex Array Object (VAO) by name */
	QSharedPointer<QOpenGLVertexArrayObject> vao(const QString& name);

	/** Lookup OpenGL Vertex Array Object (VAO) by name */
	QSharedPointer<const QOpenGLVertexArrayObject> vao(const QString& name) const;

	/** Lookup OpenGL Frame Buffer Object (FBO) by name */
	QSharedPointer<QOpenGLFramebufferObject> fbo(const QString& name);

	/** Lookup OpenGL Frame Buffer Object (FBO) by name */
	QSharedPointer<const QOpenGLFramebufferObject> fbo(const QString& name) const;

protected:
	float														_depth;					/** Depth (layer) at which to display the rendered content */
	QMatrix4x4													_modelViewProjection;	/** Model-view-projection matrix */
	QMap<QString, QSharedPointer<QOpenGLShaderProgram>>			_shaderPrograms;		/** OpenGL Shader program map */
	QMap<QString, QSharedPointer<QOpenGLTexture>>				_textures;				/** OpenGL texture map */
	QMap<QString, QSharedPointer<QOpenGLBuffer>>				_vbos;					/** OpenGL Vertex Buffer Object (VBO) map */
	QMap<QString, QSharedPointer<QOpenGLVertexArrayObject>>		_vaos;					/** OpenGL Vertex Attribute Object (VAO) map */
	QMap<QString, QSharedPointer<QOpenGLFramebufferObject>>		_fbos;					/** OpenGL frame buffer object map */
};