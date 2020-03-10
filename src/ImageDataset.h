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
	QVariant name(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setName(const QString& name);

	/** TODO */
	QVariant type(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setType(const int& type);

	/** TODO */
	QVariant noImages(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant size(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setSize(const QSize& size);

	/** TODO */
	QVariant noPoints(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setNoPoints(const std::uint32_t& noPoints);

	/** TODO */
	QVariant noDimensions(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setNoDimensions(const std::uint32_t& noDimensions);

	/** TODO */
	QVariant currentImageName(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant imageNames(const int& role = Qt::DisplayRole) const;

	QString							_name;					/** TODO */
	int								_type;					/** TODO */
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