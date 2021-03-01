#include "SelectionLayer.h"
#include "ImageViewerPlugin.h"
#include "SelectionProp.h"
#include "SelectionToolProp.h"
#include "Renderer.h"

#include "PointData.h"
#include "util/Timer.h"
#include "Application.h"

#include <set>

#include <cmath>

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#include <QDebug>
#include <QPainter>
#include <QTextDocument>
#include <QKeyEvent>
#include <QtMath>

const QColor SelectionLayer::toolColorForeground = QColor(255, 174, 66, 200);
const QColor SelectionLayer::toolColorBackground = QColor(255, 174, 66, 150);
const QColor SelectionLayer::fillColor = QColor(255, 174, 66, 120);
const float SelectionLayer::minBrushRadius = 1.0f;
const float SelectionLayer::maxBrushRadius = 1000.0f;
const float SelectionLayer::defaultBrushRadius = 50.0f;
const float SelectionLayer::controlPointSize = 6.0f;
const float SelectionLayer::perimeterLineWidth = 1.5f;
const QPoint SelectionLayer::textPosition = QPoint(25, 25);

SelectionLayer::SelectionLayer(const QString& datasetName, const QString& id, const QString& name, const int& flags) :
    Layer(datasetName, Layer::Type::Selection, id, name, flags),
    Channels<std::uint8_t>(ult(ChannelIndex::Count)),
    _pointsDataset(nullptr),
    _image(),
    _selectionType(SelectionType::Rectangle),
    _selectionModifier(SelectionModifier::Replace),
    _brushRadius(defaultBrushRadius),
    _overlayColor(Qt::red),
    _autoZoomToSelection(false),
    _pixelBounds(),
    _selectionBounds()
{
    init();
}

void SelectionLayer::init()
{
    addProp<SelectionToolProp>(this, "SelectionTool");
    addProp<SelectionProp>(this, "Selection");

    _pointsDataset = &imageViewerPlugin->requestData<Points>(_datasetName);

    setEventCore(imageViewerPlugin->core());

    registerDataEventByType(PointType, [this](hdps::DataEvent* dataEvent) {
        computeChannel(ChannelIndex::Selection);
        Renderable::renderer->render();

        /*
        QModelIndexList affectedIds;

        affectedIds << index.siblingAtColumn(ult(Column::SelectAll));
        affectedIds << index.siblingAtColumn(ult(Column::SelectNone));
        affectedIds << index.siblingAtColumn(ult(Column::InvertSelection));
        affectedIds << index.siblingAtColumn(ult(Column::ZoomToSelection));
        affectedIds << index.siblingAtColumn(ult(Column::CreateSubset));
        */
    });

    
    updateModelMatrix();
    computeChannel(ChannelIndex::Selection);
}

