#pragma once

#include "Prop.h"

class Actor;

#include <QColor>

/**
 * Selection image prop class
 * @author Thomas Kroes
 */
class SelectionImage : public Prop
{
	Q_OBJECT

public:
	/** Constructor
	 * @param actor Parent actor
	 * @param name Name of the prop
	 */
	SelectionImage(Actor* actor, const QString& name);

public:
	/** Returns the image size */
	QSize imageSize() const;

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
	QColor		_color;		/** Selection color */
};