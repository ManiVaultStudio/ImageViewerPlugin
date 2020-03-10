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
	QVariant currentImage(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant currentImageName(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setCurrentImage(const std::uint32_t& currentImage);

	/** TODO */
	QVariant currentDimension(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant currentDimensionName(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setCurrentDimension(const std::uint32_t& currentDimension);

	/** TODO */
	QVariant imageNames(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setImageNames(const QStringList& imageNames);

	/** TODO */
	QVariant dimensionNames(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setDimensionNames(const QStringList& dimensionNames);

	/** TODO */
	QVariant average(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setAverage(const bool& average);

	/** TODO */
	QVariant imageFilePaths(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setImageFilePaths(const QStringList& imageFilePaths);

	/** TODO */
	QVariant currentImageFilePath(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant currentDimensionFilePath(const int& role = Qt::DisplayRole) const;
	
	/** TODO */
	QVariant pointsName(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setPointsName(const QString& pointsName);

	/** TODO */
	QVariant selection(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setSelection(const Indices& selection);

	/** TODO */
	QVariant selectionSize(const int& role = Qt::DisplayRole) const;

	QString							_name;					/** TODO */
	int								_type;					/** TODO */
	QSize							_size;					/** TODO */
	std::uint32_t					_noPoints;				/** TODO */
	std::uint32_t					_noDimensions;			/** TODO */
	std::int32_t					_currentImage;			/** TODO */
	std::int32_t					_currentDimension;		/** TODO */
	QStringList						_imageNames;			/** TODO */
	QStringList						_dimensionNames;		/** TODO */
	bool							_average;				/** TODO */
	QStringList						_imageFilePaths;		/** TODO */
	QString							_pointsName;			/** TODO */
	Indices							_selection;				/** TODO */
	Layers							_layers;				/** TODO */
	QSharedPointer<LayersModel>		_layersModel;			/** TODO */
};

using Datasets = QList<ImageDataset*>;