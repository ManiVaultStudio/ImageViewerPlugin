#pragma once

#include "Prop.h"

class Actor;

#include <QOpenGLFramebufferObject>

/**
 * Selection buffer prop class
 * @author Thomas Kroes
 */
class SelectionBufferProp : public Prop
{
	Q_OBJECT

public:
	/** Constructor
	 * @param actor Parent actor
	 * @param name Name of the prop
	 */
	SelectionBufferProp(Actor* actor, const QString& name);

public:
	/**
	 * Sets the image
	 * @param image Image
	 */
	void setImage(std::shared_ptr<QImage> image);

protected:
	/** Initializes the prop */
	void initialize() override;

	/** Renders the prop */
	void render() override;

private:
	QSharedPointer<QOpenGLFramebufferObject>	_fbo;		/** Frame buffer object for off-screen computation of pixel selection */
};