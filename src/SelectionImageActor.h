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
 * Selection image actor class
 * @author Thomas Kroes
 */
class SelectionImageActor : public Actor
{
	Q_OBJECT

public:
	SelectionImageActor(Renderer* renderer, const QString& name);

	/**
	 * Sets the image
	 * @param image Image
	 */
	void setImage(std::shared_ptr<QImage> image);

	/** Returns the image size */
	QSize imageSize() const;

signals:
	/**
	 * Invoked when the image size changed
	 * @param imageSize Image size
	 */
	void imageSizeChanged(const QSizeF& imageSize);
};