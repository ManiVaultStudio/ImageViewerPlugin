#pragma once

#include "Prop.h"

class Actor;

#include <QColor>

/**
 * Selection rectangle prop class
 * @author Thomas Kroes
 */
class SelectionRectangleProp : public Prop
{
	Q_OBJECT

public:
	/** Constructor
	 * @param actor Parent actor
	 * @param name Name of the prop
	 */
	SelectionRectangleProp(Actor* actor, const QString& name);

public:

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