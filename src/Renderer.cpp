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

const QMap<Renderer::InteractionMode, QString> Renderer::interactionModes = {
    { Renderer::None, "No interaction" },
    { Renderer::Navigation, "Navigation" },
    { Renderer::LayerEditing, "Layer editing" }
};

Renderer::Renderer(QOpenGLWidget* parent) :
    QObject(parent),
    hdps::Renderer(),
    _mousePositions(),
    _mouseButtons(),
    _pan(),
    _zoom(1.f),
    _zoomSensitivity(0.1f),
    _margin(25),
    _interactionMode(InteractionMode::LayerEditing)
{
    this->installEventFilter(parent);
}

void Renderer::init()
{
}

void Renderer::render()
{
    static_cast<QOpenGLWidget*>(parent())->update();
}

void Renderer::handleEvent(QEvent* event)
{
    if (_interactionMode != InteractionMode::Navigation)
        return;

    switch (event->type())
    {
        case QEvent::MouseButtonPress:
        {
            auto mouseEvent = static_cast<QMouseEvent*>(event);

            if (mouseEvent->buttons() & Qt::LeftButton) {
                _mousePositions << mouseEvent->pos();
            }

            emit mousePositionChanged(_mousePositions);

            break;
        }

        case QEvent::MouseButtonRelease:
        {
            _mousePositions.clear();
            break;
        }

        case QEvent::MouseMove:
        {
            auto mouseEvent = static_cast<QMouseEvent*>(event);

            _mousePositions << mouseEvent->pos();

            const auto noMousePositions = _mousePositions.size();

            if (mouseEvent->buttons() & Qt::LeftButton && noMousePositions >= 2) {
                const auto pPrevious    = QVector2D(_mousePositions[noMousePositions - 2]);
                const auto pCurrent     = QVector2D(_mousePositions[noMousePositions - 1]);
                const auto vDelta       = (pCurrent - pPrevious) / _zoom;

                pan(vDelta);
                render();
            }

            emit mousePositionChanged(_mousePositions);

            break;
        }

        case QEvent::Wheel:
        {
            auto wheelEvent = static_cast<QWheelEvent*>(event);

            const auto zoomCenter = wheelEvent->position().toPoint();

            if (wheelEvent->angleDelta().ry() < 0) {
                zoomAround(zoomCenter, 1.0f - _zoomSensitivity);
            }
            else {
                zoomAround(zoomCenter, 1.0f + _zoomSensitivity);
            }

            emit mousePositionChanged(_mousePositions);

            render();

            break;
        }
    }
}

Renderer::InteractionMode Renderer::interactionMode() const
{
    return _interactionMode;
}

void Renderer::setInteractionMode(const InteractionMode& interactionMode)
{
    qDebug() << "Set interaction mode to" << interactionModes.value(interactionMode);

    _interactionMode = interactionMode;
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
    scale.scale(_zoom);

    return scale * lookAt;
}

QMatrix4x4 Renderer::getProjectionMatrix() const
{
    const auto halfSize = getParentWidgetSize() / 2;

    QMatrix4x4 matrix;

    matrix.ortho(-halfSize.width(), halfSize.width(), -halfSize.height(), halfSize.height(), -1000.0f, +1000.0f);

    return matrix;
}

void Renderer::pan(const QVector2D& delta)
{
    qDebug() << "Pan by" << delta;

    _pan.setX(_pan.x() + delta.x());
    _pan.setY(_pan.y() + delta.y());
}

float Renderer::zoom() const
{
    return _zoom;
}

void Renderer::zoomBy(const float& factor)
{
    if (factor == 0.f)
        return;

    qDebug() << "Zoom by" << factor << "to" << _zoom;

    _zoom *= factor;
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

    pan(-vPanDelta);
}

void Renderer::zoomToWorldRectangle(const QRectF& rectangle, const std::uint32_t& margin /*= 50*/)
{
    if (!rectangle.isValid())
        throw std::runtime_error("Zoom rectangle is invalid.");

    qDebug() << "Zoom to rectangle" << rectangle;

    // Move to center of the world bounding rectangle
    _pan = QVector2D(rectangle.center());

    // Compute the scale factor
    const auto parentWidgetSize = getParentWidgetSize();
    const auto totalMargins     = 2 * margin;
    const auto factorX          = (parentWidgetSize.width() - totalMargins) / static_cast<float>(rectangle.width());
    const auto factorY          = (parentWidgetSize.height() - totalMargins) / static_cast<float>(rectangle.height());

    // Assign the zoom factor
    _zoom = factorX < factorY ? factorX : factorY;
}

void Renderer::zoomToObject(const Renderable& renderable, const std::uint32_t& margin /*= 50*/)
{
    zoomToWorldRectangle(renderable.getWorldBoundingRectangle(), margin);
}

void Renderer::resetView()
{
    qDebug() << "Reset view";

    _pan.setX(0);
    _pan.setY(0);

    _zoom = 1.f;
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
