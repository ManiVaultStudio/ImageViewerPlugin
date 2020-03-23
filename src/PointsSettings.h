#pragma once

#include "Settings.h"
#include "PointsDataset.h"

/** TODO */
class PointsSettings : public Settings<PointsDataset>
{
public:

	/** TODO */
	PointsSettings(QObject* parent, Dataset* dataset);

public: // TODO

	/** TODO */
	Qt::ItemFlags itemFlags(const Layer::Column& column) const override;

	/** TODO */
	QVariant data(const Layer::Column& column, int role) const override;

	/** TODO */
	void setData(const Layer::Column& column, const QVariant& value, const int& role) override;

private:
};