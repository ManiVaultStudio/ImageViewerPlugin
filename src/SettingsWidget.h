#pragma once

#include <memory>

#include <QStackedWidget.h>

class ImageViewerPlugin;

class QComboBox;
class QCheckBox;
class QLabel;

namespace Ui {
	class SettingsWidget;
}

class SettingsWidget : public QWidget
{
	Q_OBJECT

public:
	SettingsWidget(ImageViewerPlugin* imageViewerPlugin);

	void addDataSet(const QString& name);
	void removeDataSet(const QString& name);

public:
	void onSelectedPointsChanged();

protected:
	void onAverageImagesChanged(const bool& averageImages);

private:
	void update();

private:
	ImageViewerPlugin*					_imageViewerPlugin;
	std::unique_ptr<Ui::SettingsWidget>	_ui;
};
