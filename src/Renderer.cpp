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
    _pan(),
    _zoomLevel(1.f),
    _zoomPercentage(1.0f),
    _zoomSensitivity(0.1f),
    _zoomMargin(25.0f),
    _worldBoundingRectangle()
{
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

    lookAt.lookAt(QVector3D(_pan.x(), _pan.y(), -1), QVector3D(_pan.x(), _pan.y(), 0), QVector3D(0, 1, 0));
    scale.scale(_zoomLevel);

    return scale * lookAt;
}

QMatrix4x4 Renderer::getProjectionMatrix() const
{
    const auto halfSize = getParentWidgetSize() / 2;

    QMatrix4x4 matrix;

    matrix.ortho(-halfSize.width(), halfSize.width(), -halfSize.height(), halfSize.height(), -1000.0f, +1000.0f);

    return matrix;
}

QRect Renderer::getScreenBoundingRectangle(const QRectF& worldBoundingRectangle) const
{
    // Get extremes in world coordinates
    const auto worldTopLeft         = QVector3D(worldBoundingRectangle.topLeft().toPoint());
    const auto worldBottomRight     = QVector3D(worldBoundingRectangle.bottomRight().toPoint());

    // Get extremes in screen coordinates
    const auto screenTopLeft        = getWorldPositionToScreenPoint(worldTopLeft);
    const auto screenBottomRight    = getWorldPositionToScreenPoint(worldBottomRight);

    return QRect(screenTopLeft, screenBottomRight);
}

void Renderer::panBy(const QVector2D& delta)
{
    qDebug() << "Pan by" << delta;

    setPan(QVector2D(_pan.x() + delta.x(), _pan.y() + delta.y()));
}

void Renderer::setPan(const QVector2D& pan)
{
    _pan = pan;

    emit panChanged(_pan);
}

float Renderer::getZoomLevel() const
{
    return _zoomLevel;
}

void Renderer::setZoomLevel(const float& zoom)
{
    //if (zoom == _zoomLevel)
        //return;

    _zoomLevel = zoom;

    // Compute zoom percentage
    const auto screenBoundingRectangle  = getScreenBoundingRectangle(_worldBoundingRectangle);
    const auto viewerSize               = getParentWidgetSize();
    const auto totalMargins             = 2 *_zoomMargin;
    const auto factorX                  = static_cast<float>(std::abs(screenBoundingRectangle.width() - 1)) / static_cast<float>(viewerSize.width() - totalMargins);
    const auto factorY                  = static_cast<float>(std::abs(screenBoundingRectangle.height() - 1)) / static_cast<float>(viewerSize.height() - totalMargins);
    
    _zoomPercentage = factorX > factorY ? factorX : factorY;

    emit zoomLevelChanged(_zoomLevel);
    emit zoomPercentageChanged(_zoomPercentage);
}

float Renderer::getZoomPercentage() const
{
    return _zoomPercentage;
}

void Renderer::setZoomPercentage(const float& zoomPercentage)
{
    //if (zoomPercentage == _zoomPercentage)
        //return;

    _zoomPercentage = zoomPercentage;

    // Compute zoom level if the world bounding box is valid
    if (_worldBoundingRectangle.isValid()) {
        
        const auto parentWidgetSize = getParentWidgetSize();
        const auto totalMargins     = 2 * _zoomMargin;
        const auto factorX          = (parentWidgetSize.width() - totalMargins) / static_cast<float>(_worldBoundingRectangle.width() / _zoomPercentage);
        const auto factorY          = (parentWidgetSize.height() - totalMargins) / static_cast<float>(_worldBoundingRectangle.height() / _zoomPercentage);

        _zoomLevel = factorX < factorY ? factorX : factorY;
    }

    emit zoomLevelChanged(_zoomLevel);
    emit zoomPercentageChanged(_zoomPercentage);
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

    setZoomLevel(_zoomLevel);
}

void Renderer::zoomBy(const float& factor)
{
    if (factor == 0.f)
        return;

    qDebug() << "Zoom by" << factor << "to" << _zoomLevel;

    setZoomLevel(_zoomLevel * factor);
}

void Renderer::zoomAround(const QPoint& screenPoint, const float& factor)
{
    zoomBy(factor);

    qDebug() << "Zoom around" << screenPoint << "by" << factor;

    const auto pWorld       = getScreenPointToWorldPosition(getViewMatrix(), screenPoint);
    const auto pAnchor      = pWorld.toVector2D();
    const auto pPanOld      = _pan;
    const auto vPanOld      = pPanOld - pAnchor;
    const auto vPanNew      = factor * vPanOld;
    const auto vPanDelta    = vPanNew - vPanOld;

    panBy(-vPanDelta);
}

void Renderer::zoomToWorldRectangle(const QRectF& rectangle)
{
    if (!rectangle.isValid())
        throw std::runtime_error("Zoom rectangle is invalid.");

    qDebug() << "Zoom to rectangle" << rectangle;

    // Move to center of the world bounding rectangle
    setPan(QVector2D(rectangle.center()));

    // Compute the scale factor
    const auto parentWidgetSize = getParentWidgetSize();
    const auto totalMargins     = 2 * _zoomMargin;
    const auto factorX          = (parentWidgetSize.width() - totalMargins) / static_cast<float>(rectangle.width());
    const auto factorY          = (parentWidgetSize.height() - totalMargins) / static_cast<float>(rectangle.height());

    // Assign the zoom factor
    setZoomLevel(factorX < factorY ? factorX : factorY);
}

void Renderer::zoomToObject(const Renderable& renderable)
{
    zoomToWorldRectangle(renderable.getWorldBoundingRectangle());
}

void Renderer::resetView()
{
    qDebug() << "Reset view";

    setPan(QVector2D());
    setZoomLevel(0.0f);
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
