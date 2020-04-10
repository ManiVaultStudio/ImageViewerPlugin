#pragma once

#include "Prop.h"

class Actor;

#include <QOpenGLFramebufferObject>

/**
 * Interim selection buffer prop class
 * @author Thomas Kroes
 */
class InterimSelectionProp : public Prop
{
	Q_OBJECT

public:
	/** Constructor
	 * @param actor Parent actor
	 * @param name Name of the prop
	 */
	InterimSelectionProp(Actor* actor, const QString& name);

protected: // Inherited

	/** Initializes the prop */
	void initialize() override;

	/** Returns whether the prop can be rendered */
	bool canRender() const override;

	/** Renders the prop */
	void render() override;

public:
	/**
	 * Sets the image size
	 * @param imageSize Image size
	 */
	void setImageSize(const QSize& imageSize);

	/** Updates the pixel selection buffer */
	void update();

	/** Resets the pixel selection buffer */
	void reset();

private:
	QSharedPointer<QOpenGLFramebufferObject>	_fbo;		/** Frame buffer object for off-screen computation of pixel selection */
};