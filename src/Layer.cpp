#include "Layer.h"
#include "DataHierarchyItem.h"

Layer::Layer(ImageViewerPlugin* imageViewerPlugin, const QString& datasetName) :
    QObject(),
    _imageViewerPlugin(imageViewerPlugin),
    _images(datasetName),
    _points(_images->getHierarchyItem().getParent()->getDatasetName()),
    _layerAction(*this)
{
    if (!_images.isValid())
        throw std::runtime_error("The layer images dataset is not valid after initialization");

    if (!_points.isValid())
        throw std::runtime_error("The layer points dataset is not valid after initialization");
}

ImageViewerPlugin* Layer::getImageViewerPlugin()
{
    return _imageViewerPlugin;
}

const QString Layer::getImagesDatasetName() const
{
    if (!_images.isValid())
        throw std::runtime_error("The images dataset is not valid");

    return _images.getDatasetName();
}

const std::uint32_t Layer::getNumberOfImages() const
{
    if (!_images.isValid())
        throw std::runtime_error("The images dataset is not valid");

    return _images->getNumberOfImages();
}

const QSize Layer::getImageSize() const
{
    if (!_images.isValid())
        throw std::runtime_error("The images dataset is not valid");

    return _images->getImageSize();
}

const std::uint32_t Layer::getNumberOfPoints() const
{
    if (!_images.isValid() || !_points.isValid())
        throw std::runtime_error("Unable to retrieve the number of data points from layer; the images/points dataset is not valid");

    return _points->getNumPoints();
}

const std::uint32_t Layer::getNumberOfDimensions() const
{
    if (!_images.isValid() || !_points.isValid())
        throw std::runtime_error("Unable to retrieve the number of data dimensions from layer; the images/points dataset not valid");

    return _points->getNumDimensions();
}

const QStringList Layer::getDimensionNames() const
{
    if (!_images.isValid() || !_points.isValid())
        throw std::runtime_error("Unable to retrieve the number of data points from layer; the images/points dataset not valid");

    QStringList dimensionNames;

    if (_points->getDimensionNames().size() == getNumberOfDimensions()) {
        for (const auto& dimensionName : _points->getDimensionNames())
            dimensionNames << dimensionName;
    }
    else {
        for (const auto& dimensionName : _points->getDimensionNames())
            dimensionNames << QString("Dim %1").arg(QString::number(dimensionNames.count()));
    }

    return dimensionNames;
}

