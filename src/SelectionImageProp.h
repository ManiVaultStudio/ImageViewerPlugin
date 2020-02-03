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

public:
	/**
	 * Sets the image
	 * @param image Image
	 */
	void setImage(std::shared_ptr<QImage> image);

	/** Returns the image size */
	QSize imageSize() const;

protected:
	/** Initializes the prop */
	void initialize() override;

	/** Renders the prop */
	void render() override;

signals:
	/**
	 * Signals that the image size changed
	 * @para size Image size
	 */
	void imageSizeChanged(const QSize& imageSize);

private:
	QColor		_color;		/** Selection color */
};