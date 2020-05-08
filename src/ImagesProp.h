#pragma once

#include "Prop.h"
#include "Channel.h"

class ImagesLayer;

/**
 * Image prop class
 * @author Thomas Kroes
 */
class ImagesProp : public Prop
{
	Q_OBJECT

public:
	/** TODO */
	ImagesProp(ImagesLayer* imagesLayer, const QString& name);

	/** Destructor */
	~ImagesProp();

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
};