void SelectionLayer::paint(QPainter* painter)
{
    Layer::paint(painter);

    /*
    if (!_selectionBounds.isNull()) {
        auto selectionBoundsPen = QPen();

        selectionBoundsPen.setColor(toolColorForeground);
        selectionBoundsPen.setWidthF(1.5f);
        selectionBoundsPen.setStyle(Qt::DotLine);

        painter->setPen(selectionBoundsPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(_selectionBounds);
    }
    */

    if (!isFlagSet(Node::Flag::Enabled) || renderer->interactionMode() != InteractionMode::LayerEditing)
        return;

    painter->setFont(QFont("Font Awesome 5 Free Solid", 9));

    auto textRectangle = QRectF();

    auto perimeterForegroundPen = QPen();

    perimeterForegroundPen.setColor(toolColorForeground);
    perimeterForegroundPen.setWidthF(perimeterLineWidth);
    perimeterForegroundPen.setStyle(Qt::SolidLine);

    auto perimeterBackgroundPen = QPen();

    perimeterBackgroundPen.setColor(toolColorBackground);
    perimeterBackgroundPen.setWidthF(perimeterLineWidth);
    perimeterBackgroundPen.setStyle(Qt::DotLine);

    auto controlPointsPen = QPen();

    controlPointsPen.setColor(toolColorForeground);
    controlPointsPen.setWidthF(controlPointSize);
    controlPointsPen.setCapStyle(Qt::RoundCap);

    auto fillBrush = QBrush(fillColor);

    QVector<QPoint> controlPoints;

    switch (_selectionType)
    {
        case SelectionType::None:
            break;

        case SelectionType::Rectangle:
        {
            if (_mousePositions.size() == 2) {
                const auto topLeft      = QPointF(std::min(_mousePositions.first().x(), _mousePositions.last().x()), std::min(_mousePositions.first().y(), _mousePositions.last().y()));
                const auto bottomRight  = QPointF(std::max(_mousePositions.first().x(), _mousePositions.last().x()), std::max(_mousePositions.first().y(), _mousePositions.last().y()));
                const auto rectangle    = QRectF(topLeft, bottomRight);

                painter->setPen(perimeterForegroundPen);
                painter->setBrush(Qt::NoBrush);
                painter->drawRect(rectangle);

                controlPoints << _mousePositions.first();
                controlPoints << _mousePositions.last();

                const auto size = 8.0f;
                const auto textCenter = rectangle.topRight() + QPoint(size, -size);

                textRectangle = QRectF(textCenter - QPointF(size, size), textCenter + QPointF(size, size));
            }

            break;
        }

        case SelectionType::Brush:
        {
            if (_mousePositions.size() >= 1) {
                const auto brushCenter = _mousePositions.last();

                painter->setPen(Qt::NoPen);
                painter->setBrush(toolColorForeground);

                painter->drawPoint(brushCenter);

                painter->setPen(_mouseButtons & Qt::LeftButton ? perimeterForegroundPen : perimeterBackgroundPen);

                painter->setBrush(Qt::NoBrush);
                painter->drawEllipse(QPointF(brushCenter), _brushRadius, _brushRadius);

                controlPoints << _mousePositions.last();

                const auto textAngle = 0.75f * M_PI;
                const auto size = 12.0f;
                const auto textCenter = brushCenter + (_brushRadius + size) * QPointF(sin(textAngle), cos(textAngle));

                textRectangle = QRectF(textCenter - QPointF(size, size), textCenter + QPointF(size, size));
            }

            break;
        }

        case SelectionType::Lasso:
        {
            if (_mousePositions.size() >= 2) {
                painter->setPen(perimeterForegroundPen);
                painter->drawPolyline(_mousePositions.constData(), _mousePositions.count());

                controlPoints << _mousePositions.first();
                controlPoints << _mousePositions.last();

                painter->setPen(perimeterBackgroundPen);
                painter->drawPolyline(controlPoints.constData(), controlPoints.count());

                const auto size = 8.0f;
                const auto textCenter = _mousePositions.first() - QPoint(size, size);

                textRectangle = QRectF(textCenter - QPointF(size, size), textCenter + QPointF(size, size));
            }

            break;
        }

        case SelectionType::Polygon:
        {
            if (_mousePositions.size() >= 2) {
                painter->setPen(QPen(QBrush(toolColorForeground), perimeterLineWidth));
                painter->drawPolyline(_mousePositions.constData(), _mousePositions.count());

                QVector<QPoint> connectingPoints;

                connectingPoints << _mousePositions.first();
                connectingPoints << _mousePositions.last();

                painter->setPen(QPen(QBrush(toolColorBackground), perimeterLineWidth, Qt::DashLine));

                painter->drawPolyline(connectingPoints.constData(), connectingPoints.count());

                controlPoints << _mousePositions;

                const auto size = 8.0f;
                const auto textCenter = _mousePositions.first() - QPoint(size, size);

                textRectangle = QRectF(textCenter - QPointF(size, size), textCenter + QPointF(size, size));
            }

            break;
        }

        case SelectionType::Sample:
        {
            if (_mousePositions.size() == 1) {
                const auto center = _mousePositions.last();
                const auto r1 = 3.0f;
                const auto r2 = 10.0f;

                painter->setPen(QPen(QBrush(toolColorForeground), perimeterLineWidth));

                for (int section = 0; section < 4; ++section) {
                    const auto rotation = section * M_PI_2;
                    const auto point    = QPointF(qSin(rotation), qCos(rotation));

                    painter->drawLine(center + r1 * point, center + r2 * point);
                }
            }

            break;
        }

        default:
            break;
    }

    painter->setPen(controlPointsPen);
    painter->drawPoints(controlPoints);

    switch (_selectionType)
    {
        case SelectionType::None:
            break;

        case SelectionType::Rectangle:
        case SelectionType::Brush:
        case SelectionType::Lasso:
        case SelectionType::Polygon:
        {
            switch (_selectionModifier)
            {
                case SelectionModifier::Replace:
                    break;

                case SelectionModifier::Add:
                {
                    painter->setPen(toolColorForeground);
                    painter->drawText(textRectangle, hdps::Application::getIconFont("FontAwesome").getIconCharacter("plus-circle"), QTextOption(Qt::AlignCenter));
                    break;
                }

                case SelectionModifier::Remove:
                {
                    painter->setPen(toolColorForeground);
                    painter->drawText(textRectangle, hdps::Application::getIconFont("FontAwesome").getIconCharacter("minus-circle"), QTextOption(Qt::AlignCenter));
                    break;
                }

                case SelectionModifier::All:
                case SelectionModifier::None:
                case SelectionModifier::Invert:
                    break;
                default:
                    break;
            }

            break;
        }

        default:
            break;
    }
}

