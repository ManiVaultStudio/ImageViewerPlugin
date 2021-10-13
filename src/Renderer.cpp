#include "Renderer.h"
#include "Renderable.h"

#include <QtMath>
#include <QMenu>
#include <QDebug>
#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>

#include <stdexcept>

Renderer::Renderer(QOpenGLWidget* parent) :
    QObject(parent),
    hdps::Renderer(),
    _zoomSensitivity(0.1f),
    _zoomMargin(50.0f),
    _worldBoundingRectangle(),
    _zoomAnimation(this, "zoomRectangle")
{
    // Set duration of zoom animation
    _zoomAnimation.setDuration(500);

    /*
    QEasingCurve easingCurve;

    // Set type of easing
    easingCurve.setType(QEasingCurve::InSine);

    // Apply the easing curve
    _zoomAnimation.setEasingCurve(easingCurve);
    */

    // Re-render when the zoom rectangle changes
    connect(this, &Renderer::zoomRectangleChanged, this, [this]() {
        getParentWidget()->update();
    });
}

void Renderer::init()
{
}

void Renderer::render()
{
    static_cast<QOpenGLWidget*>(parent())->update();
}

QVector3D Renderer::getScreenPointToWorldPosition(const QMatrix4x4& modelViewMatrix, const QPoint& screenPoint) const
{
    return QVector3D(screenPoint.x(), getParentWidgetSize().height()- screenPoint.y(), 0).unproject(modelViewMatrix, getProjectionMatrix(), QRect(0, 0, getParentWidgetSize().width(), getParentWidgetSize().height()));
}

QVector2D Renderer::getWorldPositionToNormalizedScreenPoint(const QVector3D& position) const
{
    const auto clipSpacePos = getProjectionMatrix() * (getViewMatrix() * QVector4D(position, 1.0));
    return (clipSpacePos.toVector3D() / clipSpacePos.w()).toVector2D();
}

QPoint Renderer::getWorldPositionToScreenPoint(const QVector3D& position) const
{
    const auto normalizedScreenPoint    = getWorldPositionToNormalizedScreenPoint(position);
    const auto viewSize                 = QVector2D(getParentWidgetSize().width(), getParentWidgetSize().height());

    return (viewSize * ((QVector2D(1.0f, 1.0f) + normalizedScreenPoint) / 2.0f)).toPoint();
}

QVector2D Renderer::getScreenPointToNormalizedScreenPoint(const QVector2D& screenPoint) const
{
    const auto viewSize = QVector2D(getParentWidgetSize().width(), getParentWidgetSize().height());
    return QVector2D(-1.f, -1.f) + 2.f * (QVector2D(screenPoint.x(), getParentWidgetSize().height() - screenPoint.y()) / viewSize);
}

QMatrix4x4 Renderer::getScreenToNormalizedScreenMatrix() const
{
    QMatrix4x4 translate, scale;

    translate.translate(-1.0f, -1.0f, 0.0f);
    scale.scale(2.0f / static_cast<float>(getParentWidgetSize().width()), 2.0f / static_cast<float>(getParentWidgetSize().height()), 1.0f);
    
    return translate * scale;
}

QMatrix4x4 Renderer::getNormalizedScreenToScreenMatrix() const
{
    QMatrix4x4 translate, scale;

    const auto size     = QSizeF(getParentWidgetSize());
    const auto halfSize = 0.5f * size;

    scale.scale(halfSize.width(), halfSize.height(), 1.0f);
    translate.translate(size.width(), 1, 0.0f);

    return translate * scale;
}

QMatrix4x4 Renderer::getViewMatrix() const
{
    QMatrix4x4 lookAt, scale;

    // Construct look-at parameters
    const auto eye      = QVector3D(_zoomRectangle.center().x(), _zoomRectangle.center().y(), 1);
    const auto center   = QVector3D(_zoomRectangle.center().x(), _zoomRectangle.center().y(), 0);
    const auto up       = QVector3D(0, 1, 0);

    // Create look-at transformation matrix
    lookAt.lookAt(eye, center, up);

    const auto viewerSize   = getParentWidgetSize();
    const auto totalMargins = 2 * _zoomMargin;
    const auto factorX      = static_cast<float>(viewerSize.width() - totalMargins) / static_cast<float>(_zoomRectangle.width() - 1);
    const auto factorY      = static_cast<float>(viewerSize.height() - totalMargins) / static_cast<float>(_zoomRectangle.height() - 1);
    const auto scaleFactor  = factorX < factorY ? factorX : factorY;

    // Create scale matrix
    scale.scale(scaleFactor, scaleFactor, scaleFactor);

    // Return composite matrix of scale and look-at transformation matrix
    return scale * lookAt;
}

QMatrix4x4 Renderer::getProjectionMatrix() const
{
    // Compute half of the widget size
    const auto halfSize = getParentWidgetSize() / 2;

    QMatrix4x4 matrix;

    // Create an orthogonal transformation matrix
    matrix.ortho(-halfSize.width(), halfSize.width(), -halfSize.height(), halfSize.height(), -1000.0f, +1000.0f);

    return matrix;
}

