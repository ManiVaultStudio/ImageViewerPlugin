#pragma once

#include "Layer.h"
#include "ClustersDataset.h"

/** TODO */
class ClustersLayer : public Layer
{
public:

	/** TODO */
	ClustersLayer(Dataset* dataset, const QString& id, const QString& name, const std::uint32_t& flags);

public: // TODO

	/** TODO */
	Qt::ItemFlags itemFlags(const int& column) const override;

	/** TODO */
	QVariant data(const int& column, int role) const override;

	/** TODO */
	void setData(const int& column, const QVariant& value, const int& role) override;

private:
};