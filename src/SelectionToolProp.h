#pragma once

#include "Prop.h"

#include <QScopedPointer>
#include <QOpenGLFramebufferObject>

class SelectionLayer;

/**
 * Selection tool prop class
 *
 * TODO: Write description
 *
 * @author Thomas Kroes
 */
class SelectionToolProp : public Prop
{
	Q_OBJECT

public:
	/** TODO */
	SelectionToolProp(SelectionLayer* selectionLayer, const QString& name);

	/** Destructor */
	~SelectionToolProp();

	/** Renders the prop */
	void render(const QMatrix4x4& nodeMVP, const float& opacity) override;

	/** Computes the pixel selection (based on the tool) and stores the result in an off-screen frame buffer object */
	void computePixelSelection();

	/** Computes the enveloping bounding rectangle of the prop */
	QRectF boundingRectangle() const;

protected: // Inherited

	/** Initializes the prop */
	void initialize() override;

protected:

	/** TODO */
	void updateModelMatrix();

private:

	/** Loads the shader program for the selection tool rendering */
	void loadSelectionToolShaderProgram();

	/** Loads the shader program for the selection tool off-screen rendering */
	void loadSelectionToolOffScreenShaderProgram();

private:
	QScopedPointer<QOpenGLFramebufferObject>	_fbo;		/** Frame Buffer Object for off screen pixel selection tools */
};