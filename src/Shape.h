#pragma once

#include "Common.h"

#include <QObject>
#include <QColor>
#include <QMatrix4x4>
#include <QMap>
#include <QSharedPointer>

class QOpenGLShaderProgram;
class QOpenGLBuffer;
class QOpenGLVertexArrayObject;
class QOpenGLTexture;
class QOpenGLFramebufferObject;

class Actor;
class Renderer;

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
	Shape(Actor* actor, const QString& name);

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

	/** Returns the shape name */
	QString name() const;

	/**
	 * Sets the name
	 * @param name Shape name
	 */
	void setName(const QString& name);

	/** Returns whether the shape can be rendered */
	virtual bool canRender() const;
	
	/** Renders the shape */
	virtual void render();

	/**
	 * Bind shader program in the current OpenGL context
	 * @param name Name of the OpenGL shader program
	 */
	bool bindShaderProgram(const QString& name);

	/**
	 * Adds an OpenGL shader program
	 * @param name Name of the shader program
	 * @param shaderProgram Shared pointer to the shader program
	 */
	void setShaderProgram(const QString& name, QSharedPointer<QOpenGLShaderProgram> shaderProgram);

	/**
	 * Adds an OpenGL Vertex Array Object (VAO)
	 * @param name Name of the VAO
	 * @param vao Shared pointer to the VAO
	 */
	void setVAO(const QString& name, QSharedPointer<QOpenGLVertexArrayObject> vao);

	/**
	 * Adds an OpenGL Vertex Buffer (VBO)
	 * @param name Name of the VBO
	 * @param vbo Shared pointer to the VBO
	 */
	void setVBO(const QString& name, QSharedPointer<QOpenGLBuffer> vbo);

	/**
	 * Adds an OpenGL texture
	 * @param name Name of the texture
	 * @param texture Shared pointer to the texture
	 */
	void setTexture(const QString& name, QSharedPointer<QOpenGLTexture> texture);

	/**
	 * Adds an OpenGL Frame Buffer Object (FBO)
	 * @param name Name of the FBO
	 * @param fbo Shared pointer to the FBO
	 */
	void setFBO(const QString& name, QSharedPointer<QOpenGLFramebufferObject> fbo);

	/** Binds the OpenGL context */
	void bindOpenGLContext();

	/** Releases the OpenGL context */
	void releaseOpenGLContext();

	/** Returns the render color */
	QColor color() const;

	/** Sets the render color
	 * @param color Color
	 */
	void setColor(const QColor& color);

	Renderer* renderer();

protected:
	/** Adds the OpenGL shader programs that the shape needs */
	virtual void addShaderPrograms() {};

	/** Adds the OpenGL vertex array objects that the shape needs */
	virtual void addVAOs() {};

	/** Adds the OpenGL vertex buffer objects that the shape needs */
	virtual void addVBOs() {};

	/** Adds the OpenGL textures that the shape needs */
	virtual void addTextures() {};

	/** Adds the OpenGL frame buffer objects that the shape needs */
	virtual void addFBOs() {};

	/**
	 * Configure an OpenGL shader program (right after the shader program is bound in the render function)
	 * @param name Name of the OpenGL shader program
	 */
	virtual void configureShaderProgram(const QString& name);

	/** Updates the internal representation of the shape (e.g. vertex buffers and shader programs) */
	virtual void update();

	/**
	 * Retrieves an OpenGL shader program by name
	 * @param name Name of the shader program
	 */
	QSharedPointer<QOpenGLShaderProgram> shaderProgram(const QString& name);

	/**
	 * Retrieves an OpenGL shader program by name
	 * @param name Name of the shader program
	 */
	const QSharedPointer<QOpenGLShaderProgram> shaderProgram(const QString& name) const;

	/**
	 * Retrieves an OpenGL Vertex Array Object (VAO) by name
	 * @param name Name of vertex array object
	 */
	QSharedPointer<QOpenGLVertexArrayObject> vao(const QString& name);

	/**
	 * Retrieves an OpenGL Vertex Array Object (VAO) by name
	 * @param name Name of vertex array object
	 */
	const QSharedPointer<QOpenGLVertexArrayObject> vao(const QString& name) const;

	/**
	 * Retrieves an OpenGL Vertex Buffer Object (VBO) by name
	 * @param name Name of vertex buffer object
	 */
	QSharedPointer<QOpenGLBuffer> vbo(const QString& name);

	/**
	 * Retrieves an OpenGL Vertex Buffer Object (VBO) by name
	 * @param name Name of vertex buffer object
	 */
	const QSharedPointer<QOpenGLBuffer> vbo(const QString& name) const;

	/**
	 * Retrieves an OpenGL texture by name
	 * @param name Name of the texture
	 */
	QSharedPointer<QOpenGLTexture> texture(const QString& name);

	/**
	 * Retrieves an OpenGL texture by name
	 * @param name Name of the texture
	 */
	const QSharedPointer<QOpenGLTexture> texture(const QString& name) const;

	/**
	 * Retrieves an OpenGL Frame Buffer Object (FBO) by name
	 * @param name Name of the frame buffer object
	 */
	QSharedPointer<QOpenGLFramebufferObject> fbo(const QString& name);

	/**
	 * Retrieves an OpenGL Frame Buffer Object (FBO) by name
	 * @param name Name of the frame buffer object
	 */
	const QSharedPointer<QOpenGLFramebufferObject> fbo(const QString& name) const;

signals:
	/** Signals that the shape has been successfully initialized */
	void initialized();

	/** Signals that the shape name changed
	 * @param name New shape name
	 */
	void nameChanged(const QString& name);

	/**
	 * Signals that the render color changed
	 * @param color Render color
	 */
	void colorChanged(const QColor& color);

	/** Signals that the shape has been enabled or disabled
	 * @param enabled Whether the shape is enabled or not
	 */
	void enabledChanged(const bool& enabled);

	/** Signals that the model matrix changed
	 * @param modelMatrix Model matrix
	 */
	void modelMatrixChanged(const QMatrix4x4& modelMatrix);

	/** Signals that the shape changed */
	void changed(Shape* shape);

protected:
	Actor*														_actor;						/** Pointer to actor */
	QString														_name;						/** Name of the shape */
	bool														_active;					/** Shapes is being interacted with */
	QColor														_color;						/** Shape color */
	bool														_initialized;				/** Whether the shape is initialized or not */
	bool														_enabled;					/** Whether the shape is enabled or not */
	QMap<QString, QSharedPointer<QOpenGLShaderProgram>>			_shaderPrograms;			/** OpenGL shader program */
	QMap<QString, QSharedPointer<QOpenGLVertexArrayObject>>		_vaos;						/** OpenGL Vertex Array Object (VAO) */
	QMap<QString, QSharedPointer<QOpenGLBuffer>>				_vbos;						/** OpenGL Vertex Buffer Object (VBO) */
	QMap<QString, QSharedPointer<QOpenGLTexture>>				_textures;					/** OpenGL texture */
	QMap<QString, QSharedPointer<QOpenGLFramebufferObject>>		_fbos;						/** OpenGL Frame Buffer Objects (FBO) */
};