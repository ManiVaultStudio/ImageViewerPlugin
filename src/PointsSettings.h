#pragma once

#include "Settings.h"
#include "PointsDataset.h"

/** TODO */
class PointsSettings : public Settings<PointsDataset>
{
public:

	/** TODO */
	PointsSettings(Dataset* dataset);

public: // TODO

	/** TODO */
	Qt::ItemFlags itemFlags(const LayerColumn& column) const override;

	/** TODO */
	QVariant data(const LayerColumn& column, int role) const override;

	/** TODO */
	void setData(const LayerColumn& column, const QVariant& value, const int& role) override;

private:
};