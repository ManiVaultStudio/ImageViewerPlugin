#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class ColorWidget;
}

/**
 * Color widget class
 *
 * A widget for picking color
 * Uses the QT color picker dialog
 *
 * @author Thomas Kroes
 */
class ColorWidget : public QWidget
{
public:
	/** Constructor */
	ColorWidget(QWidget* parent);

private:

	/** Updates the button UI */
	void update();

	/** Updates the button icon */
	void updateIcon();

	/** Updates the button text */
	void updateText();

signals:

	/** Signals that the color map has been picked
	 * @param color Picked color
	 */
	void colorPicked(const QColor& color);

private:
	std::unique_ptr<Ui::ColorWidget>	_ui;			/** Color widget UI */
	QColor								_color;			/** Last picked color */
	QSize								_iconSize;		/** Size of the icon */
};