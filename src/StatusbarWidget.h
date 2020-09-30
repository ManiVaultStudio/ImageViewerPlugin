#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class StatusbarWidget;
}

class ImageViewerPlugin;

/**
 * TODO: Add description
 *
 * @author Thomas Kroes
 */
class StatusbarWidget : public QWidget
{
public: // Construction

	/**
	 * Constructor
	 * @param parent Parent widget
	 */
	StatusbarWidget(QWidget* parent);

public: // Initialization and update

	/**
	 * Initializes the widget
	 * @param imageViewerPlugin Pointer to the image viewer plugin
	 */
	void initialize(ImageViewerPlugin* imageViewerPlugin);

private:
	std::unique_ptr<Ui::StatusbarWidget> _ui;		/** User interface as produced by Qt designer */
};