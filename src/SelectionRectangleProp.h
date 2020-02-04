#pragma once

#include "PolylineProp.h"

#include <QRect>

class Actor;

/**
 * Selection rectangle prop class
 * @author Thomas Kroes
 */
class SelectionRectangleProp : public PolylineProp
{
public:
	/** Constructor
	 * @param actor Parent actor
	 * @param name Name of the prop
	 */
	SelectionRectangleProp(Actor* actor, const QString& name);

public:
	/** Returns the selection rectangle */
	QRect rectangle();

	/** Sets the selection rectangle
	 * @param rectangle Selection rectangle
	 */
	void setRectangle(const QRect& rectangle);

private:
	QRect		_rectangle;		/** Selection rectangle in screen space */
};