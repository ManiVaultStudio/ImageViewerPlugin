#pragma once

#include "Dataset.h"

#include <QStringList>
#include <QSharedPointer>

class LayersModel;

using Indices = QVector<std::uint32_t>;

Q_DECLARE_METATYPE(Indices);

/** TODO */
class ImagesDataset : public Dataset
{
public:
	/** TODO */
	ImagesDataset(QObject* parent, const QString& name);

	/** TODO */
	QVariant noImages(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant width(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant height(const int& role = Qt::DisplayRole) const;

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
	QVariant imageNames(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant filteredImageNames(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant imageIds(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setImageNames(const QStringList& imageNames);

	/** TODO */
	QVariant imageFilePaths(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setImageFilePaths(const QStringList& imageFilePaths);

	/** TODO */
	QVariant currentImage(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setCurrentImage(const std::uint32_t& currentImage);

	/** TODO */
	QVariant currentImageName(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant currentImageFilePath(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant average(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setAverage(const bool& average);

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

	static QString displayStringList(const QStringList& stringList);

private:
	QSize				_size;					/** TODO */
	std::uint32_t		_noPoints;				/** TODO */
	std::uint32_t		_noDimensions;			/** TODO */
	QStringList			_imageNames;			/** TODO */
	QStringList			_imageFilePaths;		/** TODO */
	std::int32_t		_currentImage;			/** TODO */
	bool				_average;				/** TODO */
	QString				_pointsName;			/** TODO */
	Indices				_selection;				/** TODO */
};