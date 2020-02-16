#pragma once

#include "Prop.h"

class Actor;
class Layer;

/**
 * Image prop class
 * @author Thomas Kroes
 */
class LayerProp : public Prop
{
	Q_OBJECT

public:
	/** Constructor
	 * @param actor Parent actor
	 * @param name Name of the prop
	 */
	LayerProp(Actor* actor, const QString& name, Layer* layer);

	/** Destructor */
	~LayerProp();

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

	/**
	 * Sets the display range
	 * @param displayRange Display range
	 */
	void setDisplayRange(const QPair<float, float>& displayRange);

private:
	Layer*					_layer;				/** Parent layer */
	QPair<float, float>		_displayRange;		/** Display range */
};