void SelectionLayer::handleEvent(QEvent* event, const QModelIndex& index)
{
    if (renderer->interactionMode() != InteractionMode::LayerEditing)
        return;

    QModelIndexList affectedIds;

    switch (event->type())
    {
        // Prevent recursive paint events
        case QEvent::Paint:
            return;

        case QEvent::MouseButtonPress:
        {
            auto mouseEvent = static_cast<QMouseEvent*>(event);

            _mouseButtons = mouseEvent->buttons();

            switch (_selectionType)
            {
                case SelectionType::None:
                case SelectionType::Rectangle:
                {
                    _mousePositions.clear();
                    _mousePositions << mouseEvent->pos();
                    break;
                }

                case SelectionType::Lasso:
                {
                    _mousePositions.clear();
                    _mousePositions << mouseEvent->pos();
                    break;
                }

                case SelectionType::Polygon:
                {
                    switch (mouseEvent->button())
                    {
                        case Qt::LeftButton:
                        {
                            _mousePositions << mouseEvent->pos();
                            break;
                        }

                        case Qt::RightButton:
                        {
                            _mousePositions << mouseEvent->pos();
                            break;
                        }

                        default:
                            break;
                    }

                    break;
                }

                case SelectionType::Brush:
                case SelectionType::Sample:
                {
                    _mousePositions.clear();
                    _mousePositions << mouseEvent->pos();
                    break;
                }

                default:
                    break;
            }

            break;
        }

        case QEvent::MouseButtonRelease:
        {
            auto mouseEvent = static_cast<QMouseEvent*>(event);

            _mouseButtons = mouseEvent->buttons();

            switch (_selectionType)
            {
                case SelectionType::None:
                    break;

                case SelectionType::Rectangle:
                {
                    if (mouseEvent->button() == Qt::LeftButton) {
                        publishSelection();
                        _mousePositions.clear();
                    }

                    break;
                }

                case SelectionType::Lasso:
                {
                    if (mouseEvent->button() == Qt::LeftButton) {
                        publishSelection();
                        _mousePositions.clear();
                    }

                    break;
                }

                case SelectionType::Polygon:
                {
                    if (mouseEvent->button() == Qt::RightButton) {
                        _mousePositions.removeLast();
                        publishSelection();
                        _mousePositions.clear();
                    }

                    break;
                }

                case SelectionType::Brush:
                {
                    publishSelection();
                    _mousePositions.clear();

                    break;
                }

                default:
                    break;
            }

            break;
        }

        case QEvent::MouseMove:
        {
            auto mouseEvent = static_cast<QMouseEvent*>(event);

            auto shouldComputePixelSelection = false;

            switch (_selectionType)
            {
                case SelectionType::None:
                    break;

                case SelectionType::Rectangle:
                {
                    if (mouseEvent->buttons() & Qt::LeftButton) {
                        if (_mousePositions.size() != 2)
                            _mousePositions << mouseEvent->pos();
                        else
                            _mousePositions.last() = mouseEvent->pos();

                        shouldComputePixelSelection = true;
                    }

                    break;
                }

                case SelectionType::Brush:
                {
                    if (mouseEvent->buttons() & Qt::LeftButton) {
                        _mousePositions << mouseEvent->pos();
                        shouldComputePixelSelection = true;
                    }
                    else {
                        if (_mousePositions.isEmpty())
                            _mousePositions << mouseEvent->pos();

                        _mousePositions.last() = mouseEvent->pos();
                    }

                    break;
                }

                case SelectionType::Lasso:
                {
                    if (mouseEvent->buttons() & Qt::LeftButton)
                        _mousePositions << mouseEvent->pos();

                    shouldComputePixelSelection = true;

                    break;
                }

                case SelectionType::Polygon:
                {
                    if (_mousePositions.isEmpty())
                        _mousePositions << mouseEvent->pos();
                    else
                        _mousePositions.last() = mouseEvent->pos();

                    shouldComputePixelSelection = true;

                    break;
                }

                case SelectionType::Sample:
                {
                    if (_mousePositions.isEmpty())
                        _mousePositions << mouseEvent->pos();
                    else
                        _mousePositions.last() = mouseEvent->pos();

                    shouldComputePixelSelection = false;

                    publishSelection();

                    break;
                }

                default:
                    break;
            }

            if (shouldComputePixelSelection)
                getPropByName<SelectionToolProp>("SelectionTool")->compute();

            break;
        }

        case QEvent::Wheel:
        {
            auto wheelEvent = static_cast<QWheelEvent*>(event);

            switch (_selectionType)
            {
                case SelectionType::None:
                case SelectionType::Rectangle:
                    break;

                case SelectionType::Brush:
                {
                    if (wheelEvent->delta() < 0)
                        setBrushRadius(_brushRadius - 5.0f);
                    else
                        setBrushRadius(_brushRadius + 5.0f);

                    break;
                }

                case SelectionType::Lasso:
                case SelectionType::Polygon:
                case SelectionType::Sample:
                    break;

                default:
                    break;
            }

            affectedIds << index.siblingAtColumn(ult(Column::BrushRadius));

            break;
        }

        case QEvent::KeyPress:
        {
            auto keyEvent = static_cast<QKeyEvent*>(event);

            switch (keyEvent->key())
            {
                case Qt::Key::Key_R:
                    setPixelSelectionType(SelectionType::Rectangle);
                    break;

                case Qt::Key::Key_B:
                    setPixelSelectionType(SelectionType::Brush);
                    break;

                case Qt::Key::Key_P:
                    setPixelSelectionType(SelectionType::Polygon);
                    break;

                case Qt::Key::Key_L:
                    setPixelSelectionType(SelectionType::Lasso);
                    break;

                case Qt::Key::Key_S:
                    setPixelSelectionType(SelectionType::Sample);
                    break;

                case Qt::Key::Key_A:
                {
                    selectAll();
                    break;
                }

                case Qt::Key::Key_D:
                {
                    selectNone();
                    break;
                }

                case Qt::Key::Key_I:
                {
                    invertSelection();
                    break;
                }

                case Qt::Key::Key_Z:
                {
                    zoomToSelection();
                    break;
                }

                case Qt::Key::Key_Shift:
                {
                    setPixelSelectionModifier(SelectionModifier::Add);
                    break;
                }

                case Qt::Key::Key_Control:
                {
                    setPixelSelectionModifier(SelectionModifier::Remove);
                    break;
                }

                case Qt::Key::Key_Escape:
                {
                    switch (_selectionType)
                    {
                        case SelectionType::None:
                        case SelectionType::Rectangle:
                        case SelectionType::Brush:
                            break;

                        case SelectionType::Lasso:
                        case SelectionType::Polygon:
                            _mousePositions.clear();
                            break;

                        default:
                            break;
                    }

                    break;
                }

                default:
                    break;
            }

            switch (keyEvent->key())
            {
                case Qt::Key::Key_R: {
                    affectedIds << index.siblingAtColumn(ult(Column::PixelSelectionType));
                    break;
                }

                case Qt::Key::Key_B: {
                    affectedIds << index.siblingAtColumn(ult(Column::BrushRadius));
                    affectedIds << index.siblingAtColumn(ult(Column::PixelSelectionType));
                    break;
                }

                case Qt::Key::Key_L: {
                    affectedIds << index.siblingAtColumn(ult(Column::PixelSelectionType));
                    break;
                }

                case Qt::Key::Key_P: {
                    affectedIds << index.siblingAtColumn(ult(Column::PixelSelectionType));
                    break;
                }

                case Qt::Key::Key_S: {
                    affectedIds << index.siblingAtColumn(ult(Column::PixelSelectionType));
                    break;
                }

                case Qt::Key::Key_Shift:
                case Qt::Key::Key_Control:
                    affectedIds << index.siblingAtColumn(ult(Column::PixelSelectionModifier));
                    break;

                default:
                    break;
            }

            break;
        }

        case QEvent::KeyRelease:
        {
            auto keyEvent = static_cast<QKeyEvent*>(event);

            switch (keyEvent->key())
            {
                case Qt::Key::Key_R:
                case Qt::Key::Key_B:
                case Qt::Key::Key_L:
                case Qt::Key::Key_P:
                case Qt::Key::Key_A:
                case Qt::Key::Key_D:
                case Qt::Key::Key_I:
                case Qt::Key::Key_Z:
                    break;

                case Qt::Key::Key_Shift:
                case Qt::Key::Key_Control:
                {
                    setPixelSelectionModifier(SelectionModifier::Replace);

                    affectedIds << index.siblingAtColumn(ult(Column::PixelSelectionModifier));
                    break;
                }

                default:
                    break;
            }

            break;
        }

        default:
            break;
    }

    for (auto index : affectedIds)
        emit Layer::imageViewerPlugin->getLayersModel().dataChanged(index, index);

    Renderable::renderer->render();
}

