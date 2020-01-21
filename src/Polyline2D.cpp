#include "Polyline2D.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QDebug>

Polyline2D::Polyline2D(QSharedPointer<QOpenGLShaderProgram> shaderProgram, const bool& closed /*= true*/, const float& lineWidth /*= 1.f*/, const float& textureScale /*= 0.05f*/) :
	_shaderProgram(shaderProgram),
	_vao(QSharedPointer<QOpenGLVertexArrayObject>::create()),
	_vbo(QSharedPointer<QOpenGLBuffer>::create()),
	_closed(closed),
	_lineWidth(lineWidth),
	_textureScale(textureScale)
{
}

void Polyline2D::initialize()
{
	qDebug() << "Polyline2D::initialize()";

	const auto stride = 5 * sizeof(GLfloat);

	if (_shaderProgram->bind()) {
		_vao->bind();
		_vbo->bind();

		_shaderProgram->enableAttributeArray(0);
		_shaderProgram->enableAttributeArray(1);
		_shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);
		_shaderProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

		_vbo->release();
		_vao->release();

		_shaderProgram->release();
	}
}

void Polyline2D::setPoints(QVector<QVector2D> points)
{
	qDebug() << "Polyline2D::setPoints()";

	if (_closed && points.size() > 2) {
		points.append(points.first());
	}

	QVector<float> vertexData;

	const auto noPoints = points.size();

	vertexData.reserve(noPoints * 5);

	/*
	const auto pWorld0 = _imageViewerWidget->screenToWorld(QPointF(0.0f, 0.0f));
	const auto pWorld1 = _imageViewerWidget->screenToWorld(QPointF(lineWidth, 0.0f));
	const auto worldLineWidth = (pWorld1 - pWorld0).length();
	*/
	const auto halfLineWidth = 0.5f * 100.0f;

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

		if (id >= 0 && id < noPoints) {
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

	auto vbo = QSharedPointer<QOpenGLBuffer>::create();
	auto vao = QSharedPointer<QOpenGLVertexArrayObject>::create();

	vbo->bind();
	{
		vbo->setUsagePattern(QOpenGLBuffer::DynamicDraw);
		vbo->allocate(vertexData.constData(), vertexData.count() * sizeof(GLfloat));
		vbo->release();
	}
}

void Polyline2D::render()
{
	qDebug() << "Polyline2D::render()";

	_vao->bind();
	{
		_vbo->bind();
		{
			//glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexData.size() / 5);
		}
		_vbo->release();
	}
	_vao->release();
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