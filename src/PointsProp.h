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
	void setChannel(const int& id, const QImage& channel);

protected:

	/** TODO */
	void updateModelMatrix();

private:
	WindowLevelImage	_channels[3];				/** Image */
};