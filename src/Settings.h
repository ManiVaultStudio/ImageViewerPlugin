#pragma once

#include "Common.h"
#include "Layer.h"

#include <QVariant>

class Dataset;

/** TODO */
template<typename DatasetType>
class Settings : public QObject
{
public:

	/** TODO */
	Settings(QObject* parent, Dataset* dataset) :
		QObject(parent),
		_dataset(reinterpret_cast<DatasetType*>(dataset))
	{
	}

public: // TODO

	/** TODO */
	virtual Qt::ItemFlags itemFlags(const Layer::Column& column) const = 0;

	/** TODO */
	virtual QVariant data(const Layer::Column& column, int role) const = 0;

	/** TODO */
	virtual void setData(const Layer::Column& column, const QVariant& value, const int& role) = 0;

protected:
	DatasetType*	_dataset;		/** TODO */
};