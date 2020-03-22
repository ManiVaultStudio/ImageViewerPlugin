#pragma once

#include "Dataset.h"

#include <QModelIndex>

/** TODO */
class ClustersDataset : public Dataset
{
public:
	/** TODO */
	ClustersDataset(ImageViewerPlugin* imageViewerPlugin, const QString& name);

	/** TODO */
	void init() override;

private:
};