#pragma once

#include "Settings.h"
#include "Dataset.h"

/** TODO */
class Range
{
public:

	/** TODO */
	Range(const float& min = 0.0f, const float& max = 0.0f) :
		_min(std::min(min, max)),
		_max(std::max(min, max))
	{
	}

	/** TODO */
	float min() const { return _min; }

	/** TODO */
	void setMin(const float& min) { _min = std::min(min, _max); }

	/** TODO */
	float max() const { return _max; }

	/** TODO */
	void setMax(const float& max) { _max = std::max(_min, max); }

	/** TODO */
	void include(const float& value) {
		_min = std::min(_min, value);
		_max = std::max(_max, value);
	}

	/** TODO */
	void setFullRange() {
		_max = std::numeric_limits<float>::min();
		_min = std::numeric_limits<float>::max();
	}

	/** TODO */
	float length() const { return _max - _min; }

private:
	float	_min;	/** TODO */
	float	_max;	/** TODO */
};

Q_DECLARE_METATYPE(Range);

/** TODO */
class GeneralSettings : public Settings<Dataset>
{
public:

	/** TODO */
	GeneralSettings(Dataset* dataset, const QString& id, const QString& name, const LayerType& type, const std::uint32_t& flags);

public: // TODO

	/** TODO */
	Qt::ItemFlags itemFlags(const LayerColumn& column) const override;

	/** TODO */
	QVariant data(const LayerColumn& column, int role) const override;

	/** TODO */
	void setData(const LayerColumn& column, const QVariant& value, const int& role) override;

public: // Getters/setters

/** TODO */
	QVariant id(const int& role) const;

	/** TODO */
	void setId(const QString& id);

	/** TODO */
	QVariant name(const int& role) const;

	/** TODO */
	void setName(const QString& name);

	/** TODO */
	QVariant dataset(const int& role) const;

	/** TODO */
	QVariant type(const int& role) const;

	/** TODO */
	void setType(const LayerType& type);

	/** TODO */
	QVariant flags(const int& role) const;

	/** TODO */
	QVariant flag(const LayerFlag& flag, const int& role) const;

	/** TODO */
	void setFlag(const LayerFlag& flag, const bool& enabled = true);

	/** TODO */
	void setFlags(const std::uint32_t& flags);

	/** TODO */
	QVariant order(const int& role) const;

	/** TODO */
	void setOrder(const std::uint32_t& order);

	/** TODO */
	QVariant opacity(const int& role) const;

	/** TODO */
	void setOpacity(const float& opacity);

	/** TODO */
	QVariant colorMap(const int& role) const;

	/** TODO */
	void setColorMap(const QImage& colorMap);

public: // Image functions

	/** TODO */
	QVariant image(const int& role) const;

	/** TODO */
	void setImage(const QImage& image);

	/** TODO */
	QVariant imageRange(const int& role) const;

	/** TODO */
	QVariant displayRange(const int& role) const;

	/** TODO */
	QVariant windowNormalized(const int& role) const;

	/** TODO */
	void setWindowNormalized(const float& windowNormalized);

	/** TODO */
	QVariant levelNormalized(const int& role) const;

	/** TODO */
	void setLevelNormalized(const float& levelNormalized);

	/** TODO */
	QVariant window(const int& role) const;

	/** TODO */
	void setWindow(const float& window);

	/** TODO */
	QVariant level(const int& role) const;

	/** TODO */
	void setLevel(const float& level);

protected:

	/** TODO */
	void computeImageRange();

	/** TODO */
	void computeDisplayRange();

private:
	QString				_id;					/** TODO */
	QString				_name;					/** TODO */
	LayerType			_type;					/** TODO */
	std::uint32_t		_flags;					/** TODO */
	std::uint32_t		_order;					/** TODO */
	float				_opacity;				/** TODO */
	QImage				_colorMap;				/** TODO */
	QImage				_image;					/** TODO */
	Range				_imageRange;			/** TODO */
	Range				_displayRange;			/** TODO */
	float				_windowNormalized;		/** TODO */
	float				_levelNormalized;		/** TODO */
	float				_window;				/** TODO */
	float				_level;					/** TODO */
};