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

public: // Inherited model members

	/** TODO */
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;

	/** TODO */
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	/** TODO */
	Qt::ItemFlags flags(const QModelIndex &index) const override;

	/** TODO */
	QVariant data(const int& row, const int& column, int role) const override;

	/** TODO */
	void setData(const int& row, const int& column, const QVariant& value) override;

private:
};