QRect Renderer::getScreenRectangleFromWorldRectangle(const QRectF& worldBoundingRectangle) const
{
    // Get extremes in world coordinates
    const auto worldTopLeft         = QVector3D(worldBoundingRectangle.topLeft().toPoint());
    const auto worldBottomRight     = QVector3D(worldBoundingRectangle.bottomRight().toPoint());

    // Get extremes in screen coordinates
    const auto screenTopLeft        = getWorldPositionToScreenPoint(worldTopLeft);
    const auto screenBottomRight    = getWorldPositionToScreenPoint(worldBottomRight);

    return QRect(screenTopLeft, screenBottomRight);
}

void Renderer::panBy(const QPointF& delta)
{
    qDebug() << "Pan by" << delta;

    const auto p1 = getScreenPointToWorldPosition(getViewMatrix(), QPoint()).toPointF() ;
    const auto p2 = getScreenPointToWorldPosition(getViewMatrix(), delta.toPoint()).toPointF() ;

    _zoomRectangle = QRectF(_zoomRectangle.topLeft() + (p2 - p1), _zoomRectangle.size());

    emit zoomRectangleChanged();
}

float Renderer::getZoomPercentage() const
{
    if (!_worldBoundingRectangle.isValid() || !_zoomRectangle.isValid())
        return 1.0f;

    const auto viewerSize       = getParentWidgetSize();
    const auto totalMargins     = 2 * _zoomMargin;
    const auto factorX          = static_cast<float>(_worldBoundingRectangle.width()) / static_cast<float>(_zoomRectangle.width());
    const auto factorY          = static_cast<float>(_worldBoundingRectangle.height()) / static_cast<float>(_zoomRectangle.height());
    const auto scaleFactor      = factorX < factorY ? factorX : factorY;

    return scaleFactor;
}

void Renderer::setZoomPercentage(const float& zoomPercentage)
{
    if (zoomPercentage < 0.05f)
        return;

    qDebug() << "Set zoom percentage" << zoomPercentage;

    const auto viewerSize       = getParentWidgetSize();
    const auto viewerCenter     = getScreenPointToWorldPosition(getViewMatrix(), QPoint(viewerSize.width(), viewerSize.height()) / 2).toPointF();
    const auto totalMargins     = 2 * _zoomMargin;
    const auto factorX          = static_cast<float>(_worldBoundingRectangle.width()) / static_cast<float>(zoomPercentage * _zoomRectangle.width());
    const auto factorY          = static_cast<float>(_worldBoundingRectangle.height()) / static_cast<float>(zoomPercentage * _zoomRectangle.height());
    const auto scaleFactor      = factorX < factorY ? factorX : factorY;

    _zoomRectangle = QRectF(viewerCenter - QPoint(0.5f * scaleFactor * _zoomRectangle.width(), 0.5f * scaleFactor * _zoomRectangle.height()), _zoomRectangle.size() * scaleFactor);

    emit zoomRectangleChanged();
}

float Renderer::getZoomSensitivity() const
{
    return _zoomSensitivity;
}

float Renderer::getZoomMargin() const
{
    return _zoomMargin;
}

void Renderer::setZoomMargin(const float& zoomMargin)
{
    _zoomMargin = zoomMargin;
}

QRectF Renderer::getWorldBoundingBox() const
{
    return _worldBoundingRectangle;
}

void Renderer::setWorldBoundingRectangle(const QRectF& worldBoundingRectangle)
{
    _worldBoundingRectangle = worldBoundingRectangle;
}

void Renderer::zoomAround(const QPoint& screenPoint, const float& factor)
{
    qDebug() << "Zoom around" << screenPoint << "by" << factor;

    const auto p1   = getScreenPointToWorldPosition(getViewMatrix(), screenPoint).toPointF();
    const auto v1   = _zoomRectangle.topLeft() - p1;
    const auto v2   = v1 / factor;

    _zoomRectangle = QRectF(p1 + v2, _zoomRectangle.size() / factor);

    emit zoomRectangleChanged();
}

void Renderer::setZoomRectangle(const QRectF& zoomRectangle)
{
    _zoomAnimation.setStartValue(_zoomRectangle.isValid() ? _zoomRectangle : zoomRectangle);
    _zoomAnimation.setEndValue(zoomRectangle);
    _zoomAnimation.start();
}

void Renderer::bindOpenGLContext()
{
    getParentWidget()->makeCurrent();
}

void Renderer::releaseOpenGLContext()
{
    getParentWidget()->doneCurrent();
}

QOpenGLWidget* Renderer::getParentWidget() const
{
    return dynamic_cast<QOpenGLWidget*>(parent());
}

QOpenGLContext* Renderer::getOpenGLContext() const
{
    return getParentWidget()->context();
}

QSize Renderer::getParentWidgetSize() const
{
    return getParentWidget()->size();
}
