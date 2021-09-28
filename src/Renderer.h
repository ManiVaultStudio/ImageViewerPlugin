#pragma once

#include "renderers/Renderer.h"

#include <QWidget>
#include <QColor>
#include <QVector2D>

class QMouseEvent;
class QWheelEvent;
class QOpenGLWidget;
class QMenu;
class QOpenGLWidget;

class Renderable;

/**
 * Renderer class
 *
 * Class for rendering image layers on the screen using the layers model and OpenGL
 *
 * @author Thomas Kroes
 */
class Renderer : public QObject, public hdps::Renderer
{
    Q_OBJECT

public: // Construction

    /**
     * Constructor
     * @param parentWidget Parent OpenGL image viewer widget
     */
    Renderer(QOpenGLWidget* parent);

    /** Initialize the renderer */
    void init() override;

    /** Renders the layers */
    void render() override;
    
    /** Destroys the renderer */
    void destroy() override {};

    /** Resizes the renderer */
    void resize(QSize renderSize) override {};

public: // Coordinate conversions

    /**
     * Convert point in screen coordinates to point in world coordinates
     * @param modelViewMatrix Model-view matrix
     * @param screenPoint Point in screen coordinates [0..width, 0..height]
     * @return Position in world coordinates
     */
    QVector3D getScreenPointToWorldPosition(const QMatrix4x4& modelViewMatrix, const QPoint& screenPoint) const;

    /**
     * Convert position in world coordinates to point in normalized screen coordinates
     * @param position Position in world coordinates
     * @return Point in normalized screen coordinates [-1..1, -1..1]
     */
    QVector2D getWorldPositionToNormalizedScreenPoint(const QVector3D& position) const;

    /**
     * Convert position in world coordinates to point in screen coordinates
     * @param position Position in world coordinates
     * @return Point in screen coordinates [0..width, 0..height]
     */
    QPoint getWorldPositionToScreenPoint(const QVector3D& position) const;

    /**
     * Convert point in screen coordinates to point in normalized screen coordinates
     * @param screenPoint Point in screen coordinates [0..width, 0..height]
     * @return Point in normalized screen coordinates [-1..1, -1..1]
     */
    QVector2D getScreenPointToNormalizedScreenPoint(const QVector2D& screenPoint) const;

    /** Returns the matrix that converts screen coordinates [0..width, 0..height] to normalized screen coordinates [-1..1, -1..1] */
    QMatrix4x4 getScreenToNormalizedScreenMatrix() const;

    /** Returns the matrix that converts normalized screen coordinates [-1..1, -1..1] to screen coordinates [0..width, 0..height] */
    QMatrix4x4 getNormalizedScreenToScreenMatrix() const;

    /** Returns the view matrix */
    QMatrix4x4 getViewMatrix() const;

    /** Returns the projection matrix */
    QMatrix4x4 getProjectionMatrix() const;

    /**
     * Get screen bounding rectangle from world bounding rectangle
     * @param worldBoundingRectangle World bounding rectangle
     */
    virtual QRect getScreenBoundingRectangle(const QRectF& worldBoundingRectangle) const final;

public: // Navigation

    /**
     * Move the view horizontally/vertically
     * @param delta Amount to move
     */
    void panBy(const QVector2D& delta);

    /**
     * Set pan
     * @param pan Pan
     */
    void setPan(const QVector2D& pan);

    /** Get the zoom level */
    float getZoomLevel() const;

    /**
     * Set the zoom level
     * @param zoom Zoom level
     */
    void setZoomLevel(const float& zoom);

    /** Get the zoom percentage */
    float getZoomPercentage() const;

    /**
     * Set the zoom percentage
     * @param zoomPercentage Zoom percentage
     */
    void setZoomPercentage(const float& zoomPercentage);

    /** get the zoom level sensitivity */
    float getZoomSensitivity() const;

    /** get the zoom margin */
    float getZoomMargin() const;

    /**
     * Set zoom margin
     * @param zoomMargin Zoom margin
     */
    void setZoomMargin(const float& zoomMargin);

    /** get the world bounding box of all objects */
    QRectF getWorldBoundingBox() const;

    /**
     * Set world bounding box
     * @param worldBoundingBox World bounding box
     */
    void setWorldBoundingRectangle(const QRectF& worldBoundingRectangle);

    /**
     * Zoom the view
     * @param factor Factor to zoom by
     */
    void zoomBy(const float& factor);

    /**
     * Zoom around screen point
     * @param screenPoint Point in screen coordinates
     * @param factor Factor to zoom by
     */
    void zoomAround(const QPoint& screenPoint, const float& factor);

    /** Zoom to rectangle in world coordinates */
    void zoomToWorldRectangle(const QRectF& rectangle);

    /** Zoom to selected pixels */
    void zoomToObject(const Renderable& renderable);

    /** Reset the view */
    void resetView();

public: // Miscellaneous

    /** Returns the parent widget */
    QOpenGLWidget* getParentWidget() const;

    /** Returns the parent widget size */
    QSize getParentWidgetSize() const;

    /** Returns the parent widget */
    QOpenGLContext* getOpenGLContext() const;

    /** Binds the OpenGL context */
    void bindOpenGLContext();

    /** Releases the OpenGL context */
    void releaseOpenGLContext();

signals:

    /**
     * Signals that the zoom level changed
     * @param zoomLevel Zoom level
     */
    void zoomLevelChanged(const float& zoomLevel);

    /**
     * Signals that the zoom percentage changed
     * @param zoomPercentage Zoom percentage
     */
    void zoomPercentageChanged(const float& zoomPercentage);

    /**
     * Signals that the pan changed
     * @param pan Pan
     */
    void panChanged(const QVector2D& pan);

protected:
    QVector2D   _pan;                       /** Move view horizontally/vertically */
    float       _zoomLevel;                 /** Zoom level */
    float       _zoomPercentage;            /** Zoom percentage */
    float       _zoomSensitivity;           /** Zoom sensitivity */
    float       _zoomMargin;                /** Zoom margin */
    QRectF      _worldBoundingRectangle;    /** World bounding rectangle */
};