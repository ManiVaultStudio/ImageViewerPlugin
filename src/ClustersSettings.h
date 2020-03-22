#pragma once

#include "Settings.h"
#include "ClustersDataset.h"

/** TODO */
class ClustersSettings : public Settings<ClustersDataset>
{
public:

	/** TODO */
	ClustersSettings(Dataset* dataset);

public: // TODO

	/** TODO */
	Qt::ItemFlags itemFlags(const LayerColumn& column) const override;

	/** TODO */
	QVariant data(const LayerColumn& column, int role) const override;

	/** TODO */
	void setData(const LayerColumn& column, const QVariant& value, const int& role) override;

private:
};