//#include "ImageViewerPlugin.h"
//#include "Renderer.h"
//
//#include "ImageData/Images.h"
//#include "PointData.h"
//
//#include "Application.h"
//
//#include <QFont>
//#include <QDebug>
//#include <QPainter>
//#include <QTextDocument>
//#include <QtMath>
//
//ImageViewerPlugin* Layer::imageViewerPlugin     = nullptr;
//bool Layer::showHints                           = true;
//const QColor Layer::hintsColor                  = QColor(255, 174, 66, 200);
//const qreal Layer::textMargins                  = 10.0;
//
//Layer::Layer(const QString& datasetName, const Type& type, const QString& id, const QString& name, const int& flags) :
//    Node(id, name, flags),
//    hdps::EventListener(),
//    _datasetName(datasetName),
//    _type(type),
//    _mousePositions(),
//    _mouseButtons(),
//    _keys()
//{
//}
//
//Layer::~Layer() = default;
//
//void Layer::matchScaling(const QSize& targetImageSize)
//{
//    const auto layerImageSize   = getImageSize();
//    const auto widthScaling     = static_cast<float>(targetImageSize.width()) / layerImageSize.width();
//    const auto heightScaling    = static_cast<float>(targetImageSize.height()) / layerImageSize.height();
//
//    const auto scale = std::min(widthScaling, heightScaling);
//
//    setScale(scale);
//}
//
//void Layer::paint(QPainter* painter)
//{
//    drawTitle(painter);
//    drawHints(painter);
//}
//
//void Layer::zoomExtents()
//{
//    renderer->zoomToRectangle(getBoundingRectangle());
//}
//
//Qt::ItemFlags Layer::getFlags(const QModelIndex& index) const
//{
//    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
//
//    const auto type = static_cast<Type>(_type);
//
//    switch (static_cast<Column>(index.column())) {
//        case Column::Name:
//        {
//            flags |= Qt::ItemIsUserCheckable;
//
//            if (getFlag(Layer::Flag::Renamable, Qt::EditRole).toBool())
//                flags |= Qt::ItemIsEditable;
//
//            break;
//        }
//
//        case Column::DatasetName:
//        case Column::Type:
//        case Column::ID:
//        case Column::ImageSize:
//        case Column::ImageWidth:
//        case Column::ImageHeight:
//            break;
//
//        case Column::Opacity:
//            flags |= Qt::ItemIsEditable;
//            break;
//
//        case Column::Scale:
//            flags |= Qt::ItemIsEditable;
//            break;
//
//        case Column::Flags:
//            break;
//
//        default:
//            break;
//    }
//
//    return flags;
//}
//
//QVariant Layer::getData(const QModelIndex& index, const int& role) const
//{
//    switch (static_cast<Column>(index.column())) {
//        case Column::Name:
//            return getName(role);
//
//        case Column::DatasetName:
//            return getDatasetName(role);
//
//        case Column::Type:
//            return getType(role);
//
//        case Column::ID:
//            return getID(role);
//
//        case Column::ImageSize:
//            return getImageSize(role);
//
//        case Column::ImageWidth:
//            return getImageWidth(role);
//
//        case Column::ImageHeight:
//            return getImageHeight(role);
//
//        case Column::Opacity:
//            return getOpacity(role);
//
//        case Column::Scale:
//            return getScale(role);
//
//        case Column::Flags:
//            return Node::getFlags(role);
//
//        default:
//            break;
//    }
//
//    return QVariant();
//}
//
//QModelIndexList Layer::setData(const QModelIndex& index, const QVariant& value, const int& role)
//{
//    QModelIndexList affectedIndices{index};
//
//    const auto column = static_cast<Column>(index.column());
//
//    switch (role)
//    {
//        case Qt::CheckStateRole:
//        {
//            switch (column) {
//                case Column::Name:
//                {
//                    setFlag(Layer::Flag::Enabled, value.toBool());
//
//                    for (int column = ult(Column::Type); column <= ult(Column::End); ++column) {
//                        affectedIndices << index.siblingAtColumn(column);
//                    }
//
//                    auto parent = index.parent();
//
//                    while (parent.isValid()) {
//                        affectedIndices << parent;
//                        parent = parent.parent();
//                    }
//                    
//                    break;
//                }
//
//                default:
//                    break;
//            }
//
//            break;
//        }
//
//        case Qt::EditRole:
//        {
//            switch (column) {
//                case Column::Name:
//                    setName(value.toString());
//                    break;
//
//                case Column::DatasetName:
//                    break;
//
//                case Column::Type:
//                    setType(static_cast<Type>(value.toInt()));
//                    break;
//
//                case Column::ID:
//                    setId(value.toString());
//                    break;
//
//                case Column::ImageSize:
//                case Column::ImageWidth:
//                case Column::ImageHeight:
//                    break;
//
//                case Column::Opacity:
//                    setOpacity(value.toFloat());
//                    break;
//
//                case Column::Scale:
//                    setScale(value.toFloat());
//                    break;
//
//                case Column::Flags:
//                    setFlags(value.toInt());
//                    break;
//
//                default:
//                    break;
//            }
//
//            break;
//        }
//
//        default:
//            break;
//    }
//
//    Renderable::renderer->render();
//
//    return affectedIndices;
//}
//
//QVariant Layer::getDatasetName(const int& role) const
//{
//    switch (role)
//    {
//        case Qt::DisplayRole:
//        case Qt::EditRole:
//            return _datasetName;
//
//        case Qt::ToolTipRole:
//            return QString("Dataset name: %1").arg(_datasetName);
//
//        default:
//            break;
//    }
//
//    return QVariant();
//}
//
//QVariant Layer::getType(const int& role) const
//{
//    const auto typeName = Layer::getTypeName(_type);
//
//    switch (role)
//    {
//        case Qt::FontRole:
//            return hdps::Application::getIconFont("FontAwesome").getFont(9);
//
//        case Qt::EditRole:
//            return static_cast<int>(_type);
//
//        case Qt::ToolTipRole:
//            return QString("Type: %1").arg(typeName);
//
//        case Qt::DisplayRole:
//        {
//            switch (_type) {
//                case Type::Selection:
//                    return hdps::Application::getIconFont("FontAwesome").getIconCharacter("mouse-pointer");
//
//                case Type::Points:
//                    return hdps::Application::getIconFont("FontAwesome").getIconCharacter("th");
//
//                default:
//                    break;
//            }
//
//            break;
//        }
//
//        case Qt::TextAlignmentRole:
//            return Qt::AlignCenter;
//
//        default:
//            break;
//    }
//
//    return QVariant();
//}
//
//void Layer::setType(const Type& type)
//{
//    _type = type;
//}
//
//QVariant Layer::getImageSize(const int& role /*= Qt::DisplayRole*/) const
//{
//    const auto imageSize        = this->getImageSize();
//    const auto imageSizeString  = QString::number(imageSize.width()) + " x" + QString::number(imageSize.height());
//
//    switch (role)
//    {
//        case Qt::DisplayRole:
//            return imageSizeString;
//
//        case Qt::EditRole:
//            return imageSize;
//
//        case Qt::ToolTipRole:
//            return QString("Image size: %1").arg(imageSizeString);
//
//        default:
//            break;
//    }
//
//    return QVariant();
//}
//
//// TODO
//void Layer::updateModelMatrix()
//{
//    QMatrix4x4 modelMatrix;
//
//    modelMatrix.translate(-0.5f * getImageWidth(Qt::EditRole).toInt(), -0.5f * getImageHeight(Qt::EditRole).toInt(), 0.0f);
//
//    //setModelMatrix(modelMatrix);
//}
//
//QPoint Layer::getTextureCoordinateFromScreenPoint(const QPoint& screenPoint) const
//{
//    auto correctedScreenPosition = QPoint(renderer->getParentWidgetSize().width() - screenPoint.x(), screenPoint.y());
//    const auto worldPosition = renderer->getScreenPointToWorldPosition(getModelViewMatrix(), correctedScreenPosition);
//
//    return QPoint(qFloor(worldPosition.x()), qFloor(worldPosition.y()));
//}
//
//bool Layer::isWithin(const QPoint& screenPoint) const
//{
//    const auto textureCoordinate = getTextureCoordinateFromScreenPoint(screenPoint);
//
//    if (textureCoordinate.x() < 0 || textureCoordinate.x() >= getImageWidth(Qt::EditRole).toInt())
//        return false;
//
//    if (textureCoordinate.y() < 0 || textureCoordinate.y() >= getImageHeight(Qt::EditRole).toInt())
//        return false;
//
//    return true;
//}
//
//QVariant Layer::getImageWidth(const int& role) const
//{
//    const auto imageSize    = this->getImageSize(Qt::EditRole).toSize();
//    const auto widthString  = QString::number(imageSize.width());
//
//    switch (role)
//    {
//        case Qt::DisplayRole:
//            return widthString;
//
//        case Qt::EditRole:
//            return imageSize.width();
//
//        case Qt::ToolTipRole:
//            return QString("Image width: %1 pixels").arg(widthString);
//
//        default:
//            break;
//    }
//
//    return QVariant();
//}
//
//QVariant Layer::getImageHeight(const int& role) const
//{
//    const auto imageSize    = this->getImageSize(Qt::EditRole).toSize();
//    const auto heightString = QString::number(imageSize.height());
//
//    switch (role)
//    {
//        case Qt::DisplayRole:
//            return heightString;
//
//        case Qt::EditRole:
//            return imageSize.height();
//
//        case Qt::ToolTipRole:
//            return QString("Image height: %1 pixels").arg(heightString);
//
//        default:
//            break;
//    }
//
//    return QVariant();
//}
//
//QVariant Layer::getKeys(const int& role /*= Qt::DisplayRole*/) const
//{
//    const auto keysString = "";
//
//    switch (role)
//    {
//        case Qt::DisplayRole:
//            return keysString;
//
//        case Qt::EditRole:
//            return _keys;
//
//        case Qt::ToolTipRole:
//        {
//            return QString("Keys: %1").arg(keysString);
//        }
//
//        default:
//            break;
//    }
//
//    return QVariant();
//}
//
//void Layer::setKeys(const int& keys)
//{
//    _keys = keys;
//}
//
//QVector<QPoint> Layer::getMousePositions() const
//{
//    return _mousePositions;
//}
//
//int Layer::getNoPixels() const
//{
//    return getImageSize().width() * getImageSize().height();
//}
//
//Layer::Hints Layer::getHints() const
//{
//    return Hints({
//        Hint(),
//        Hint(),
//        Hint(),
//        Hint("Space + Scroll up", "Zoom in"),
//        Hint("Space + Scroll down", "Zoom out"),
//        Hint("Space + Move mouse", "Pan view")
//    });
//}
//
//void Layer::drawTitle(QPainter* painter)
//{
//    QTextDocument titleDocument;
//
//    const auto color = QString("rgba(%1, %2, %3, %4)").arg(QString::number(hintsColor.red()), QString::number(hintsColor.green()), QString::number(hintsColor.blue()), QString::number(isFlagSet(Flag::Enabled) ? hintsColor.alpha() : 80));
//
//    QString titleHtml = QString("<div style='width: 100%; text-align: center; color: %1; font-weight: bold;'>%2 (%3x%4)<div>").arg(color, _name, QString::number(getImageSize().width()), QString::number(getImageSize().height()));
//    
//    titleDocument.setTextWidth(painter->viewport().width());
//    titleDocument.setDocumentMargin(textMargins);
//    titleDocument.setHtml(titleHtml);
//    titleDocument.drawContents(painter, painter->viewport());
//}
//
//void Layer::drawHints(QPainter* painter)
//{
//    if (!Layer::showHints)
//        return;
//
//    QTextDocument hintsDocument;
//
//    QString hintsHtml;
//
//    const auto color = QString("rgba(%1, %2, %3, %4)").arg(QString::number(hintsColor.red()), QString::number(hintsColor.green()), QString::number(hintsColor.blue()), QString::number(isFlagSet(Flag::Enabled) ? hintsColor.alpha() : 80));
//
//    hintsHtml += QString("<div style='height: 100%'><table style='color: %1;'>").arg(color);
//
//    for (auto hint : getHints()) {
//        if (hint.getTitle().isEmpty())
//            hintsHtml += "<tr><td></td><td></td></tr>";
//        else
//            hintsHtml += QString("<tr style='font-weight: %1'><td width=120>%2</td><td>: %3</td></tr>").arg(hint.isActive() ? "bold" : "normal", hint.getTitle(), hint.getDescription());
//    }
//
//    hintsHtml += "</table></div>";
//
//    hintsDocument.setTextWidth(painter->viewport().width());
//    hintsDocument.setDocumentMargin(textMargins);
//    hintsDocument.setHtml(hintsHtml);
//    hintsDocument.drawContents(painter);
//}


