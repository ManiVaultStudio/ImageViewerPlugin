#pragma once

#include "Layer.h"

#include <QStringList>
#include <QSharedPointer>

class LayersModel;

/** TODO */
class ImageDataset : public QObject
{
public:
	/** TODO */
	ImageDataset(QObject* parent);

	/** TODO */
	void addLayer(const QString& name, const Layer::Type& type, const std::uint32_t& flags);

	QString							_name;					/** TODO */
	int								_type;					/** TODO */
	std::uint32_t					_noImages;				/** TODO */
	QSize							_size;					/** TODO */
	std::uint32_t					_noPoints;				/** TODO */
	std::uint32_t					_noDimensions;			/** TODO */
	std::uint32_t					_currentImage;			/** TODO */
	std::uint32_t					_currentDimension;		/** TODO */
	QStringList						_imageNames;			/** TODO */
	QStringList						_dimensionNames;		/** TODO */
	bool							_averageImages;			/** TODO */
	QStringList						_imageFilePaths;		/** TODO */
	Layers							_layers;				/** TODO */
	QSharedPointer<LayersModel>		_layersModel;			/** TODO */
};

using Datasets = QList<ImageDataset*>;