void SelectionLayer::subsetFromSelectedPixels()
{
    auto core = imageViewerPlugin->core();

    const auto& selection = static_cast<Points&>(_pointsDataset->getSelection());

    core->createSubsetFromSelection(selection, *_pointsDataset, QString("%1_subset").arg(_pointsDataset->getName()));

    selectNone();
}

void SelectionLayer::subsetFromSelectionBounds()
{
    /*
    auto core = imageViewerPlugin->core();

    const auto dataName = _pointsDataset->getDataName();

    const hdps::DataSet& selection = core->requestSelection(dataName);

    core->createSubsetFromSelection(selection, dataName, QString("%1_subset").arg(_pointsDataset->getName()));

    selectNone();
    */
}

QSize SelectionLayer::getImageSize() const
{
    if (_pointsDataset == nullptr)
        return QSize();

    if (_pointsDataset->isDerivedData()) {
        auto sourcePointsDataset = hdps::DataSet::getSourceData<Points>(*_pointsDataset);

        return sourcePointsDataset.getProperty("ImageSize", QSize()).toSize();
    }

    return _pointsDataset->getProperty("ImageSize", QSize()).toSize();
}

Qt::ItemFlags SelectionLayer::getFlags(const QModelIndex& index) const
{
    auto flags = Layer::getFlags(index);

    const auto selectionSize = static_cast<Points&>(_pointsDataset->getSelection()).indices.size();

    switch (static_cast<Column>(index.column())) {
        case Column::PixelSelectionType:
        {
            flags |= Qt::ItemIsEditable;
            break;
        }

        case Column::PixelSelectionModifier:
        {
            flags |= Qt::ItemIsEditable;
            break;
        }

        case Column::BrushRadius:
        {
            if (_selectionType == SelectionType::Brush)
                flags |= Qt::ItemIsEditable;

            break;
        }

        case Column::SelectAll:
        {
            if (selectionSize < getNoPixels())
                flags |= Qt::ItemIsEditable;

            break;
        }

        case Column::SelectNone:
        {
            if (selectionSize > 0)
                flags |= Qt::ItemIsEditable;

            break;
        }

        case Column::InvertSelection:
        {
            flags |= Qt::ItemIsEditable;

            break;
        }

        case Column::AutoZoomToSelection:
            flags |= Qt::ItemIsEditable;
            break;

        case Column::ZoomToSelection:
        case Column::CreateSubset:
        {
            if (!_autoZoomToSelection && selectionSize > 0)
                flags |= Qt::ItemIsEditable;

            break;
        }

        case Column::OverlayColor:
        {
            flags |= Qt::ItemIsEditable;
            break;
        }

        default:
            break;
    }

    return flags;
}

