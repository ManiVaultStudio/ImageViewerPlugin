#pragma once

#include "Common.h"

#include "StackedRenderer.h"

#include <QSize>
#include <QImage>
#include <QVector>
#include <QMatrix4x4>

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
	QSize					_size;
	QVector<GLfloat>		_vertexData;
	static std::uint32_t	_quadVertexAttribute;
	static std::uint32_t	_quadTextureAttribute;
};