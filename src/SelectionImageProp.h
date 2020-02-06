#pragma once

#include "Prop.h"

class Actor;

#include <QColor>

/**
 * Selection image prop class
 * @author Thomas Kroes
 */
class SelectionImageProp : public Prop
{
	Q_OBJECT

public:
	/** Constructor
	 * @param actor Parent actor
	 * @param name Name of the prop
	 */
	SelectionImageProp(Actor* actor, const QString& name);

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
	void setImage(std::shared_ptr<QImage> image);

	/** Returns the image size */
	QSize imageSize() const;

private:
	QColor		_color;		/** Selection color */
};