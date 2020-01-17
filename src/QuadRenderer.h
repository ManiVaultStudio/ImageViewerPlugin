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
	QuadRenderer(const float& depth);

public:
	void init() override;
	void resize(QSize renderSize) override;
	void render() override;

public:
	QSize size() const;
	void setSize(const QSize& size);

protected:
	void createShaderPrograms() override  = 0;
	void createVBOs() override;
	void createVAOs() override;

private:
	void createQuad();

signals:
	void sizeChanged(const QSize& size);

protected:
	QSize					_size;						/*! Size of the rendered quad */
	QVector<GLfloat>		_vertexData;				/*! Quad vertex data */
	static std::uint32_t	_quadVertexAttribute;		/*! Quad vertex attribute location */
	static std::uint32_t	_quadTextureAttribute;		/*! Quad texture attribute location */
};