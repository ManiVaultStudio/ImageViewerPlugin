#pragma once

#include "Layer.h"

class PointsDataset;

/** TODO */
class PointsLayer : public _Layer
{
public:

	/** TODO */
	enum class Column {
	};

public:

	/** TODO */
	PointsLayer(PointsDataset* pointsDataset, const QString& id, const QString& name, const int& flags);

public: // Inherited MVC

	/** TODO */
	Qt::ItemFlags flags(const QModelIndex& index) const override;

	/** TODO */
	QVariant data(const QModelIndex& index, const int& role) const override;

	/** TODO */
	void setData(const QModelIndex& index, const QVariant& value, const int& role) override;

private:
	PointsDataset*	_points;	/** TODO */
};