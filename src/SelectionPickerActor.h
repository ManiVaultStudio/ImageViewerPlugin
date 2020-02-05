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
class PolylineProp;
class SelectionRectangleProp;
class SelectionBrushProp;

/**
 * Selection picker actor class
 * @author Thomas Kroes
 */
class SelectionPickerActor : public Actor
{
	Q_OBJECT

public:
	SelectionPickerActor(Renderer* renderer, const QString& name);

	/** Show the actor */
	void show() override;

	/** Hide the actor */
	void hide() override;

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

	/** Returns whether selection is taking place */
	bool isSelecting() const;

private:
	/** Initialize the actor */
	void initialize();

	/** Starts the selection process */
	void startSelection();

	/** Ends the selection process */
	void endSelection();

	/** Updates the selection geometry */
	void update();

	/** Returns a pointer to the selection rectangle prop */
	PolylineProp* rectangleProp();

	/** Returns a pointer to the selection brush prop */
	PolylineProp* brushProp();

	/** Returns a pointer to the lasso prop */
	PolylineProp* lassoProp();

	/** Returns a pointer to the polygon segments prop */
	PolylineProp* polygonSegmentsProp();

	/** Returns a pointer to the polygon closing segment prop */
	PolylineProp* polygonClosingSegmentProp();

	/** Updates the selection rectangle */
	void updateSelectionRectangle();

	/** Updates the selection brush */
	void updateSelectionBrush();

	/** Updates the selection lasso */
	void updateSelectionLasso();

	/** Updates the selection polygon */
	void updateSelectionPolygon();

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

	/** Signals that the pixel selection should include all pixels */
	void selectAll();

	/** Signals that the pixel selection should include no pixels */
	void selectNone();

	/** Signals that the pixel selection needs to be inverted */
	void selectInvert();

private:
	QSize					_imageSize;				/** Image size */
	SelectionType			_selectionType;			/** Type of selection e.g. rectangle, brush */
	SelectionModifier		_selectionModifier;		/** The selection modifier determines if and how new selections are combined with existing selections e.g. add, replace and remove */
	float					_brushRadius;			/** Brush radius */
	float					_brushRadiusDelta;		/** Selection brush size increase/decrease delta */
	bool					_selecting;				/** Whether selection is taking place */
};