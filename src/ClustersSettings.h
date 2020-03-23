#pragma once

#include "Settings.h"
#include "ClustersDataset.h"

/** TODO */
class ClustersSettings : public Settings<ClustersDataset>
{
public:

	/** TODO */
	ClustersSettings(QObject* parent, Dataset* dataset);

public: // TODO

	/** TODO */
	Qt::ItemFlags itemFlags(const Layer::Column& column) const override;

	/** TODO */
	QVariant data(const Layer::Column& column, int role) const override;

	/** TODO */
	void setData(const Layer::Column& column, const QVariant& value, const int& role) override;

private:
};