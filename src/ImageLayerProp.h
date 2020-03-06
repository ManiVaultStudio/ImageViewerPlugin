#pragma once

#include "Prop.h"

class Actor;
class LayersModel;

/**
 * Image prop class
 * @author Thomas Kroes
 */
class ImageLayerProp : public Prop
{
	Q_OBJECT

public:
	/** TODO */
	ImageLayerProp(Actor* actor, const QString& name);

	/** Destructor */
	~ImageLayerProp();

protected: // Inherited

	/** Initializes the prop */
	void initialize() override;

	/** Renders the prop */
	void render() override;

public: // Configuration

	/** TODO */
	void setImage(const QImage& image);

	/** TODO */
	void setDisplayRange(const float& min, const float& max);

	/** TODO */
	void setOpacity(const float& opacity);

private:
	QImage		_image;
	float		_displayRange[2];
	float		_opacity;
};

using SharedImageLayerProp = QSharedPointer<ImageLayerProp>;