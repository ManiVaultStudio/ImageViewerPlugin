//#pragma once
//
//#include "Common.h"
//
//#include "renderers/Renderer.h"
//
//#include <QWidget>
//#include <QColor>
//#include <QVector2D>
//
//class ViewerWidget;
//class ImageDatasetsModel;
//class SelectionPickerActor;
//
//class QMouseEvent;
//class QWheelEvent;
//class QOpenGLWidget;
//class QMenu;
//class QOpenGLWidget;
//
///**
// * Renderer class
// *
// * Class for rendering image layers on the screen using the layers model and OpenGL
// *
// * @author Thomas Kroes
// */
//class Renderer : public QObject, public hdps::Renderer
//{
//    Q_OBJECT
//
//public: // Construction
//
//    /**
//     * Constructor
//     * @param parentWidget Parent OpenGL image viewer widget
//     */
//    Renderer(QOpenGLWidget* parent);
//
//public: // 
//
//    /** Initialize the renderer */
//    void init() override;
//
//    /** Renders the layers */
//    void render() override;
//    
//    /** Destroys the renderer */
//    void destroy() override {};
//
//    /** Resizes the renderer */
//    void resize(QSize renderSize) override {};
//
//    /**
//     * Handles events passed on from widgets
//     * @param event Event
//     */
//    void handleEvent(QEvent* event);
//
//public: // Getters/setters
//
//    /** Returns the current interaction mode */
//    InteractionMode interactionMode() const;
//
//    /**
//     * Sets the current interaction mode
//     * @param interactionMode The interaction mode
//     */
//    void setInteractionMode(const InteractionMode& interactionMode);
//
//public: // Coordinate conversions
//
//    /** Convert point in screen coordinates to point in world coordinates
//     * @param modelViewMatrix Model-view matrix
//     * @param screenPoint Point in screen coordinates [0..width, 0..height]
//     * @return Position in world coordinates
//     */
//    QVector3D getScreenPointToWorldPosition(const QMatrix4x4& modelViewMatrix, const QPoint& screenPoint) const;
//
//    /** Convert position in world coordinates to point in normalized screen coordinates
//     * @param position Position in world coordinates
//     * @return Point in normalized screen coordinates [-1..1, -1..1]
//     */
//    QVector2D getWorldPositionToNormalizedScreenPoint(const QVector3D& position) const;
//
//    /** Convert position in world coordinates to point in screen coordinates
//     * @param position Position in world coordinates
//     * @return Point in screen coordinates [0..width, 0..height]
//     */
//    QPoint getWorldPositionToScreenPoint(const QVector3D& position) const;
//
//    /** Convert point in screen coordinates to point in normalized screen coordinates
//     * @param screenPoint Point in screen coordinates [0..width, 0..height]
//     * @return Point in normalized screen coordinates [-1..1, -1..1]
//     */
//    QVector2D getScreenPointToNormalizedScreenPoint(const QVector2D& screenPoint) const;
//
//    /** Returns the matrix that converts screen coordinates [0..width, 0..height] to normalized screen coordinates [-1..1, -1..1] */
//    QMatrix4x4 getScreenToNormalizedScreenMatrix() const;
//
//    /** Returns the matrix that converts normalized screen coordinates [-1..1, -1..1] to screen coordinates [0..width, 0..height] */
//    QMatrix4x4 getNormalizedScreenToScreenMatrix() const;
//
//    /** Returns the view matrix */
//    QMatrix4x4 getViewMatrix() const;
//
//    /** Returns the projection matrix */
//    QMatrix4x4 getProjectionMatrix() const;
//
//public: // Navigation
//
//    /**
//     * Move the view horizontally/vertically
//     * @param delta Amount to move
//     */
//    void pan(const QVector2D& delta);
//
//    /** Return the current zoom level */
//    float zoom() const;
//
//    /**
//     * Zoom the view
//     * @param factor Factor to zoom by
//     */
//    void zoomBy(const float& factor);
//
//    /**
//     * Zoom around screen point
//     * @param screenPoint Point in screen coordinates
//     * @param factor Factor to zoom by
//     */
//    void zoomAround(const QPoint& screenPoint, const float& factor);
//
//    /**
//     * Zoom to rectangle
//     * @param rectangle Rectangle to zoom to
//     */
//    void zoomToRectangle(const QRectF& rectangle);
//
//    /** Zoom to selected pixels */
//    void zoomToSelection();
//
//    /** Reset the view */
//    void resetView();
//
//public: // Miscellaneous
//
//    /** Returns the parent widget */
//    QOpenGLWidget* getParentWidget() const;
//
//    /** Returns the parent widget size */
//    QSize getParentWidgetSize() const;
//
//    /** Returns the parent widget */
//    QOpenGLContext* getOpenGLContext() const;
//
//    /** Binds the OpenGL context */
//    void bindOpenGLContext();
//
//    /** Releases the OpenGL context */
//    void releaseOpenGLContext();
//
//protected:
//    QVector<QPoint>     _mousePositions;        /** Recorded mouse positions */
//    int                 _mouseButtons;          /** State of the left, middle and right mouse buttons */
//    QVector2D           _pan;                   /** Move view horizontally/vertically */
//    float               _zoom;                  /** Zoom view in/out */
//    float               _zoomSensitivity;       /** Zoom sensitivity */
//    int                 _margin;                /** Margin between image and viewer widget boundaries */
//    InteractionMode     _interactionMode;       /** Interaction mode e.g. navigation and layer editing */
//};