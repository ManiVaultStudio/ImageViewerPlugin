#pragma once

#include "Layer.h"

class Dataset;

/** TODO */
class SelectionLayer : public Layer
{
public:

	/** TODO */
	enum class Column {
		ParamA = ult(Layer::Column::End) + 1,
		ParamB,

		Start = ParamA,
		End = ParamB
	};

public:

	/** TODO */
	SelectionLayer(Dataset* dataset, const QString& id, const QString& name, const int& flags);

public: // Inherited MVC

	/** TODO */
	int noColumns() const override;

	/** TODO */
	Qt::ItemFlags flags(const QModelIndex& index) const override;

	/** TODO */
	QVariant data(const QModelIndex& index, const int& role) const override;

	/** TODO */
	QModelIndexList setData(const QModelIndex& index, const QVariant& value, const int& role) override;

private:
	Dataset*	_dataset;	/** TODO */
};