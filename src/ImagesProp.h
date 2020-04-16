#pragma once

#include "Prop.h"
#include "WindowLevelImage.h"

class Node;

/**
 * Image prop class
 * @author Thomas Kroes
 */
class ImagesProp : public Prop
{
	Q_OBJECT

public:
	/** TODO */
	ImagesProp(Node* node, const QString& name);

	/** Destructor */
	~ImagesProp();

	/** Renders the prop */
	void render(const QMatrix4x4& nodeMVP, const float& opacity) override;

protected: // Inherited

	/** Initializes the prop */
	void initialize() override;

public: // Configuration

	/** TODO */
	WindowLevelImage& image();

	/** TODO */
	const WindowLevelImage& image() const;

	/** TODO */
	void setImage(const QImage& image);

protected:

	/** TODO */
	void updateModelMatrix();

private:
	WindowLevelImage	_windowLevelImage;				/** Image */
};

using SharedImageLayerProp = QSharedPointer<ImagesProp>;