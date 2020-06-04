#pragma once

#include "Prop.h"

#include <QScopedPointer>
#include <QOpenGLFramebufferObject>

class SelectionLayer;

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

public: // Construction/destruction

	/**
	 * Constructor
	 * @param selectionLayer Pointer to the associated selection layer
	 * @param name Name of the prop
	 */
	SelectionToolProp(SelectionLayer* selectionLayer, const QString& name);

	/** Destructor */
	~SelectionToolProp() override;

public: // Rendering

	/** Renders the prop */
	void render(const QMatrix4x4& nodeMVP, const float& opacity) override;

	/** Returns the bounding rectangle of the prop */
	QRectF boundingRectangle() const override;

public: // Pixel selection

	/** Computes the pixel selection (based on the tool) and stores the result in an off-screen pixel selection buffer */
	void compute();

	/** Resets the off-screen pixel selection buffer */
	void reset();

	/** Returns the pixel selection in image format */
	QImage selectionImage();

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
	QScopedPointer<QOpenGLFramebufferObject>	_fbo;		/** Frame Buffer Object for off screen pixel selection tools */
};