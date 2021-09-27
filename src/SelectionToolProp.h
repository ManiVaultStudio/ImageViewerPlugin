#pragma once

#include "Prop.h"

#include <QScopedPointer>
#include <QOpenGLFramebufferObject>

class Layer;

/**
 * Selection tool prop class
 *
 * This prop provides functionality for selecting pixels using pixel selection tools (e.g. brush, polygon etc.)
 *
 * @author Thomas Kroes
 */
class SelectionToolProp : public Prop
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param layer Reference to layer in which the prop resides
     * @param name Name of the prop
     */
    SelectionToolProp(Layer& layer, const QString& name);

    /** Destructor */
    ~SelectionToolProp() = default;

public:

    /**
     * Renders the prop
     * @param modelViewProjectionMatrix Model view projection matrix
     */
    void render(const QMatrix4x4& modelViewProjectionMatrix) override;

    /**
     * Set image size
     * @param imageSize Image size
     */
    void setImageSize(const QSize& imageSize);

    /** Returns the bounding rectangle of the prop */
    QRectF getWorldBoundingRectangle() const override;

public: // Pixel selection

    /** 
     * Computes the pixel selection (based on the tool) and stores the result in an off-screen pixel selection buffer
     * @param mousePositions Mouse positions
     */
    void compute(const QVector<QPoint>& mousePositions);

    /** Resets the off-screen pixel selection buffer */
    void resetOffScreenSelectionBuffer();

    /** Returns the pixel selection in image format */
    QImage getSelectionImage();

protected: // Inherited

    /** Initializes the prop */
    void initialize() override;

protected: // Miscellaneous

	/** Updates the internal model matrix */
	void updateModelMatrix();

private: // Shader programs

    /** Loads the shader program for the selection tool rendering */
    void loadSelectionToolShaderProgram();

    /** Loads the shader program for the selection tool off-screen rendering */
    void loadSelectionToolOffScreenShaderProgram();

private:
    Layer&                                      _layer;     /** Reference to layer */
    QScopedPointer<QOpenGLFramebufferObject>    _fbo;       /** Frame Buffer Object for off screen pixel selection tools */
};