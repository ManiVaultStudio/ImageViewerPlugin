#pragma once

#include <memory>

#include <QWidget>

class ImageViewerPlugin;
class DatasetsModel;

namespace Ui {
	class SettingsWidget;
}

/** TODO */
class SettingsWidget : public QWidget
{
public: // Construction/destruction

	/** TODO */
	SettingsWidget(ImageViewerPlugin* imageViewerPlugin);

	/** Destructor */
	~SettingsWidget();

private:
	ImageViewerPlugin*						_imageViewerPlugin;		/** TODO */
	std::unique_ptr<Ui::SettingsWidget>		_ui;					/** TODO */
};