QVariant SelectionLayer::getData(const QModelIndex& index, const int& role) const
{
    if (index.column() < ult(Column::Start))
        return Layer::getData(index, role);

    switch (static_cast<Column>(index.column())) {
        case Column::PixelSelectionType:
            return getPixelSelectionType(role);

        case Column::PixelSelectionModifier:
            return getSelectionModifier(role);

        case Column::BrushRadius:
            return getBrushRadius(role);

        case Column::SelectAll:
        case Column::SelectNone:
        case Column::InvertSelection:
            break;

        case Column::AutoZoomToSelection:
            return getAutoZoomToSelection(role);

        case Column::ZoomToSelection:
        case Column::CreateSubset:
            break;

        case Column::OverlayColor:
            return getOverlayColor(role);

        default:
            break;
    }

    return QVariant();
}

QModelIndexList SelectionLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
    QModelIndexList affectedIds = Layer::setData(index, value, role);

    switch (static_cast<Column>(index.column())) {
        case Column::PixelSelectionType:
        {
            setPixelSelectionType(static_cast<SelectionType>(value.toInt()));

            affectedIds << index.siblingAtColumn(ult(Column::BrushRadius));
            break;
        }

        case Column::PixelSelectionModifier:
        {
            setPixelSelectionModifier(static_cast<SelectionModifier>(value.toInt()));
            break;
        }

        case Column::BrushRadius:
        {
            setBrushRadius(value.toFloat());
            break;
        }

        case Column::SelectAll:
        {
            selectAll();
            break;
        }

        case Column::SelectNone:
        {
            selectNone();
            break;
        }

        case Column::InvertSelection:
        {
            invertSelection();
            break;
        }

        case Column::OverlayColor:
        {
            setOverlayColor(value.value<QColor>());
            break;
        }

        case Column::AutoZoomToSelection:
        {
            setAutoZoomToSelection(value.toBool());

            affectedIds << index.siblingAtColumn(ult(Column::ZoomToSelection));
            break;
        }

        case Column::ZoomToSelection:
        case Column::CreateSubset:
            break;

        default:
            break;
    }

    return affectedIds;
}

