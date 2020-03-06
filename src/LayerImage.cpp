#include "LayerImage.h"

#include <QDebug>

LayerImage::LayerImage(QObject* parent, const float& window /*= 1.0f*/, const float& level /*= 0.5f*/) :
	QObject(parent),
	_image(),
	_imageRange(),
	_displayRange(),
	_windowNormalized(1.0f),
	_levelNormalized(0.5f),
	_window(window),
	_level(level)
{
}

void LayerImage::setImage(const QImage& image)
{

}

/*
			auto image = data(topLeft.row(), LayersModel::Columns::Image, Qt::EditRole).value<QImage>();
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

			setData(topLeft.row(), LayersModel::Columns::ImageRange, QVariant::fromValue(range));
			*/