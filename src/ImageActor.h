#pragma once

#include "Common.h"

#include <QObject>
#include <QColor>
#include <QMatrix4x4>
#include <QMap>
#include <QSharedPointer>

#include "Actor.h"

class QMouseEvent;
class QWheelEvent;

class Renderer;

/**
 * Image actor class
 * @author Thomas Kroes
 */
class ImageActor : public Actor
{
	Q_OBJECT

public:
	ImageActor(Renderer* renderer, const QString& name);

	bool isInitialized() const;

	/**
	 * Sets the image
	 * @param image Image
	 */
	void setImage(std::shared_ptr<QImage> image);

	QSize imageSize();

	/** Returns the normalized display window */
	float windowNormalized();

	/** Returns the normalized display level */
	float levelNormalized();

	/** Invoked when a mouse button is pressed */
	void onMousePressEvent(QMouseEvent* mouseEvent) override;

	/** Invoked when a mouse button is released */
	void onMouseReleaseEvent(QMouseEvent* mouseEvent) override;

	/** Invoked when the mouse pointer is moved */
	void onMouseMoveEvent(QMouseEvent* mouseEvent) override;

private:
	QVector<QPoint>		_mousePositions;		/** Recorded mouse positions in screen coordinates */
};