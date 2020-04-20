#pragma once

#include "Prop.h"
#include "Channel.h"

class PointsLayer;

/**
 * Points prop class
 *
 * @author Thomas Kroes
 */
class PointsProp : public Prop
{
	Q_OBJECT

public:

	/** Texture identifiers */
	enum class TextureId {
		ColorMap = 0,
		Channel1,
		Channel2,
		Channel3,
		Channel4
	};

public:
	/** TODO */
	PointsProp(PointsLayer* pointsLayer, const QString& name);

	/** Destructor */
	~PointsProp();

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
	std::int32_t		_noChannels;		/** Number of active channels */
	QVector<Channel*>	_channels;			/** Channels */

	static const GLuint channels[];
};