#include "Polyline2D.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QDebug>

Polyline2D::Polyline2D(const QString& name, const bool& closed /*= true*/, const float& lineWidth /*= 1.f*/, const float& textureScale /*= 0.05f*/) :
	Shape(name),
	_closed(closed),
	_lineWidth(lineWidth),
	_textureScale(textureScale),
	_noPoints(0)
{
}

void Polyline2D::initialize()
{
	Shape::initialize();

	const auto stride = 5 * sizeof(GLfloat);

	if (this->shaderProgram()->isLinked() && this->shaderProgram()->bind()) {
		vao()->bind();
		vbo()->bind();

		this->shaderProgram()->enableAttributeArray(0);
		this->shaderProgram()->enableAttributeArray(1);
		this->shaderProgram()->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);
		this->shaderProgram()->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

		vbo()->release();
		vao()->release();

		this->shaderProgram()->release();

		_initialized = true;
	}
}

void Polyline2D::setPoints(QVector<QVector2D> points)
{
	qDebug() << "Set polyline points";

	if (_closed && points.size() > 2) {
		points.append(points.first());
	}

	_noPoints = points.size();

	QVector<float> vertexData;

	const auto noPoints = points.size();

	vertexData.reserve(noPoints * 5);

	const auto halfLineWidth = 0.5f * _lineWidth;

	QVector<QPair<QVector2D, QVector2D>> coordinates;

	auto halfAngleVector = [](const QVector2D& v0, const QVector2D& v1) {
		if (std::abs(QVector2D::dotProduct(v0, v1)) == 1.0f)
			return QVector2D(-v0.y(), v0.x()).normalized();

		return (v0 + v1).normalized();
	};

	auto outsideVectorAtPoint = [&](const std::uint32_t& id, const QVector2D& direction) {
		if (id == 0) {
			if (_closed) {
				const auto p = points[0];
				const auto v0 = (points[noPoints - 2] - p).normalized();
				const auto v1 = (points[1] - p).normalized();
				const auto vHalfAngle = halfAngleVector(v0, v1);
				const auto vOutside = halfLineWidth * vHalfAngle * (1.0f / std::abs(QVector2D::dotProduct(vHalfAngle, QVector2D(-v0.y(), v0.x()).normalized())));

				if (QVector2D::dotProduct(vOutside, direction) < 0)
					return -vOutside;

				return vOutside;
			}
			else {
				const auto p = points[1];
				const auto a = (points[0] - p).normalized();
				const auto b = (points[2] - p).normalized();
				const auto v = (points[1] - points[0]).normalized();
				const auto ha = halfAngleVector(a, b);
				const auto vp = halfLineWidth * QVector2D(-v.y(), v.x());

				if (QVector2D::dotProduct(vp, ha) < 0)
					return -vp;

				return vp;
			}
		}

		if (id == noPoints - 1) {
			if (_closed) {
				const auto p = points[0];
				const auto v0 = (points[noPoints - 2] - p).normalized();
				const auto v1 = (points[1] - p).normalized();
				const auto vHalfAngle = halfAngleVector(v0, v1);
				const auto vOutside = halfLineWidth * vHalfAngle * (1.0f / std::abs(QVector2D::dotProduct(vHalfAngle, QVector2D(-v0.y(), v0.x()).normalized())));

				if (QVector2D::dotProduct(vOutside, direction) < 0)
					return -vOutside;

				return vOutside;
			}
			else {
				const auto p = points[noPoints - 1];
				const auto a = (points[noPoints - 3] - p).normalized();
				const auto b = (points[noPoints - 2] - p).normalized();
				const auto v = (points[noPoints - 2] - points[noPoints - 1]).normalized();
				const auto ha = halfAngleVector(a, b);
				const auto vp = halfLineWidth * QVector2D(-v.y(), v.x());

				if (QVector2D::dotProduct(vp, ha) < 0)
					return -vp;

				return vp;
			}
		}

		const auto p = points[id];
		const auto v0 = (points[id - 1] - p).normalized();
		const auto v1 = (points[id + 1] - p).normalized();
		const auto vHalfAngle = halfAngleVector(v0, v1);
		const auto vOutside = halfLineWidth * vHalfAngle * (1.0f / std::abs(QVector2D::dotProduct(vHalfAngle, QVector2D(-v0.y(), v0.x()).normalized())));

		//qDebug() << v0 << v1 << vHalfAngle;
		if (QVector2D::dotProduct(vHalfAngle, direction) < 0)
			return -vOutside;

		return vOutside;
	};

	auto outsidePoint = [&](const std::uint32_t& id, const QVector2D& direction) {
		if (noPoints == 0)
			return QVector2D(0.0f, 0.0f);

		if (noPoints == 1)
			return QVector2D(0.0f, 0.0f);

		if (noPoints == 2) {
			const auto v = (points[1] - points[0]).normalized();
			return halfLineWidth * QVector2D(-v.y(), v.x());
		}

		if (id >= 0 && id < static_cast<std::uint32_t>(noPoints)) {
			return outsideVectorAtPoint(id, direction);
		}

		return QVector2D(0.0f, 0.0f);
	};

	const auto v = (points[1] - points[0]).normalized();

	auto direction = QVector2D(-v.y(), v.x()).normalized();

	for (int j = 0; j < points.size(); ++j)
	{
		const auto pOutside = outsidePoint(j, direction);
		const auto vInner = points[j] - pOutside;
		const auto vOuter = points[j] + pOutside;

		coordinates.append(QPair(vInner, vOuter));

		if (j < points.size() - 1) {
			const auto p1 = points[j + 1];
			const auto p0 = points[j];
			const auto vPar = (p1 - p0).normalized();
			const auto vPerp = QVector2D(-vPar.y(), vPar.x()).normalized();
			const auto dot = QVector2D::dotProduct(pOutside, vPerp);

			direction = vPerp;

			if (dot < 0)
				direction *= -1.0f;
		}
	}

	auto addVertex = [&vertexData](const float& x, const float& y, const float& u, const float& v) {
		vertexData.append(x);
		vertexData.append(y);
		vertexData.append(0.0f);
		vertexData.append(u);
		vertexData.append(v);
	};

	for (const auto& coordinate : coordinates) {
		const auto inner = coordinate.first;
		const auto outer = coordinate.second;

		addVertex(inner.x(), inner.y(), 0.0f, 0.0f);
		addVertex(outer.x(), outer.y(), 0.0f, 1.0f);
	}
	
	vbo()->bind();
	{
		vbo()->setUsagePattern(QOpenGLBuffer::DynamicDraw);
		vbo()->allocate(vertexData.constData(), vertexData.count() * sizeof(GLfloat));
		vbo()->release();
	}

	qDebug() << vertexData;
}

