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
 * Selection overlay actor class
 * @author Thomas Kroes
 */
class SelectionOverlayActor : public Actor
{
	Q_OBJECT

public:
	SelectionOverlayActor(Renderer* renderer, const QString& name);

	bool isInitialized() const;

	/**
	 * Sets the image
	 * @param image Image
	 */
	void setImage(std::shared_ptr<QImage> image);

	/** Returns the color image size */
	QSize imageSize() const;
};