#pragma once

#include "Prop.h"

class SelectionLayer;

/**
 * Selection prop class
 *
 * TODO
 *
 * @author Thomas Kroes
 */
class SelectionProp : public Prop
{
	Q_OBJECT

public:
	/** TODO */
	SelectionProp(SelectionLayer* selectionLayer, const QString& name);

	/** Destructor */
	~SelectionProp();

	/** Renders the prop */
	void render(const QMatrix4x4& nodeMVP, const float& opacity) override;

	/** Computes the enveloping bounding rectangle of the prop */
	QRectF boundingRectangle() const;

protected: // Inherited

	/** Initializes the prop */
	void initialize() override;

public: // Configuration

	/** TODO */
	void setImage(const QImage& image);

protected:

	/** TODO */
	void updateModelMatrix();

private:
};