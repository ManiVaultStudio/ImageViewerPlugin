#pragma once

#include <memory>

#include <QWidget>

class ImageViewerPlugin;
class DatasetsModel;

/** TODO */
class SettingsWidget : public QWidget
{
public: // Construction/destruction

	/** TODO */
	SettingsWidget(ImageViewerPlugin* imageViewerPlugin);

	/** Destructor */
	~SettingsWidget();
};