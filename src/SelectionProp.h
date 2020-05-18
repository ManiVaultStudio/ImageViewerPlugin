#pragma once

#include "Prop.h"

class SelectionLayer;

/**
 * Selection prop class
 *
 * This prop renders a selection layer onto the screen using OpenGL
 *
 * @author Thomas Kroes
 */
class SelectionProp : public Prop
{
	Q_OBJECT

public: // Construction/destruction

	/**
	 * Constructor
	 * @param selectionLayer Pointer to the associated selection layer
	 * @param name Name of the prop
	 */
	SelectionProp(SelectionLayer* selectionLayer, const QString& name);

	/** Destructor */
	~SelectionProp();

public: // Rendering

	/**
	 * Renders the prop
	 * @param nodeMVP Node model view projection matrix
	 * @param opacity Render opacity [0-1]
	 */
	void render(const QMatrix4x4& nodeMVP, const float& opacity) override;

	/** Returns the bounding rectangle of the prop */
	QRectF boundingRectangle() const;

protected: // Inherited

	/** Initializes the prop */
	void initialize() override;

protected:

	/** Updates the internal model matrix */
	void updateModelMatrix();
};