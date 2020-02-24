#pragma once

#include "ImageData/Images.h"

#include "ImageDataset.h"

/**
 * Datasets class
 * @author Thomas Kroes
 */
class ImageDatasets : public QObject
{
	Q_OBJECT

public:
	/** Constructor */
	ImageDatasets();

	/** Destructor */
	~ImageDatasets();

	/** Add a dataset by name */
	void add(const QString& name, const QSharedPointer<ImageDataset>& dataset);

	/**
	 * Set the current (selected) dataset name
	 * @param currentDatasetName Current dataset name
	 */
	void setCurrentDatasetName(const QString& currentDatasetName);

	/**
	 * Returns the current (selected) dataset
	 * @param currentDatasetName Current dataset name
	 */
	ImageDataset* currentDataset();

	/**
	 * Returns dataset by name
	 * @param datasetName Dataset name
	 */
	ImageDataset* datasetByName(const QString& datasetName);

signals:
	/** Signals that dataset names changed */
	void namesChanged(const QStringList& names);

	/**
	 * Signals the current dataset name changed
	 * @param previousDatasetName Previous dataset name (if any)
	 * @param currentDatasetName Current dataset name
	 */
	void currentDatasetNameChanged(const QString& previousDatasetName, const QString& currentDatasetName);

	/**
	 * Signals the current dataset changed
	 * @param previousDataset Previous dataset (if any)
	 * @param currentDataset Current dataset
	 */
	void currentDatasetChanged(ImageDataset* previousDataset, ImageDataset* currentDataset);

private:
	QMap<QString, QSharedPointer<ImageDataset>>		_datasets;
	QString											_currentDatasetName;
};