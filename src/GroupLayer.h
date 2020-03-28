#pragma once

#include "Layer.h"

class ClustersDataset;

/** TODO */
class GroupLayer : public Layer
{
public:
	
	/** TODO */
	enum class Column {
	};

public:

	/** TODO */
	GroupLayer(const QString& id, const QString& name, const int& flags);

public: // Inherited MVC

	/** TODO */
	Qt::ItemFlags flags(const QModelIndex& index) const override;

	/** TODO */
	QVariant data(const QModelIndex& index, const int& role) const override;

	/** TODO */
	void setData(const QModelIndex& index, const QVariant& value, const int& role) override;

	/** TODO */
	template<typename LayerType, typename ...Args>
	void addLayer(Args... args)
	{
		try {
			appendChild(new LayerType(args...));
		}
		catch (const std::exception& e)
		{
			throw std::exception(QString("Unable to add layer").toLatin1());
		}
	}

private:
};