#pragma once

#include "Common.h"

#include "Actor.h"

#include <QSharedPointer>

class QMouseEvent;

class Renderer;
class PolylineProp;

/**
 * Selection picker actor class
 * @author Thomas Kroes
 */
class RectangleSelectionActor : public Actor
{
	Q_OBJECT

public:
	RectangleSelectionActor(Renderer* renderer, const QString& name);

	/** Invoked when a mouse button is pressed */
	void onMousePressEvent(QMouseEvent* mouseEvent) override;

	/** Invoked when a mouse button is released */
	void onMouseReleaseEvent(QMouseEvent* mouseEvent) override;

	/** Invoked when the mouse pointer is moved */
	void onMouseMoveEvent(QMouseEvent* mouseEvent) override;

private:
	/** Initialize the actor */
	void initialize();

	/** Updates the selection geometry */
	void update();

	/** Returns a pointer to the selection rectangle prop */
	PolylineProp* rectangleProp();

signals:

private:
};