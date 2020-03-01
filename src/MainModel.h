#pragma once

#include "ImageDataset.h"

class ImageViewerPlugin;
class DatasetsModel;

/**
 * Datasets class
 * @author Thomas Kroes
 */
class MainModel : public QObject
{

public: // Construction/destruction

	/** Constructor */
	MainModel(ImageViewerPlugin* imageViewerPlugin);

	/** Destructor */
	~MainModel();

public: //

	/** TODO */
	void addDataset(const ImageDataset& dataset);

	/** TODO */
	Datasets* datasets() { return &_datasets; };

	/** TODO */
	DatasetsModel* datasetsModel() { return _datasetsModel; }

private:
	Datasets			_datasets;				/** TODO */
	std::uint32_t		_currentDatasetID;		/** TODO */
	DatasetsModel*		_datasetsModel;			/** TODO */
};