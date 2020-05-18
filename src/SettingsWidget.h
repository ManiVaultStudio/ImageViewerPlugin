#pragma once

#include <memory>

#include <QWidget>

class ImageViewerPlugin;
class DatasetsModel;

namespace Ui {
	class SettingsWidget;
}

/**
 * Settings widget class
 *
 * This widget class provides the user interface for editing settings
 *
 * @author Thomas Kroes
 */
class SettingsWidget : public QWidget
{
public: // Construction/destruction

	/**
	 * Constructor
	 * @param imageViewerPlugin Pointer to the image viewer plugin
	 */
	SettingsWidget(ImageViewerPlugin* imageViewerPlugin);

	/** Destructor */
	~SettingsWidget();

private:
	ImageViewerPlugin*						_imageViewerPlugin;		/** Pointer to the image viewer plugin */
	std::unique_ptr<Ui::SettingsWidget>		_ui;					/** User interface as produced by Qt designer */
};