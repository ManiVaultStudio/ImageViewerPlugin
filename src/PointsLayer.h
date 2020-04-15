#pragma once

#include "LayerNode.h"

#include "PointData.h"

/** TODO */
class PointsLayer : public LayerNode
{
public:

	/** TODO */
	enum class Column {
		Size = ult(LayerNode::Column::End) + 1,
		Channel1,
		Channel2,
		Channel3,
		NoChannels,
		DimensionNames,
		NoPoints,
		NoDimensions,

		Start = Size,
		End = NoDimensions
	};

	/** TODO */
	struct Channel {
		Channel() :
			_enabled(false),
			_dimension(0)
		{
		}

		bool	_enabled;		/** TODO */
		int		_dimension;		/** TODO */
	};

public:

	/** TODO */
	PointsLayer(const QString& dataset, const QString& id, const QString& name, const int& flags);

	/** Initializes the layer */
	void init();

public: // Inherited MVC

	/** TODO */
	int columnCount() const override { return ult(Column::End) + 1; }

	/** TODO */
	Qt::ItemFlags flags(const QModelIndex& index) const override;

	/** TODO */
	QVariant data(const QModelIndex& index, const int& role) const override;

	/** TODO */
	QModelIndexList setData(const QModelIndex& index, const QVariant& value, const int& role) override;

public: // Getters/setters

	/** TODO */
	QVariant noPoints(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setNoPoints(const std::uint32_t& noPoints);

	/** TODO */
	QVariant noDimensions(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setNoDimensions(const std::uint32_t& noDimensions);

	/** TODO */
	QVariant dimensionNames(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setDimensionNames(const QStringList& dimensionNames);

	/** TODO */
	QVariant channel(const int& channel, const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setChannel(const int& channel, const int& dimension);

	/** TODO */
	QVariant noChannels(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setNoChannels(const int& noChannels);

private:
	Points*				_points;			/** TODO */
	std::uint32_t		_noPoints;			/** TODO */
	std::uint32_t		_noDimensions;		/** TODO */
	QStringList			_dimensionNames;	/** TODO */
	int					_channels[3];		/** TODO */
	int					_noChannels;		/** TODO */
};