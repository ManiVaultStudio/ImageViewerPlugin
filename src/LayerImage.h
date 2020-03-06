#pragma once

#include <QImage>

/** TODO */
class LayerImage : public QObject
{
public:

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
		float setMin(const float& min) { return _min = std::min(min, _max); }

		/** TODO */
		float max() const { return _max; }

		/** TODO */
		float setMax(const float& max) { return _max = std::max(_min, max); }

	private:
		float	_min;	/** TODO */
		float	_max;	/** TODO */
	};

	/** TODO */
	LayerImage(QObject* parent = nullptr, const float& window = 1.0f, const float& level = 0.5f);

	/** TODO */
	QImage image() const { return _image; }

	/** TODO */
	void setImage(const QImage& image);

	/** TODO */
	Range imageRange() const { return _imageRange; }

	/** TODO */
	Range displayRange() const { return _displayRange; }

	/** TODO */
	float windowNormalized() const { return _windowNormalized; }

	/** TODO */
	void setWindowNormalized(const float& windowNormalized) { _windowNormalized = windowNormalized; }

	/** TODO */
	float levelNormalized() const { return _levelNormalized; }

	/** TODO */
	void setLevelNormalized(const float& levelNormalized) { _levelNormalized = levelNormalized; }

	/** TODO */
	float window() const { return _window; }

	/** TODO */
	void setWindow(const float& window) { _window = window; }

	/** TODO */
	float level() const { return _level; }

	/** TODO */
	void setLevel(const float& level) { _level = level; }

public:
	QImage		_image;					/** TODO */
	Range		_imageRange;			/** TODO */
	Range		_displayRange;			/** TODO */
	float		_windowNormalized;		/** TODO */
	float		_levelNormalized;		/** TODO */
	float		_window;				/** TODO */
	float		_level;					/** TODO */
};

//Q_DECLARE_METATYPE(LayerImage);

/*
Layer::Range LayersModel::imageRange(const QImage& image)
{
	auto imageBits = reinterpret_cast<ushort*>(const_cast<uchar*>(image.bits()));

	const auto noPixels = image.width() * image.height();

	Layer::Range range;

	range.setMin(std::numeric_limits<float>::max());
	range.setMax(std::numeric_limits<float>::min());

	for (std::int32_t y = 0; y < image.height(); y++)
	{
		for (std::int32_t x = 0; x < image.width(); x++)
		{
			const auto pixelId = y * image.width() + x;

			for (int c = 0; c < 3; c++)
			{
				const auto channel = static_cast<float>(imageBits[pixelId * 4 + c]);

				if (channel < range.min())
					range.setMin(channel);

				if (channel > range.max())
					range.setMax(channel);
			}
		}
	}

	return range;
}

Layer::Range LayersModel::displayRange(const QImage& image)
{
	const auto maxWindow = _imageRange.second - _imageRange.first;

	_level = std::clamp(_imageRange.first + (_levelNormalized * maxWindow), _imageRange.first, _imageRange.second);
	_window = std::clamp(_windowNormalized * maxWindow, _imageRange.first, _imageRange.second);
	_displayRange.first = std::clamp(_level - (_window / 2.0f), _imageRange.first, _imageRange.second);
	_displayRange.second = std::clamp(_level + (_window / 2.0f), _imageRange.first, _imageRange.second);
}
*/