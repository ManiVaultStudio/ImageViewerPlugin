#pragma once

#include "Layer.h"
#include "LayersModel.h"

#include <QStringList>

/** TODO */
struct ImageDataset
{
	/** TODO */
	ImageDataset();

	QString				_name;				/** TODO */
	int					_type;				/** TODO */
	std::uint32_t		_noImages;			/** TODO */
	QSize				_size;				/** TODO */
	std::uint32_t		_noPoints;			/** TODO */
	std::uint32_t		_noDimensions;		/** TODO */
	std::uint32_t		_currentImage;		/** TODO */
	std::uint32_t		_currentDimension;	/** TODO */
	QStringList			_imageNames;		/** TODO */
	QStringList			_dimensionNames;	/** TODO */
	bool				_averageImages;		/** TODO */
	QStringList			_imageFilePaths;	/** TODO */
	QList<Layer>		_layers;			/** TODO */
	LayersModel			_layersModel;		/** TODO */
};

using Datasets = QList<ImageDataset>;