#pragma once

#include <QStackedWidget.h>

class ImageViewerPlugin;

class QComboBox;
class QCheckBox;
class QLabel;

class SettingsWidget : public QWidget
{
	Q_OBJECT

public:
	SettingsWidget(ImageViewerPlugin* imageViewerPlugin);

	void addDataSet(const QString& name);
	void removeDataSet(const QString& name);

public:
	void onSelectedPointsChanged();

public:
	void updateImagesComboBox();

protected:
	void onCurrentDataSetNameChanged(const QString& name);
	void onAverageImagesChanged(const bool& averageImages);

private:
	void update();

private:
	ImageViewerPlugin*	_imageViewerPlugin;
	QComboBox*			_dataSetsComboBox;
	QLabel*				_imagesLabel;
	QComboBox*			_imagesComboBox;
	QCheckBox*			_imagesAverageCheckBox;
	QCheckBox*			_zoomToSelection;
};
