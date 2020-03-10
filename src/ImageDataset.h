#pragma once

#include "Layer.h"

#include <QStringList>
#include <QSharedPointer>

class LayersModel;

using Indices = QVector<std::uint32_t>;

Q_DECLARE_METATYPE(Indices);

/** TODO */
class ImageDataset : public QObject
{
public:
	/** TODO */
	ImageDataset(QObject* parent);

	/** TODO */
	void addLayer(const QString& name, const Layer::Type& type, const std::uint32_t& flags);

	/** TODO */
	QVariant currentImageName(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant imageNames(const int& role = Qt::DisplayRole) const;

	QString							_name;					/** TODO */
	int								_type;					/** TODO */
	std::uint32_t					_noImages;				/** TODO */
	QSize							_size;					/** TODO */
	std::uint32_t					_noPoints;				/** TODO */
	std::uint32_t					_noDimensions;			/** TODO */
	std::int32_t					_currentImage;			/** TODO */
	std::int32_t					_currentDimension;		/** TODO */
	QStringList						_imageNames;			/** TODO */
	QStringList						_dimensionNames;		/** TODO */
	bool							_averageImages;			/** TODO */
	QStringList						_imageFilePaths;		/** TODO */
	QString							_pointsName;			/** TODO */
	Indices							_selection;				/** TODO */
	Layers							_layers;				/** TODO */
	QSharedPointer<LayersModel>		_layersModel;			/** TODO */
};

using Datasets = QList<ImageDataset*>;