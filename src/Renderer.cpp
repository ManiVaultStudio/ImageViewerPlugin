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
    _parallelAnimationGroup(this),
    _zoomRectangleTopLeftAnimation(this, "zoomRectangleTopLeft"),
    _zoomRectangleSizeAnimation(this, "zoomRectangleSize"),
    _animationEnabled(),
    _zoomRectangleTopLeft(),
    _zoomRectangleSize()
{
    _parallelAnimationGroup.addAnimation(&_zoomRectangleTopLeftAnimation);
    _parallelAnimationGroup.addAnimation(&_zoomRectangleSizeAnimation);

    // Set duration of zoom animation
    _zoomRectangleTopLeftAnimation.setDuration(500);
    _zoomRectangleSizeAnimation.setDuration(500);

    QEasingCurve easingCurve;

    // Set type of easing
    easingCurve.setType(QEasingCurve::InOutQuad);

    // Apply the easing curves
    _zoomRectangleTopLeftAnimation.setEasingCurve(easingCurve);
    _zoomRectangleSizeAnimation.setEasingCurve(easingCurve);

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
    const auto normalizedScreenPoint    = QVector2D(1.0f, -1.0f) * getWorldPositionToNormalizedScreenPoint(position);
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
    const auto eye = QVector3D(getZoomRectangle().center().x(), getZoomRectangle().center().y(), 1);
    const auto center = QVector3D(getZoomRectangle().center().x(), getZoomRectangle().center().y(), 0);
    const auto up       = QVector3D(0, 1, 0);

    // Create look-at transformation matrix
    lookAt.lookAt(eye, center, up);

    const auto viewerSize           = getParentWidgetSize();
    const auto totalMargins         = 2 * _zoomMargin;
    const auto factorX              = static_cast<float>(viewerSize.width()) / static_cast<float>(getZoomRectangle().width());
    const auto factorY              = static_cast<float>(viewerSize.height()) / static_cast<float>(getZoomRectangle().height());
    const auto scaleFactor          = factorX < factorY ? factorX : factorY;

    const auto d = 1.0f - (2 * _zoomMargin) / std::max(viewerSize.width(), viewerSize.height());

    // Create scale matrix
    scale.scale(scaleFactor * d, scaleFactor * d, scaleFactor * d);

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
    // Compute screen bounding rectangle extremes
    const auto topLeftScreen        = getWorldPositionToScreenPoint(QVector3D(worldBoundingRectangle.bottomLeft()));
    const auto bottomRightScreen    = getWorldPositionToScreenPoint(QVector3D(worldBoundingRectangle.topRight()));

    return QRect(topLeftScreen, bottomRightScreen);
}

void Renderer::panBy(const QPointF& delta)
{
    qDebug() << "Pan by" << delta;

    const auto p1 = getScreenPointToWorldPosition(getViewMatrix(), QPoint()).toPointF() ;
    const auto p2 = getScreenPointToWorldPosition(getViewMatrix(), delta.toPoint()).toPointF() ;

    _zoomRectangleTopLeft = getZoomRectangle().topLeft() + (p2 - p1);

    emit zoomRectangleChanged();
}

float Renderer::getZoomPercentage() const
{
    if (!_worldBoundingRectangle.isValid() || !getZoomRectangle().isValid())
        return 1.0f;

    const auto viewerSize       = getParentWidgetSize();
    const auto totalMargins     = 2 * _zoomMargin;
    const auto factorX          = static_cast<float>(_worldBoundingRectangle.width()) / static_cast<float>(getZoomRectangle().width());
    const auto factorY          = static_cast<float>(_worldBoundingRectangle.height()) / static_cast<float>(getZoomRectangle().height());
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
    const auto factorX          = static_cast<float>(_worldBoundingRectangle.width()) / static_cast<float>(zoomPercentage * getZoomRectangle().width());
    const auto factorY          = static_cast<float>(_worldBoundingRectangle.height()) / static_cast<float>(zoomPercentage * getZoomRectangle().height());
    const auto scaleFactor      = factorX < factorY ? factorX : factorY;

    _zoomRectangleTopLeft   = viewerCenter - QPoint(0.5f * scaleFactor * getZoomRectangle().width(), 0.5f * scaleFactor * getZoomRectangle().height());
    _zoomRectangleSize      = getZoomRectangle().size() * scaleFactor;

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

    setZoomRectangle(_worldBoundingRectangle);
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
    const auto v1   = getZoomRectangle().topLeft() - p1;
    const auto v2   = v1 / factor;

    _zoomRectangleTopLeft   = p1 + v2;
    _zoomRectangleSize      = getZoomRectangle().size() / factor;

    emit zoomRectangleChanged();
}

QRectF Renderer::getZoomRectangle() const
{
    return QRectF(_zoomRectangleTopLeft, _zoomRectangleSize);
}

void Renderer::setZoomRectangle(const QRectF& zoomRectangle)
{
    if (!getZoomRectangle().isValid() || !_animationEnabled) {
        _zoomRectangleTopLeft   = zoomRectangle.topLeft();
        _zoomRectangleSize      = zoomRectangle.size();

        return;
    }

    // Configure the zoom rectangle top-left animation
    _zoomRectangleTopLeftAnimation.setStartValue(getZoomRectangle().topLeft());
    _zoomRectangleTopLeftAnimation.setEndValue(zoomRectangle.topLeft());

    // Configure the zoom rectangle size animation
    _zoomRectangleSizeAnimation.setStartValue(getZoomRectangle().size());
    _zoomRectangleSizeAnimation.setEndValue(zoomRectangle.size());

    // Start the parallel animation
    _parallelAnimationGroup.start();
}

bool Renderer::getAnimationEnabled() const
{
    return _animationEnabled;
}

void Renderer::setAnimationEnabled(const bool& animationEnabled)
{
    _animationEnabled = animationEnabled;
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