QSharedPointer<QOpenGLShaderProgram> Polyline2D::shaderProgram()
{
	return _shaderPrograms["Polyline"];
}

const QSharedPointer<QOpenGLShaderProgram> Polyline2D::shaderProgram() const
{
	return _shaderPrograms["Polyline"];
}

QSharedPointer<QOpenGLVertexArrayObject> Polyline2D::vao()
{
	return _vaos["Polyline"];
}

const QSharedPointer<QOpenGLVertexArrayObject> Polyline2D::vao() const
{
	return _vaos["Polyline"];
}

QSharedPointer<QOpenGLBuffer> Polyline2D::vbo()
{
	return _vbos["Polyline"];
}

const QSharedPointer<QOpenGLBuffer> Polyline2D::vbo() const
{
	return _vbos["Polyline"];
}

QSharedPointer<QOpenGLTexture> Polyline2D::texture()
{
	return _textures["Polyline"];
}

const QSharedPointer<QOpenGLTexture> Polyline2D::texture() const
{
	return _textures["Polyline"];
}

float Polyline2D::lineWidth() const
{
	return _lineWidth;
}

void Polyline2D::setLineWidth(const float& lineWidth)
{
	if (lineWidth == _lineWidth)
		return;

	qDebug() << "Set polyline line width to" << QString::number(lineWidth, 'f', 1);

	_lineWidth = lineWidth;

	emit lineWidthChanged(_lineWidth);
}

bool Polyline2D::isTextured() const
{
	return texture().get() != nullptr && texture()->isCreated();
}

void Polyline2D::render()
{
	if (!canRender())
		return;

	Shape::render();

	qDebug() << "Render" << _name << "shape";

	if (isTextured()) {
		texture()->bind();

		qDebug() << "Using texture";
	}

	if (shaderProgram()->bind()) {
		shaderProgram()->setUniformValue("lineTexture", 0);
		shaderProgram()->setUniformValue("transform", _modelViewProjection);

		vao()->bind();
		{
			//qDebug() << _noPoints;
			glDrawArrays(GL_TRIANGLE_STRIP, 0, _noPoints * 2);
		}
		vao()->release();

		shaderProgram()->release();
	}

	if (isTextured()) {
		texture()->release();
	}
}

//void SelectionRenderer::drawPolyline(QVector<QPoint> pointsScreen, QOpenGLBuffer* vbo, QOpenGLVertexArrayObject* vao, const bool& closed /*= true*/, const float& lineWidth /*= 1.f*/, const float& textureScale /*= 0.05f*/)
// {
//	auto pointsWorld = QVector<QVector2D>();

//	for (const auto& pointScreen : pointsScreen)
//	{
//		const auto pointWorld = _imageViewerWidget->screenToWorld(pointScreen);
//		pointsWorld.push_back(QVector2D(pointWorld.x(), pointWorld.y()));
//	}

//	qDebug() << pointsWorld;

//	//drawPolyline(pointsWorld, vbo, vao, closed, lineWidth, textureScale);
//}