Layer::Hints SelectionLayer::getHints() const
{
    auto result = Layer::getHints();

    result << Hints({
        Hint(),
        Hint("R", "Rectangle", _selectionType == SelectionType::Rectangle),
        Hint("B", "Brush", _selectionType == SelectionType::Brush),
        Hint("P", "Polygon", _selectionType == SelectionType::Polygon),
        Hint("L", "Lasso", _selectionType == SelectionType::Lasso),
        Hint("S", "Sample", _selectionType == SelectionType::Sample),
        Hint(),
        Hint("A", "Select all pixels"),
        Hint("D", "Deselect all pixels"),
        Hint("I", "Invert selection"),
        Hint(),
        Hint("Shift", "Add pixels to selection", _selectionModifier == SelectionModifier::Add),
        Hint("Ctrl", "Remove pixels from selection", _selectionModifier == SelectionModifier::Remove),
        Hint(),
        });

    switch (_selectionType)
    {
        case SelectionType::None:
            break;

        case SelectionType::Rectangle:
        {
            result << Hints({
                Hint("LMB down", "Start rectangle selection"),
                Hint("LMB up", "Finish selection")
                });

            break;
        }

        case SelectionType::Brush:
        {
            result << Hints({
                Hint("LMB down", "Paint using the brush"),
                Hint("Scroll up", "Increase brush radius"),
                Hint("Scroll down", "Decrease brush radius")
                });

            break;
        }

        case SelectionType::Lasso:
        {
            result << Hints({
                Hint("LMB down", "Draw selection perimeter"),
                Hint("LMB up", "Finish selection")
                });

            break;
        }

        case SelectionType::Polygon:
        {
            result << Hints({
                Hint("LMB", "Add polygon control point"),
                Hint("RMB", "Finish selection")
                });

            break;
        }

        default:
            break;
    }

    return result;
}

QVariant SelectionLayer::getOverlayColor(const int& role) const
{
    const auto overlayColorString = QString("rgb(%1, %2, %3)").arg(QString::number(_overlayColor.red()), QString::number(_overlayColor.green()), QString::number(_overlayColor.blue()));

    switch (role)
    {
        case Qt::DisplayRole:
            return overlayColorString;

        case Qt::EditRole:
            return _overlayColor;

        case Qt::ToolTipRole:
            return QString("Overlay color: %1").arg(overlayColorString);

        default:
            break;
    }

    return QVariant();
}

void SelectionLayer::setOverlayColor(const QColor& overlayColor)
{
    _overlayColor = overlayColor;
}

QVariant SelectionLayer::getPixelSelectionType(const int& role) const
{
    const auto pixelSelectionTypeString = selectionTypeName(_selectionType);

    switch (role)
    {
        case Qt::DisplayRole:
            return pixelSelectionTypeString;

        case Qt::EditRole:
            return ult(_selectionType);

        case Qt::ToolTipRole:
            return QString("Pixel selection type: %1").arg(pixelSelectionTypeString);

        default:
            break;
    }

    return QVariant();
}

void SelectionLayer::setPixelSelectionType(const SelectionType& pixelSelectionType)
{
    _selectionType = static_cast<SelectionType>(pixelSelectionType);

    _mousePositions.clear();
}

QVariant SelectionLayer::getSelectionModifier(const int& role) const
{
    const auto pixelSelectionModifierString = selectionModifierName(_selectionModifier);

    switch (role)
    {
        case Qt::DisplayRole:
            return pixelSelectionModifierString;

        case Qt::EditRole:
            return ult(_selectionModifier);

        case Qt::ToolTipRole:
            return QString("Pixel selection modifier: %1").arg(pixelSelectionModifierString);

        default:
            break;
    }

    return QVariant();
}

void SelectionLayer::setPixelSelectionModifier(const SelectionModifier& pixelSelectionModifier)
{
    _selectionModifier = pixelSelectionModifier;
}

QVariant SelectionLayer::getBrushRadius(const int& role) const
{
    const auto brushRadiusString = QString::number(_brushRadius, 'f', 1);

    switch (role)
    {
        case Qt::DisplayRole:
            return brushRadiusString;

        case Qt::EditRole:
            return _brushRadius;

        case Qt::ToolTipRole:
            return QString("Brush radius: %1").arg(brushRadiusString);

        default:
            break;
    }

    return QVariant();
}

void SelectionLayer::setBrushRadius(const float& brushRadius)
{
    _brushRadius = std::max(0.1f, brushRadius);
}

