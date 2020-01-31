#pragma once

#include "Common.h"

#include "ImageData/ImageData.h"

#include <QObject>
#include <QColor>
#include <QMatrix4x4>
#include <QMap>
#include <QSharedPointer>

#include "Actor.h"

class QMouseEvent;
class QWheelEvent;
class QMenu;

class Renderer;
class Polyline2D;

/**
 * Selection picker actor class
 * @author Thomas Kroes
 */
class SelectionPickerActor : public Actor
{
	Q_OBJECT

public:
	SelectionPickerActor(Renderer* renderer, const QString& name);

	/** Initialize the Actor (must be called in appropriate OpenGL context) */
	virtual void initialize();

	/** Returns the image size */
	QSize imageSize() const;

	/**
	 * Sets the image size
	 * @param imageSize Image size
	 */
	void setImageSize(const QSize& imageSize);

	/** Returns the selection type */
	SelectionType selectionType() const;

	/**
	 * Sets the selection type
	 * @param selectionType Selection type
	 */
	void setSelectionType(const SelectionType& selectionType);

	/** Returns the selection modifier */
	SelectionModifier selectionModifier() const;

	/**
	 * Sets the selection modifier
	 * @param selectionModifier Selection modifier
	 */
	void setSelectionModifier(const SelectionModifier& selectionModifier);

	/** Returns the brush radius */
	float brushRadius() const;

	/**
	 * Sets the brush radius
	 * @param brushRadius Brush radius
	 */
	void setBrushRadius(const float& brushRadius);

	/** Returns the brush radius delta (amount to increasing/decreasing) */
	float brushRadiusDelta() const;

	/**
	 * Sets the brush radius delta (amount to increasing/decreasing)
	 * @param brushRadiusDelta Amount to add/remove
	 */
	void setBrushRadiusDelta(const float& brushRadiusDelta);

	/** Increase the brush size by _brushRadiusDelta */
	void brushSizeIncrease();

	/** Decrease the brush size by _brushRadiusDelta */
	void brushSizeDecrease();

	/** Invoked when a mouse button is pressed */
	void onMousePressEvent(QMouseEvent* mouseEvent) override;

	/** Invoked when a mouse button is released */
	void onMouseReleaseEvent(QMouseEvent* mouseEvent) override;

	/** Invoked when the mouse pointer is moved */
	void onMouseMoveEvent(QMouseEvent* mouseEvent) override;

	/** Invoked when the mouse wheel is rotated */
	void onMouseWheelEvent(QWheelEvent* wheelEvent) override;

	/** Invoked when a key is pressed */
	void onKeyPressEvent(QKeyEvent* keyEvent) override;

	/** Invoked when a key is release */
	void onKeyReleaseEvent(QKeyEvent* keyEvent) override;

	/** Returns the context menu */
	QMenu* contextMenu();

protected:
	Polyline2D* rectangleShape();
	Polyline2D* brushShape();
	Polyline2D* lassoShape();
	Polyline2D* polygonShape();

private:
	/** Starts the selection process */
	void startSelection();

	/** Ends the selection process */
	void endSelection();

	/**
	 * TODO
	 */
	void addMousePosition(const QPoint& point);

	/**
	 * TODO
	 */
	void updateRectangle();

	/**
	 * TODO
	 */
	void updateBrush();

	/**
	 * TODO
	 */
	void updateLasso();

	/**
	 * TODO
	 */
	void updatePolygon();

signals:
	/**
	 * Invoked when the image size changed
	 * @param imageSize Image size
	 */
	void imageSizeChanged(const QSizeF& imageSize);

	/**
	 * Signals the selection type changed
	 * @param selectionType Selection type
	 */
	void selectionTypeChanged(const SelectionType& selectionType);

	/**
	 * Signals the selection modifier changed
	 * @param selectionModifier Selection modifier
	 */
	void selectionModifierChanged(const SelectionModifier& selectionModifier);

	/**
	 * Invoked when the brush radius changed
	 * @param brushRadius Brush radius
	 */
	void brushRadiusChanged(const float& brushRadius);

	/**
	 * Signals the brush radius delta changed
	 * @param brushRadiusDelta Brush radius delta
	 */
	void brushRadiusDeltaChanged(const float& brushRadiusDelta);

	/** Signals that all pixels need to be selected */
	void selectAll();

	/** Signals that no pixels need to be selected */
	void selectNone();

	/** Signals that pixel selection needs to be inverted */
	void invertSelection();

private:
	QSize					_imageSize;				/** Image size */
	SelectionType			_selectionType;			/** Type of selection e.g. rectangle, brush */
	SelectionModifier		_selectionModifier;		/** The selection modifier determines if and how new selections are combined with existing selections e.g. add, replace and remove */
	float					_brushRadius;			/** Brush radius */
	float					_brushRadiusDelta;		/** Selection brush size increase/decrease delta */
	QVector<QPoint>			_mousePositions;		/** Recorded mouse positions in screen coordinates */
	QVector<QVector3D>		_positions;				/** Recorded mouse positions in world coordinates */
	float					_outlineLineWidth;		/** Line width of the outline geometry */

};