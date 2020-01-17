#pragma once

#include "Common.h"

#include "StackedRenderer.h"

#include <QSize>
#include <QVector>

/**
 * Quad renderer
 * This renderer class is used to display quads using OpenGL
 */
class QuadRenderer : public StackedRenderer
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * @param depth Depth (layer) to render content at
	 */
	QuadRenderer(const float& depth);

public:
	/** Initializes the renderer */
	void init() override;

	/** Resizes the renderer */
	void resize(QSize renderSize) override;

	/** Renders the content */
	void render() override;

public:
	/** Returns the size of the quad */
	QSize size() const;

	/**
	 * Sets the size of the image quad
	 * @param size Size of the image quad
	 */
	void setSize(const QSize& size);

protected:
	/** Create the necessary OpenGL shader programs */
	void createShaderPrograms() override  = 0;

	/** Create the necessary OpenGL Vertex Buffer Objects (VBO) */
	void createVBOs() override;

	/** Create the necessary OpenGL Vertex Array Objects (VAO) */
	void createVAOs() override;

private:
	/** Create the quad using OpenGL*/
	void createQuad();

signals:
	/**
	 * Invoked when the quad size has changed
	 * @param size Size of the quad
	 */
	void sizeChanged(const QSize& size);

protected:
	QSize					_size;						/*! Size of the rendered quad */
	QVector<GLfloat>		_vertexData;				/*! Quad vertex data */
	static std::uint32_t	_quadVertexAttribute;		/*! Quad vertex attribute location */
	static std::uint32_t	_quadTextureAttribute;		/*! Quad texture attribute location */
};