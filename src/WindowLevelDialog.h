#pragma once

#include <memory>

#include <QDialog>

class ImageViewerPlugin;
class DatasetsModel;

namespace Ui {
	class WindowLevelDialog;
}

/**
 * TODO
 *
 * @author Thomas Kroes
 */
class WindowLevelDialog : public QDialog
{
public: // Construction/destruction

	/**
	 * Constructor
	 * @param imageViewerPlugin Pointer to the image viewer plugin
	 */
	WindowLevelDialog(ImageViewerPlugin* imageViewerPlugin);

	/** Destructor */
	~WindowLevelDialog();

private:
	ImageViewerPlugin*						_imageViewerPlugin;		/** Pointer to the image viewer plugin */
	std::unique_ptr<Ui::WindowLevelDialog>	_ui;					/** User interface as produced by Qt designer */
};