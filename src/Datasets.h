#pragma once

#include "ImageData/Images.h"

#include "Dataset.h"

/**
 * Datasets class
 * @author Thomas Kroes
 */
class Datasets : public QObject
{
	Q_OBJECT

public:
	/** Constructor */
	Datasets();

	/** Destructor */
	~Datasets();

	/** Add a dataset by name */
	void add(const QString& name, const QSharedPointer<Dataset>& dataset);

	/**
	 * Set the current (selected) dataset name
	 * @param currentDatasetName Current dataset name
	 */
	void setCurrentDatasetName(const QString& currentDatasetName);

	/**
	 * Returns the current (selected) dataset
	 * @param currentDatasetName Current dataset name
	 */
	Dataset* currentDataset();

	/**
	 * Returns dataset by name
	 * @param datasetName Dataset name
	 */
	Dataset* datasetByName(const QString& datasetName);

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
	void currentDatasetChanged(Dataset* previousDataset, Dataset* currentDataset);

private:
	QMap<QString, QSharedPointer<Dataset>>		_datasets;
	QString										_currentDatasetName;
};