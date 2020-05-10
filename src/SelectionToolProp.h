#pragma once

#include "Prop.h"

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

	/** Computes the enveloping bounding rectangle of the prop */
	QRectF boundingRectangle() const;

protected: // Inherited

	/** Initializes the prop */
	void initialize() override;

protected:

	/** TODO */
	void updateModelMatrix();

private:
};