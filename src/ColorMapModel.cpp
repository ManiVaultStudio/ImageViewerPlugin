#include "ColorMapModel.h"

#include <QDebug>
#include <QDirIterator>
#include <QPainter>

ColorMapModel::ColorMapModel(QObject* parent, const ColorMap::Type& type) :
    QAbstractListModel(parent),
    _colorMaps()
{
    setupModelData();
}

int ColorMapModel::columnCount(const QModelIndex& parent) const
{
    return ult(Column::End) + 1;
}

int ColorMapModel::rowCount(const QModelIndex& parent /* = QModelIndex() */) const
{
    return _colorMaps.count();
}

QVariant ColorMapModel::data(const QModelIndex& index, int role /* = Qt::DisplayRole */) const
{
    if (!index.isValid())
        return QVariant();

    const auto colorMap = _colorMaps.at(index.row());

    auto icon = [](const QImage& image, const QSize& size) {
        auto pixmap     = QPixmap::fromImage(image).scaled(size);
        auto painter    = QPainter(&pixmap);

        painter.setPen(QPen(QBrush(QColor(30, 30, 30)), 1.5f));

        QPointF points[5] = {
            QPointF(0.0f, 0.0f),
            QPointF(size.width(), 0.0f),
            QPointF(size.width(), size.height()),
            QPointF(0.0f, size.height()),
            QPointF(0.0f, 0.0f)
        };

        painter.drawPolyline(points, 5);

        return pixmap;
    };

    switch (role) {
        case Qt::DecorationRole:
        {
            switch (index.column()) {
                case ult(Column::Preview):
                {
                    switch (colorMap.getNoDimensions())
                    {
                        case 0:
                            return icon(colorMap.getImage(), QSize(15, 15));

                        case 1:
                            return icon(colorMap.getImage(), QSize(60, 12));

                        case 2:
                            return icon(colorMap.getImage(), QSize(32, 32));

                        default:
                            break;
                    }

                    break;
                }
                    
                case ult(Column::Name):
                case ult(Column::Image):
                case ult(Column::ResourcePath):
                    break;
            }

            break;
        }

        case Qt::DisplayRole:
        {
            switch (index.column()) {
                case ult(Column::Preview):
                    return colorMap.getName();

                case ult(Column::Name):
                    return colorMap.getName();

                case ult(Column::Image):
                    break;

                case ult(Column::ResourcePath):
                    colorMap.getResourcePath();
            }

            break;
        }
        
        case Qt::EditRole:
        {
            switch (index.column()) {
                case ult(Column::Preview):
                    return QVariant();

                case ult(Column::Name):
                    return colorMap.getName();

                case ult(Column::Image):
                    return colorMap.getImage();

                case ult(Column::NoDimensions):
                    return colorMap.getNoDimensions();

                case ult(Column::ResourcePath):
                    return colorMap.getResourcePath();
            }
            break;
        }

        case Qt::TextAlignmentRole:
            return static_cast<int>(Qt::AlignLeft | Qt::AlignVCenter);

        default:
            break;
    }

    return QVariant();
}

void ColorMapModel::setupModelData()
{
    auto prefix = ":/resources/colormaps";

    const auto noSteps = 256;

    QImage blackToWhite(noSteps, 1, QImage::Format::Format_RGB32);

    for (int value = 0; value < noSteps; ++value) {
        blackToWhite.setPixelColor(value, 0, QColor(value, value, value, 255));
    }

    _colorMaps.append(ColorMap("Black to white", "", ColorMap::Type::OneDimensional, blackToWhite));

    QDirIterator iterator1D(QString("%1/1D/").arg(prefix), QDirIterator::Subdirectories);

    while (iterator1D.hasNext()) {
        const auto resourcePath = iterator1D.next();
        _colorMaps.append(ColorMap(QFileInfo(resourcePath).baseName(), resourcePath, ColorMap::Type::OneDimensional, QImage(resourcePath)));
    }

    QDirIterator iterator2D(QString("%1/2D/").arg(prefix), QDirIterator::Subdirectories);

    while (iterator2D.hasNext()) {
        const auto resourcePath = iterator2D.next();
        _colorMaps.append(ColorMap(QFileInfo(resourcePath).baseName(), resourcePath, ColorMap::Type::TwoDimensional, QImage(resourcePath)));
    }

    const auto noHueSteps   = 36;
    const auto hueDelta     = 360.0f / noHueSteps;
    
    for (int h = 0; h < noHueSteps; ++h)
    {
        const auto color = QColor::fromHsl(h * hueDelta, 255, 125);

        auto colorMapImage = QImage(32, 32, QImage::Format::Format_RGB888);
        
        colorMapImage.fill(color);

        const auto name = QString("[%1, %2, %3]").arg(QString::number(color.red()), QString::number(color.green()), QString::number(color.blue()));

        _colorMaps.append(ColorMap(name, "", ColorMap::Type::ZeroDimensional, colorMapImage));
    }

    beginInsertRows(QModelIndex(), 0, _colorMaps.count());
    endInsertRows();
}

const ColorMap* ColorMapModel::getColorMap(const int& row) const
{
    const auto colorMapIndex = index(row, 0);

    if (!colorMapIndex.isValid())
        return nullptr;

    return &_colorMaps.at(row);
}