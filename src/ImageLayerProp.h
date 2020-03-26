#pragma once

#include "Prop.h"
#include "Layer.h"

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
	ImageLayerProp(Actor* actor, const QString& name, const LayerItem::Type& type);

	/** Destructor */
	~ImageLayerProp();

	/** Renders the prop */
	void render() override;

protected: // Inherited

	/** Initializes the prop */
	void initialize() override;

public: // Configuration

	/** TODO */
	void setImage(const QImage& image);

	/** TODO */
	void setDisplayRange(const float& min, const float& max);

	/** TODO */
	void setOpacity(const float& opacity);

	/** TODO */
	void setOrder(const std::uint32_t& order);

protected:

	/** TODO */
	void updateModelMatrix();

private:
	LayerItem::Type		_type;
	QImage			_image;
	float			_displayRange[2];
	float			_opacity;
	std::uint32_t	_order;
};

using SharedImageLayerProp = QSharedPointer<ImageLayerProp>;