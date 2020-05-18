#pragma once

#include "Prop.h"
#include "Channel.h"

class ImagesLayer;

/**
 * Images prop class
 * 
 * This prop renders an images layer onto the screen using OpenGL
 * 
 * @author Thomas Kroes
 */
class ImagesProp : public Prop
{
	Q_OBJECT

public: // Construction/destruction
	
	/**
	 * Constructor
	 * @param imagesLayer Pointer to the associated images layer
	 * @param name Name of the prop
	 */
	ImagesProp(ImagesLayer* imagesLayer, const QString& name);

	/** Destructor */
	~ImagesProp();

public: // Rendering

	/**
	 * Renders the prop
	 * @param nodeMVP Node model view projection matrix
	 * @param opacity Render opacity [0-1]
	 */
	void render(const QMatrix4x4& nodeMVP, const float& opacity) override;

	/** Returns the enveloping bounding rectangle of the prop */
	QRectF boundingRectangle() const;

protected: // Inherited

	/** Initializes the prop */
	void initialize() override;

protected: // Miscellaneous

	/** Updates the internal model matrix */
	void updateModelMatrix();
};