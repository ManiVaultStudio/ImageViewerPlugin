#pragma once

#include "Common.h"

#include "Dataset.h"

#include <QVariant>

//class Dataset;

/** TODO */
template<typename DatasetType>
class Settings
{
public:

	/** TODO */
	Settings(Dataset* dataset) :
		_dataset(reinterpret_cast<DatasetType*>(dataset))
	{
	}

public: // MVC

	/** TODO */
	virtual Qt::ItemFlags itemFlags(const LayerColumn& column) const = 0;

	/** TODO */
	virtual QVariant data(const LayerColumn& column, int role) const = 0;

	/** TODO */
	virtual void setData(const LayerColumn& column, const QVariant& value, const int& role) = 0;

protected:
	DatasetType*	_dataset;		/** TODO */
};