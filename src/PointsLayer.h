#pragma once

#include "Layer.h"

class PointsDataset;

/** TODO */
class PointsLayer : public Layer
{
public:

	/** TODO */
	enum class Column {
		Size = ult(Layer::Column::End) + 1,
		Width,
		Height,
		Square,
		Channel1,
		Channel2,
		Channel3,
		NoChannels,
		NoPoints,
		NoDimensions,
		Selection,
		SelectionSize,

		Start = Size,
		End = SelectionSize
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
	PointsLayer(PointsDataset* pointsDataset, const QString& id, const QString& name, const int& flags);

public: // Inherited MVC

	/** TODO */
	int noColumns() const override;

	/** TODO */
	Qt::ItemFlags flags(const QModelIndex& index) const override;

	/** TODO */
	QVariant data(const QModelIndex& index, const int& role) const override;

	/** TODO */
	QModelIndexList setData(const QModelIndex& index, const QVariant& value, const int& role) override;

public: // Getters/setters

	/** TODO */
	QVariant size(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setSize(const QSize& size);

	/** TODO */
	QVariant width(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setWidth(const int& width);

	/** TODO */
	QVariant height(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setHeight(const int& height);

	/** TODO */
	QVariant square(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setSquare(const bool& square);

	/** TODO */
	QVariant channel(const int& channel, const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setChannel(const int& channel, const int& dimension);

	/** TODO */
	QVariant noChannels(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setNoChannels(const int& noChannels);

private:
	PointsDataset*		_points;			/** TODO */
	int					_channels[3];		/** TODO */
	int					_noChannels;		/** TODO */
	QSize				_size;				/** TODO */
	bool				_square;			/** TODO */
};