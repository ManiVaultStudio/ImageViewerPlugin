#pragma once

#include "Prop.h"
#include "WindowLevelImage.h"

class Node;

/**
 * TODO
 *
 * @author Thomas Kroes
 */
class PointsProp : public Prop
{
	Q_OBJECT

public:
	/** TODO */
	PointsProp(Node* node, const QString& name);

	/** Destructor */
	~PointsProp();

	/** Renders the prop */
	void render(const QMatrix4x4& nodeMVP, const float& opacity) override;

	/** Computes the enveloping bounding rectangle of the prop */
	QRectF boundingRectangle() const;

protected: // Inherited

	/** Initializes the prop */
	void initialize() override;

public: // Configuration

	/** TODO */
	WindowLevelImage& channel(const int& id);

	/** TODO */
	const WindowLevelImage& channel(const int& id) const;

	/** TODO */
	void setChannelImage(const std::uint32_t& channelId, const QImage& image);

	/** TODO */
	void setColorMap(const QImage& colorMap);

protected:

	/** TODO */
	void updateModelMatrix();

private:
	std::int32_t		_noChannels;		/** Number of active channels */
	WindowLevelImage	_channels[3];		/** Image */
};