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
 * Selection picker actor class
 * @author Thomas Kroes
 */
class SelectionPickerActor : public Actor
{
	Q_OBJECT

public:
	SelectionPickerActor(Renderer* renderer, const QString& name);

	/** Returns the image size */
	QSize imageSize() const;

	/**
	 * Sets the image size
	 * @param imageSize Image size
	 */
	void setImageSize(const QSize& imageSize);

signals:
	/**
	 * Invoked when the image size changed
	 * @param imageSize Image size
	 */
	void imageSizeChanged(const QSizeF& imageSize);

private:
	QSize		_imageSize;		/** Image size */
};