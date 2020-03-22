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

private:
};