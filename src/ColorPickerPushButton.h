#pragma once

#include <QPushButton>

class QColorDialog;

/**
 * Color picker push button class
 *
 * A push button widget class for picking, storing and display a color
 *
 * @author Thomas Kroes
 */
class ColorPickerPushButton : public QPushButton
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * @param parent Pointer to parent widget
	 */
	ColorPickerPushButton(QWidget* parent);

	/** Returns the currently selected color */
	QColor currentColor() const;

	/**
	 * Sets the current color
	 * @param color Color
	 */
	void setCurrentColor(const QColor& color);

signals:

	/** Signals that the current color has changed
	 * @param currentColor Currently selected color
	 */
	void currentColorChanged(const QColor& currentColor);

private:
	QColorDialog*	_colorDialog;		/** Qt color picker dialog */
	
	static const QSize _iconSize;
};