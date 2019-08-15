#pragma once

#include <QStackedWidget.h>

class ImageViewerPlugin;

class QComboBox;
class QLabel;

class SettingsWidget : public QWidget
{
	Q_OBJECT
public:
	SettingsWidget(ImageViewerPlugin* imageViewerPlugin);

	void addDataSet(const QString& name);
	void removeDataSet(const QString& name);
	QString currentDataSetName() const;

	void onSelectedPointsChanged();

signals:
	void currentImageChanged(const QString& dataSetName, const int& imageIndex);

protected:
	void onCurrentDataSetChanged(const QString& name);
	void onCurrentImageIndexChanged(int index);

private:
	void updateImagesComboBox();

private:
	ImageViewerPlugin*	_imageViewerPlugin;
	QComboBox*			_dataSetsComboBox;
	QLabel*				_imagesLabel;
	QComboBox*			_imagesComboBox;
};
