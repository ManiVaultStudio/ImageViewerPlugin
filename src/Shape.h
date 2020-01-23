#pragma once

#include "Common.h"

#include <QObject>
#include <QMap>
#include <QSharedPointer>

class QOpenGLShaderProgram;
class QOpenGLBuffer;
class QOpenGLVertexArrayObject;
class QOpenGLTexture;

/**
 * OpenGL shape class
 * @author Thomas Kroes
 */
class Shape : public QObject
{
	Q_OBJECT

public:
	/** Constructor
	 * @param name Name of the shape
	 */
	Shape(const QString& name);

	/** Destructor */
	~Shape();

	/**
	 * Performs a cleanup e.g. removes various OpenGL buffers
	 * Must be called in the correct OpenGL context (the context from which initializeGL() is called)
	 */
	virtual void destroy();

	/** Initialize the shape (must be called in appropriate OpenGL context)
	 * @param shaderProgram Shared pointer to the OpenGL shader program
	 * @param texture Shared pointer to the OpenGL texture
	 */
	virtual void initialize();

	/**
	 * Determines whether the shape is properly initialized (all buffers etc. are setup correctly)
	 * @return Whether the shape is initialized
	 */
	bool isInitialized() const;

	/** Renders the shape */
	virtual void render();

protected:
	/** Adds the OpenGL shader programs that the shape needs */
	virtual void addShaderPrograms() = 0;

	/** Adds the OpenGL vertex array objects that the shape needs */
	virtual void addVAOs() = 0;

	/** Adds the OpenGL vertex buffer objects that the shape needs */
	virtual void addVBOs() = 0;

	/** Adds the OpenGL textures that the shape needs */
	virtual void addTextures() = 0;

	/**
	 * Adds an OpenGL shader program
	 * @param name Name of the shader program
	 * @param shaderProgram Shared pointer to the shader program
	 */
	void addShaderProgram(const QString& name, QSharedPointer<QOpenGLShaderProgram> shaderProgram);

	/**
	 * Adds an OpenGL Vertex Array Object (VAO)
	 * @param name Name of the VAO
	 * @param vao Shared pointer to the VAO
	 */
	void addVAO(const QString& name, QSharedPointer<QOpenGLVertexArrayObject> vao);

	/**
	 * Adds an OpenGL Vertex Buffer (VBO)
	 * @param name Name of the VBO
	 * @param vbo Shared pointer to the VBO
	 */
	void addVBO(const QString& name, QSharedPointer<QOpenGLBuffer> vbo);

	/**
	 * Adds an OpenGL texture
	 * @param name Name of the texture
	 * @param texture Shared pointer to the texture
	 */
	void addTexture(const QString& name, QSharedPointer<QOpenGLTexture> texture);

	/**
	 * Retrieves an OpenGL shader program by name
	 * @param name Name of the shader program
	 */
	QSharedPointer<QOpenGLShaderProgram> shaderProgram(const QString& name);

	/**
	 * Retrieves an OpenGL Vertex Array Object (VAO) by name
	 * @param name Name of vertex array object
	 */
	QSharedPointer<QOpenGLVertexArrayObject> vao(const QString& name);

	/**
	 * Retrieves an OpenGL Vertex Buffer Object (VBO) by name
	 * @param name Name of vertex buffer object
	 */
	QSharedPointer<QOpenGLBuffer> vbo(const QString& name);

	/**
	 * Retrieves an OpenGL texture by name
	 * @param name Name of the texture
	 */
	QSharedPointer<QOpenGLTexture> texture(const QString& name);

signals:
	/** Signals that the shape has been successfully initialized */
	void initialized();

protected:
	QString														_name;				/** Name of the shape */
	bool														_initialized;		/** Whether the shape is initialized or not */
	QMap<QString, QSharedPointer<QOpenGLShaderProgram>>			_shaderPrograms;	/** OpenGL shader program */
	QMap<QString, QSharedPointer<QOpenGLVertexArrayObject>>		_vaos;				/** OpenGL Vertex Array Object (VAO) */
	QMap<QString, QSharedPointer<QOpenGLBuffer>>				_vbos;				/** OpenGL Vertex Buffer Object (VBO) */
	QMap<QString, QSharedPointer<QOpenGLTexture>>				_textures;			/** OpenGL texture */
};