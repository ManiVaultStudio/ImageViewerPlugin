#pragma once

#include "Layer.h"

class ClustersDataset;

/** TODO */
class ClustersLayer : public _Layer
{
public:
	
	/** TODO */
	enum class Column {
	};

public:

	/** TODO */
	ClustersLayer(ClustersDataset* clustersDataset, const QString& id, const QString& name, const int& flags);

public: // Inherited MVC

	/** TODO */
	Qt::ItemFlags flags(const QModelIndex& index) const override;

	/** TODO */
	QVariant data(const QModelIndex& index, const int& role) const override;

	/** TODO */
	void setData(const QModelIndex& index, const QVariant& value, const int& role) override;

private:
	ClustersDataset*	_clusters;		/** TODO */
};