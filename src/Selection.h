#pragma once

#include "Common.h"

#include "ImageData/ImageData.h"

class Renderer;

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

/**
 * Selection class
 * @author Thomas Kroes
 */
class Selection : public QObject
{
	Q_OBJECT

public:
	/** Constructor */
	Selection(Renderer* renderer);

	/** Destructor */
	~Selection();

public:
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

	/** Aborts the selection process */
	void abortSelection();

	/**
	 * Set selected indices
	 * @param indices Selected indices
	 */
	void setIndices(const QVector<std::uint32_t>& indices);

	/** Select all pixels */
	void selectAll();

	/** Clear the selection */
	void selectNone();

	/** Invert the selection */
	void selectInvert();

private: // Event handlers

	/**
	 * Invoked when a key is pressed
	 * @param keyEvent Key event
	 */
	void onKeyPress(QKeyEvent* keyEvent);

	/**
	 * Invoked when a key is released
	 * @param keyEvent Key event
	 */
	void onKeyRelease(QKeyEvent* keyEvent);

	/**
	 * Invoked when the mouse button is pressed
	 * @param mouseEvent Mouse event
	 */
	void onMousePress(QMouseEvent* mouseEvent);

	/**
	 * Invoked when the mouse button is released
	 * @param mouseEvent Mouse event
	 */
	void onMouseRelease(QMouseEvent* mouseEvent);

	/**
	 * Invoked when the mouse pointer is moved
	 * @param mouseEvent Mouse event
	 */
	void onMouseMove(QMouseEvent* mouseEvent);

	/**
	 * Invoked when the mouse wheel is rotated
	 * @param wheelEvent Mouse wheel event
	 */
	void onMouseWheel(QWheelEvent* wheelEvent);

signals:
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
	 * Signals the selection rectangle changed
	 * @param rectangle Rectangle in screen coordinates
	 */
	void selectionRectangleChanged(const QRectF& rectangle);

	/** Signals the selection process aborted */
	void selectionAborted();

	/**
	 * Signals that the selection indices changed
	 * @param indices Selected indices
	 */
	void indicesChanged(const QVector<std::uint32_t>& indices);

protected:
	Renderer*				_renderer;				/** Parent renderer */
	SelectionType			_selectionType;			/** Type of selection e.g. rectangle, brush */
	SelectionModifier		_selectionModifier;		/** The selection modifier determines if and how new selections are combined with existing selections e.g. add, replace and remove */
	QVector<std::uint32_t>	_indices;				/** Selected indices */
};