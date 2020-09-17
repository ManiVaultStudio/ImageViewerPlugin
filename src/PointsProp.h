#pragma once

#include "Prop.h"
#include "Channel.h"

class PointsLayer;

/**
 * Points prop class
 *
 * This prop renders a points layer onto the screen using OpenGL
 *
 * @author Thomas Kroes
 */
class PointsProp : public Prop
{
	Q_OBJECT

public: // Enumerations

	/** Texture identifiers */
	enum class TextureId {
		ColorMap = 0,
		Channel1,
		Channel2,
		Channel3,
		Channel4
	};

public: // Construction/destruction

	/**
	 * Constructor
	 * @param pointsLayer Pointer to the associated points layer
	 * @param name Name of the prop
	 */
	PointsProp(PointsLayer* pointsLayer, const QString& name);

	/** Destructor */
	~PointsProp() override;

public: // Rendering

	/**
	 * Renders the prop
	 * @param nodeMVP Node model view projection matrix
	 * @param opacity Render opacity [0-1]
	 */
	void render(const QMatrix4x4& nodeMVP, const float& opacity) override;

	/** Returns the bounding rectangle of the prop */
	QRectF getBoundingRectangle() const override;

protected: // Inherited

	/** Initializes the prop */
	void initialize() override;

protected: // Miscellaneous

	/** Updates the internal model matrix */
	void updateModelMatrix();

private:
	std::int32_t		_noChannels;		/** Number of active channels */

	static const GLuint channels[];
};