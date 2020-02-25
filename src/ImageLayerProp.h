#pragma once

#include "Prop.h"
#include "ImageLayer.h"

class Actor;

/**
 * Image prop class
 * @author Thomas Kroes
 */
class ImageLayerProp : public Prop
{
	Q_OBJECT

public:
	/** TODO */
	ImageLayerProp(Actor* actor, const QString& name, SharedImageLayer imageLayer);

	/** Destructor */
	~ImageLayerProp();

protected: // Inherited

	/** Initializes the prop */
	void initialize() override;

	/** Renders the prop */
	void render() override;

public:
	/**
	 * Sets the image
	 * @param image Image
	 */
	void setImage(const QImage& image);

private:
	SharedImageLayer	_imageLayer;	/** TODO */
};

using SharedImageLayerProp = QSharedPointer<ImageLayerProp>;