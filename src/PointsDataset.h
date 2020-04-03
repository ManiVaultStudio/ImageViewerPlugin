#pragma once

#include "Dataset.h"

#include <QModelIndex>

/** TODO */
class PointsDataset : public Dataset
{
public:
	/** TODO */
	PointsDataset(ImageViewerPlugin* imageViewerPlugin, const QString& name);

	/** TODO */
	void init() override;

public: // Getters/setters

	/** TODO */
	QVariant noPoints(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setNoPoints(const std::uint32_t& noPoints);

	/** TODO */
	QVariant noDimensions(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setNoDimensions(const std::uint32_t& noDimensions);

	/** TODO */
	QVariant selection(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setSelection(const Indices& selection);

	/** TODO */
	QVariant selectionSize(const int& role = Qt::DisplayRole) const;

private:
	std::uint32_t		_noPoints;				/** TODO */
	std::uint32_t		_noDimensions;			/** TODO */
	Indices				_selection;				/** TODO */
};