QVariant SelectionLayer::getAutoZoomToSelection(const int& role) const
{
    const auto autoZoomToSelectionString = _autoZoomToSelection ? "true" : "false";

    switch (role)
    {
        case Qt::DisplayRole:
            return autoZoomToSelectionString;

        case Qt::EditRole:
            return _autoZoomToSelection;

        case Qt::ToolTipRole:
            return QString("Auto zoom to selection: %1").arg(autoZoomToSelectionString);

        default:
            break;
    }

    return QVariant();
}

void SelectionLayer::setAutoZoomToSelection(const bool& autoZoomToSelection)
{
    _autoZoomToSelection = autoZoomToSelection;
}

void SelectionLayer::computeChannel(const ChannelIndex& channelIndex)
{
    if (_pointsDataset == nullptr)
        return;

#ifdef _DEBUG
    auto timer = Timer("Compute channel");
#endif

    switch (channelIndex)
    {
        case ChannelIndex::Selection:
        {
            auto& selectionChannel = (*getChannel(ult(ChannelIndex::Selection)));

            selectionChannel.setImageSize(getImageSize());
            selectionChannel.fill(0);

            for (auto selectionIndex : getSelectionIndices())
                selectionChannel[selectionIndex] = 255;

            selectionChannel.setChanged();

            emit channelChanged(ult(ChannelIndex::Selection));
            
            break;
        }

        default:
            break;
    }
}

Points& SelectionLayer::getSelection()
{
    return static_cast<Points&>(_pointsDataset->getSelection());
}

std::vector<std::uint32_t>& SelectionLayer::getSelectionIndices()
{
    return getSelection().indices;
}

std::uint32_t SelectionLayer::getSelectionSize() const
{
    return static_cast<std::uint32_t>(const_cast<SelectionLayer*>(this)->getSelectionIndices().size());
}

bool SelectionLayer::hasSelection() const
{
    return getSelectionSize() > 0;
}

void SelectionLayer::selectAll()
{
#ifdef _DEBUG
    auto timer = Timer("Select all");
#endif

    auto& selectionIndices = getSelectionIndices();

    selectionIndices.clear();
    selectionIndices.reserve(getNoPixels());

    for (const auto& index : getSelectionIndices()) {
        if (index > 0)
            selectionIndices.push_back(index);
    }

    if (_pointsDataset->isDerivedData())
        imageViewerPlugin->core()->notifySelectionChanged(_pointsDataset->getSourceData<Points>(*_pointsDataset).getName());
    else
        imageViewerPlugin->core()->notifySelectionChanged(_pointsDataset->getName());
}

void SelectionLayer::selectNone()
{
#ifdef _DEBUG
    auto timer = Timer("Select none");
#endif

    auto& selectionIndices = getSelectionIndices();

    selectionIndices.clear();

    if (_pointsDataset->isDerivedData())
        imageViewerPlugin->core()->notifySelectionChanged(_pointsDataset->getSourceData<Points>(*_pointsDataset).getName());
    else
        imageViewerPlugin->core()->notifySelectionChanged(_pointsDataset->getName());
}

void SelectionLayer::invertSelection()
{
#ifdef _DEBUG
    auto timer = Timer("Invert selection");
#endif

    auto& selectionIndices = getSelectionIndices();

    std::set<std::uint32_t> selectionSet(selectionIndices.begin(), selectionIndices.end());

    const auto noPixels = getNoPixels();

    selectionIndices.clear();
    selectionIndices.reserve(noPixels - selectionSet.size());

    for (int i = 0; i < noPixels; i++) {
        if (selectionSet.find(i) == selectionSet.end())
            selectionIndices.push_back(i);
    }

    if (_pointsDataset->isDerivedData())
        imageViewerPlugin->core()->notifySelectionChanged(_pointsDataset->getSourceData<Points>(*_pointsDataset).getName());
    else
        imageViewerPlugin->core()->notifySelectionChanged(_pointsDataset->getName());
}

void SelectionLayer::zoomToSelection()
{
}

