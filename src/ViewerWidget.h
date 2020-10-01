#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class ViewerWidget;
}

class ImageViewerPlugin;
class CanvasWidget;
class StatusbarWidget;

/**
 * Viewer widget class
 *
 * This is a container class for the canvas widget (in which the images are drawn) and the status bar widget
 *
 * @author Thomas Kroes
 */
class ViewerWidget : public QWidget
{
public: // Construction

	/**
	 * Constructor
	 * @param parent Parent widget
	 */
	ViewerWidget(QWidget* parent);

public: // Hints

	/** Returns whether to show hints */
	bool getShowHints() const;

	/**
	 * Sets whether to show hints
	 * @param showHints Whether to show hints
	 */
	void setShowHints(const bool& showHints);

public: // Getters

	/** Get a pointer to the canvas widget */
	CanvasWidget* getCanvasWidget();

	/** Get a pointer to the statusbar widget */
	StatusbarWidget* getStatusbarWidget();

private:
	ImageViewerPlugin*					_imageViewerPlugin;		/** Pointer to the image viewer plugin */
	std::unique_ptr<Ui::ViewerWidget>	_ui;					/** User interface as produced by Qt designer */
};