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

signals:
	/** Signals that the number of datasets changed */
	void noDatasetsChanged();

	/**
	 * Signals the current dataset name changed
	 * @param currentDatasetName Current dataset name
	 */
	void currentDatasetNameChanged(const QString& currentDatasetName);

private:
	QMap<QString, QSharedPointer<Dataset>>		_datasets;
	QString										_currentDatasetName;
};