void SelectionLayer::publishSelection()
{
    auto timer = Timer("Publish selection");

    auto& selectionIndices = getSelectionIndices();

    switch (_selectionType)
    {
        case SelectionType::None:
            break;

        case SelectionType::Rectangle:
        case SelectionType::Brush:
        case SelectionType::Lasso:
        case SelectionType::Polygon: {
            const auto selectionImage = getPropByName<SelectionToolProp>("SelectionTool")->getSelectionImage().mirrored(false, true);

            auto& sourceIndices = _pointsDataset->getSourceData<Points>(*_pointsDataset).indices;

            const auto noComponents = 4;
            const auto width = static_cast<float>(getImageSize().width());
            const auto noPixels = this->getNoPixels();

            switch (_selectionModifier)
            {
                case SelectionModifier::Replace:
                {
                    selectionIndices.clear();
                    selectionIndices.reserve(noPixels);

                    for (std::uint32_t pixelIndex = 0; pixelIndex < noPixels; ++pixelIndex) {
                        if (selectionImage.bits()[pixelIndex * noComponents] > 0)
                            selectionIndices.push_back(pixelIndex);
                    }

                    break;
                }

                case SelectionModifier::Add:
                {
                    auto selectionSet = std::set<std::uint32_t>(selectionIndices.begin(), selectionIndices.end());

                    for (std::int32_t pixelIndex = 0; pixelIndex < noPixels; ++pixelIndex) {
                        if (selectionImage.bits()[pixelIndex * noComponents] > 0) {
                            selectionSet.insert(pixelIndex);
                        }
                    }

                    selectionIndices = std::vector<std::uint32_t>(selectionSet.begin(), selectionSet.end());
                    break;
                }

                case SelectionModifier::Remove:
                {
                    auto selectionSet = std::set<std::uint32_t>(selectionIndices.begin(), selectionIndices.end());

                    for (std::int32_t pixelIndex = 0; pixelIndex < noPixels; ++pixelIndex) {
                        if (selectionImage.bits()[pixelIndex * noComponents] > 0) {
                            selectionSet.erase(pixelIndex);
                        }
                    }

                    selectionIndices = std::vector<std::uint32_t>(selectionSet.begin(), selectionSet.end());
                    break;
                }

                case SelectionModifier::All:
                case SelectionModifier::None:
                case SelectionModifier::Invert:
                    break;

                default:
                    break;
            }

            break;
        }

        case SelectionType::Sample: {
            /*
            if (isWithin(_mousePositions.last())) {
                const auto textureCoordinate    = getTextureCoordinateFromScreenPoint(_mousePositions.last());
                const auto imageWidth           = getImageWidth(Qt::EditRole).toInt();
                const auto pointIndex           = textureCoordinate.y() * imageWidth + textureCoordinate.x();

                selectionIndices = std::vector<std::uint32_t>({ static_cast<std::uint32_t>(pointIndex) });
                //qDebug() << _mousePositions.last();
                //qDebug() << textureCoordinate;
            }
            */

            break;
        }

        default:
            break;
    }

    if (_pointsDataset->isDerivedData())
        imageViewerPlugin->core()->notifySelectionChanged(_pointsDataset->getSourceData<Points>(*_pointsDataset).getName());
    else
        imageViewerPlugin->core()->notifySelectionChanged(_pointsDataset->getName());

    getPropByName<SelectionToolProp>("SelectionTool")->reset();
}

void SelectionLayer::computeSelectionBounds()
{
    /*
#ifdef _DEBUG
    auto timer = Timer("Compute selection bounds");
#endif

    auto& selectionIndices = static_cast<Points&>(_pointsDataset->getSelection()).indices;

    if (selectionIndices.empty()) {
        _selectionBounds = QRect();
        return;
    }

    auto& selectionChannel = (*getChannel(ult(ChannelIndex::Selection)));

    const auto width = static_cast<float>(getImageSize().width());
    const auto height = static_cast<float>(getImageSize().height());

    std::vector<uint32_t> xCoordinates;
    std::vector<uint32_t> yCoordinates;

    xCoordinates.reserve(selectionIndices.size());
    yCoordinates.reserve(selectionIndices.size());

    for (auto selectionIndex : selectionIndices) {
        const int y = height - static_cast<std::int32_t>(floorf(selectionIndex / width));
        const int x = width - (selectionIndex % getImageSize().width());

        xCoordinates.push_back(x);
        yCoordinates.push_back(y);
    }

    QVector3D topLeft(*std::min_element(xCoordinates.begin(), xCoordinates.end()), *std::min_element(yCoordinates.begin(), yCoordinates.end()), 0.0f);
    QVector3D bottomRight(*std::max_element(xCoordinates.begin(), xCoordinates.end()), *std::max_element(yCoordinates.begin(), yCoordinates.end()), 0.0f);

    topLeft -= QVector3D(1.0f, 1.0f, 0.0f);

    const auto modelMatrix = getPropByName<SelectionProp>("Selection")->getModelMatrix();

    _selectionBounds = QRect(renderer->getWorldPositionToScreenPoint(modelMatrix * topLeft), renderer->getWorldPositionToScreenPoint(modelMatrix * bottomRight));
    */
}