#pragma once

#include <vector>
#include <map>

#include <QDebug>

template<class PixelType>
class Image
{
public:
	Image() :
		_width(0),
		_height(0),
		_pixels(),
		_min(std::numeric_limits<double>::min()),
		_max(std::numeric_limits<double>::max())
	{
	}

	Image(const std::uint32_t& width, const std::uint32_t& height) :
		_width(0),
		_height(0),
		_pixels(),
		_min(std::numeric_limits<double>::min()),
		_max(std::numeric_limits<double>::max())
	{
		create(width, height);
	}

	void create(const std::uint32_t& width, const std::uint32_t& height)
	{
		if (width == _width && height == _height)
			return;

		_width	= width;
		_height = height;

		_pixels.clear();
		_pixels.resize(noPixels());
	}

	void computeMinMax()
	{
		const auto[min, max] = std::minmax_element(begin(_pixels), end(_pixels));

		_min = static_cast<PixelType>(*min);
		_max = static_cast<PixelType>(*max);
	}

	void setPixel(const std::uint32_t& x, const std::uint32_t& y, const PixelType& pixel)
	{
		_pixels[y * _width + x] = pixel;
	}

	void computeWindowLevel(const double& windowNorm, const double& levelNorm, double& window, double& level)
	{
		const double min = _min;
		const double max = _max;
		const double maxWindow = max - min;

		level = std::clamp(min, min + (levelNorm * maxWindow), max);
		window = std::clamp(min, windowNorm * maxWindow, max);

		/*
		double window = 0.0;
		double level = 0.0;

		computeWindowLevel(window, level);

		const auto minPixelValue = std::clamp(_imageViewerPlugin->imageMin(), level - (window / 2.0), _imageViewerPlugin->imageMax());
		const auto maxPixelValue = std::clamp(_imageViewerPlugin->imageMin(), level + (window / 2.0), _imageViewerPlugin->imageMax());
		*/
	}

	std::uint32_t width() const
	{
		return _width;
	}

	std::uint32_t height() const
	{
		return _height;
	}

	std::uint32_t noPixels() const
	{
		return _width * _height;
	}

	bool isCreated() const
	{
		return noPixels() > 0;
	}

	std::vector<PixelType>& pixels()
	{
		return _pixels;
	}

	PixelType min() const
	{
		return _min;
	}

	PixelType max() const
	{
		return _max;
	}

private:
	std::uint32_t			_width;
	std::uint32_t			_height;
	std::vector<PixelType>	_pixels;
	PixelType				_min;
	PixelType				_max;
};

template<class PixelType>
QDebug operator<<(QDebug dbg, const Image<PixelType>& image)
{
	dbg << QString("%1x%2").arg(QString::number(image.width()), QString::number(image.height()));

	return dbg;
}