#pragma once

#include "Prop.h"
#include "LayerNode.h"

class Actor;

/**
 * Image prop class
 * @author Thomas Kroes
 */
class ImagesLayerProp : public Prop
{
	Q_OBJECT

public:
	/** TODO */
	ImagesLayerProp(RenderNode* renderNode, const QString& name, const LayerNode::Type& type);

	/** Destructor */
	~ImagesLayerProp();

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
	LayerNode::Type		_type;					/** TODO */
	QImage			_image;					/** TODO */
	float			_displayRange[2];		/** TODO */
	float			_opacity;				/** TODO */
	std::uint32_t	_order;					/** TODO */
};

using SharedImageLayerProp = QSharedPointer<ImagesLayerProp>;