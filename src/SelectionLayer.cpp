#include "SelectionLayer.h"
#include "ImageViewerPlugin.h"
#include "SelectionProp.h"
#include "Renderer.h"

#include "PointData.h"

#include <set>

#include <cmath>

#ifndef M_PI
	#define M_PI (3.14159265358979323846)
#endif

#include <QDebug>
#include <QPainter>

const QColor SelectionLayer::toolColorForeground	= QColor(255, 174, 66, 200);
const QColor SelectionLayer::toolColorBackground	= QColor(255, 174, 66, 150);
const float SelectionLayer::minBrushRadius			= 1.0f;
const float SelectionLayer::maxBrushRadius			= 1000.0f;
const float SelectionLayer::defaultBrushRadius		= 50.0f;
const float SelectionLayer::controlPointSize		= 6.0f;
const float SelectionLayer::perimeterLineWidth		= 1.5f;

SelectionLayer::SelectionLayer(const QString& datasetName, const QString& id, const QString& name, const int& flags) :
	Layer(datasetName, Layer::Type::Selection, id, name, flags),
	Channels<std::uint8_t>(1),
	_pointsDataset(nullptr),
	_imagesDataset(nullptr),
	_image(),
	_imageData(),
	_pixelSelectionType(SelectionType::Rectangle),
	_pixelSelectionModifier(SelectionModifier::Replace),
	_brushRadius(defaultBrushRadius),
	_overlayColor(Qt::green),
	_autoZoomToSelection(false)
{
	init();
}

void SelectionLayer::init()
{
	addProp<SelectionProp>(this, "Selection");

	_pointsDataset	= &imageViewerPlugin->requestData<Points>(_datasetName);
	_imagesDataset	= imageViewerPlugin->sourceImagesSetFromPointsSet(_datasetName);
	_dataName		= hdps::DataSet::getSourceData(*_pointsDataset).getDataName();

	computeImage();
}

void SelectionLayer::paint(QPainter* painter)
{
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

	QVector<QPoint> controlPoints;

	switch (_pixelSelectionType)
	{
		case SelectionType::None:
			break;

		case SelectionType::Rectangle:
		{
			if (_mousePositions.size() == 2) {
				painter->setPen(perimeterForegroundPen);
				painter->setBrush(Qt::NoBrush);

				const auto topLeft		= QPointF(std::min(_mousePositions.first().x(), _mousePositions.last().x()), std::min(_mousePositions.first().y(), _mousePositions.last().y()));
				const auto bottomRight	= QPointF(std::max(_mousePositions.first().x(), _mousePositions.last().x()), std::max(_mousePositions.first().y(), _mousePositions.last().y()));
				const auto rectangle	= QRectF(topLeft, bottomRight);

				painter->drawRect(rectangle);

				controlPoints << _mousePositions.first();
				controlPoints << _mousePositions.last();
				
				const auto size			= 8.0f;
				const auto textCenter	= rectangle.topRight() + QPoint(size, -size);
				
				textRectangle = QRectF(textCenter - QPointF(size, size), textCenter + QPointF(size, size));
			}

			break;
		}

		case SelectionType::Brush:
		{
			if (_mousePositions.size() == 1) {
				const auto brushCenter = _mousePositions.last();

				painter->setPen(Qt::NoPen);
				painter->setBrush(toolColorForeground);

				painter->drawPoint(brushCenter);

				painter->setPen(_mouseButtons & Qt::LeftButton ? perimeterForegroundPen : perimeterBackgroundPen);
				painter->setBrush(Qt::NoBrush);

				painter->drawEllipse(QPointF(brushCenter), _brushRadius, _brushRadius);

				controlPoints << _mousePositions.first();

				const auto textAngle		= 0.75f * M_PI;
				const auto size				= 12.0f;
				const auto textCenter		= brushCenter + (_brushRadius + size) * QPointF(sin(textAngle), cos(textAngle));
				
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

				const auto size			= 8.0f;
				const auto textCenter	= _mousePositions.first() - QPoint(size, size);

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

				const auto size			= 8.0f;
				const auto textCenter	= _mousePositions.first() - QPoint(size, size);

				textRectangle = QRectF(textCenter - QPointF(size, size), textCenter + QPointF(size, size));
			}

			break;
		}

		default:
			break;
	}

	painter->setPen(controlPointsPen);
	painter->drawPoints(controlPoints);

	//painter->setBrush(toolColorForeground);

	switch (_pixelSelectionType)
	{
		case SelectionType::None:
			break;

		case SelectionType::Rectangle:
		case SelectionType::Brush:
		case SelectionType::Lasso:
		case SelectionType::Polygon:
		{
			switch (_pixelSelectionModifier)
			{
				case SelectionModifier::Replace:
					break;

				case SelectionModifier::Add:
				{
					painter->setPen(toolColorForeground);
					painter->drawText(textRectangle, u8"\uf055", QTextOption(Qt::AlignCenter));
					break;
				}

				case SelectionModifier::Remove:
				{
					painter->setPen(toolColorForeground);
					painter->drawText(textRectangle, u8"\uf056", QTextOption(Qt::AlignCenter));
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

Qt::ItemFlags SelectionLayer::flags(const QModelIndex& index) const
{
	auto flags = Layer::flags(index);

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
			if (_pixelSelectionType == SelectionType::Brush)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::SelectAll:
		{
			if (_selection.count() < noPixels())
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::SelectNone:
		{
			if (_selection.count() > 0)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::InvertSelection:
		{
			if (_selection.count() > 0 && _selection.count() < noPixels())
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::AutoZoomToSelection:
			flags |= Qt::ItemIsEditable;
			break;

		case Column::ZoomToSelection:
		{
			if (!_autoZoomToSelection && _selection.count() > 0)
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

QVariant SelectionLayer::data(const QModelIndex& index, const int& role) const
{
	if (index.column() < ult(Column::Start))
		return Layer::data(index, role);

	switch (static_cast<Column>(index.column())) {
		case Column::PixelSelectionType:
			return pixelSelectionType(role);

		case Column::PixelSelectionModifier:
			return pixelSelectionModifier(role);

		case Column::BrushRadius:
			return brushRadius(role);

		case Column::OverlayColor:
			return overlayColor(role);

		case Column::AutoZoomToSelection:
			return autoZoomToSelection(role);

		default:
			break;
	}

	return QVariant();
}

QModelIndexList SelectionLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	QModelIndexList affectedIds = Layer::setData(index, value, role);

	if (static_cast<Layer::Column>(index.column()) == Layer::Column::Selection) {
		computeImage();

		affectedIds << index.siblingAtColumn(ult(Column::SelectAll));
		affectedIds << index.siblingAtColumn(ult(Column::SelectNone));
		affectedIds << index.siblingAtColumn(ult(Column::InvertSelection));
		affectedIds << index.siblingAtColumn(ult(Column::ZoomToSelection));
	}

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

		default:
			break;
	}
	
	return affectedIds;
}

void SelectionLayer::mousePressEvent(QMouseEvent* mouseEvent, const QModelIndex& index)
{
	Layer::mousePressEvent(mouseEvent, index);

	QModelIndexList affectedIds;

	_mouseButtons = mouseEvent->buttons();

	switch (_pixelSelectionType)
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
					_mousePositions << mouseEvent->pos();
					break;

				case Qt::RightButton:
					_mousePositions.clear();
					break;

				default:
					break;
			}

			break;
		}

		case SelectionType::Brush:
			break;

		default:
			break;
	}

	for (auto index : affectedIds)
		emit Layer::imageViewerPlugin->layersModel().dataChanged(index, index);

	Renderable::renderer->render();
}

void SelectionLayer::mouseReleaseEvent(QMouseEvent* mouseEvent, const QModelIndex& index)
{
	Layer::mousePressEvent(mouseEvent, index);

	QModelIndexList affectedIds;

	_mouseButtons = mouseEvent->buttons();

	switch (_pixelSelectionType)
	{
		case SelectionType::None:
			break;

		case SelectionType::Rectangle:
			_mousePositions << mouseEvent->pos();
			break;

		case SelectionType::Lasso:
		{
			_mousePositions.clear();
			break;
		}

		case SelectionType::Polygon:
			break;

		case SelectionType::Brush:
		{
			_mousePositions.clear();
			break;
		}

		default:
			break;
	}

	for (auto index : affectedIds)
		emit Layer::imageViewerPlugin->layersModel().dataChanged(index, index);

	Renderable::renderer->render();
}

void SelectionLayer::mouseMoveEvent(QMouseEvent* mouseEvent, const QModelIndex& index)
{
	Layer::mousePressEvent(mouseEvent, index);

	QModelIndexList affectedIds;

	switch (_pixelSelectionType)
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
			}

			break;
		}

		case SelectionType::Lasso:
		{
			if (mouseEvent->buttons() & Qt::LeftButton)
				_mousePositions << mouseEvent->pos();

			break;
		}

		case SelectionType::Polygon:
		{
			if (_mousePositions.isEmpty())
				_mousePositions << mouseEvent->pos();
			else
				_mousePositions.last() = mouseEvent->pos();

			break;
		}

		case SelectionType::Brush:
		{
			if (_mousePositions.isEmpty())
				_mousePositions << mouseEvent->pos();
			else
				_mousePositions.last() = mouseEvent->pos();

			break;
		}

		default:
			break;
	}

	for (auto index : affectedIds)
		emit Layer::imageViewerPlugin->layersModel().dataChanged(index, index);

	Renderable::renderer->render();
}

void SelectionLayer::mouseWheelEvent(QWheelEvent* wheelEvent, const QModelIndex& index)
{
	Layer::mouseWheelEvent(wheelEvent, index);

	QModelIndexList affectedIds;

	switch (_pixelSelectionType)
	{
		case SelectionType::None:
		case SelectionType::Rectangle:
		case SelectionType::Lasso:
		case SelectionType::Polygon:
			break;

		case SelectionType::Brush:
		{
			if (wheelEvent->delta() < 0)
				setBrushRadius(_brushRadius - 5.0f);
			else
				setBrushRadius(_brushRadius + 5.0f);

			break;
		}

		default:
			break;
	}

	affectedIds << index.siblingAtColumn(ult(Column::BrushRadius));

	for (auto index : affectedIds)
		emit Layer::imageViewerPlugin->layersModel().dataChanged(index, index);

	Renderable::renderer->render();
}

void SelectionLayer::keyPressEvent(QKeyEvent* keyEvent, const QModelIndex& index)
{
	Layer::keyPressEvent(keyEvent, index);

	QModelIndexList affectedIds;

	_keys |= keyEvent->key();

	switch (keyEvent->key())
	{
		case Qt::Key::Key_R:
			setPixelSelectionType(SelectionType::Rectangle);
			break;

		case Qt::Key::Key_B:
			setPixelSelectionType(SelectionType::Brush);
			break;

		case Qt::Key::Key_L:
			setPixelSelectionType(SelectionType::Lasso);
			break;

		case Qt::Key::Key_P:
			setPixelSelectionType(SelectionType::Polygon);
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
			switch (_pixelSelectionType)
			{
				case SelectionType::None:
				case SelectionType::Rectangle:
					break;

				case SelectionType::Lasso:
				case SelectionType::Polygon:
					_mousePositions.clear();
					break;

				case SelectionType::Brush:
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
		case Qt::Key::Key_R:
		case Qt::Key::Key_B:
		case Qt::Key::Key_L:
		case Qt::Key::Key_P:
			affectedIds << index.siblingAtColumn(ult(Column::PixelSelectionType));
			affectedIds << index.siblingAtColumn(ult(Column::BrushRadius));
			break;

		case Qt::Key::Key_A:
		case Qt::Key::Key_D:
		case Qt::Key::Key_I:
			affectedIds << index.siblingAtColumn(ult(Layer::Column::Selection));
			break;

		case Qt::Key::Key_Shift:
		case Qt::Key::Key_Control:
			affectedIds << index.siblingAtColumn(ult(Column::PixelSelectionModifier));
			break;

		default:
			break;
	}

	for (auto index : affectedIds)
		emit Layer::imageViewerPlugin->layersModel().dataChanged(index, index);

	Renderable::renderer->render();
}

void SelectionLayer::keyReleaseEvent(QKeyEvent* keyEvent, const QModelIndex& index)
{
	Layer::keyReleaseEvent(keyEvent, index);

	QModelIndexList affectedIds;

	_keys &= ~keyEvent->key();

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

	for (auto index : affectedIds)
		emit Layer::imageViewerPlugin->layersModel().dataChanged(index, index);

	Renderable::renderer->render();
}

QSize SelectionLayer::imageSize() const
{
	return _imagesDataset->imageSize();
}

QVariant SelectionLayer::overlayColor(const int& role) const
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

QVariant SelectionLayer::pixelSelectionType(const int& role) const
{
	const auto pixelSelectionTypeString = selectionTypeName(_pixelSelectionType);

	switch (role)
	{
		case Qt::DisplayRole:
			return pixelSelectionTypeString;

		case Qt::EditRole:
			return ult(_pixelSelectionType);

		case Qt::ToolTipRole:
			return QString("Pixel selection type: %1").arg(pixelSelectionTypeString);

		default:
			break;
	}

	return QVariant();
}

void SelectionLayer::setPixelSelectionType(const SelectionType& pixelSelectionType)
{
	_pixelSelectionType = static_cast<SelectionType>(pixelSelectionType);
	
	_mousePositions.clear();
}

QVariant SelectionLayer::pixelSelectionModifier(const int& role) const
{
	const auto pixelSelectionModifierString = selectionModifierName(_pixelSelectionModifier);

	switch (role)
	{
		case Qt::DisplayRole:
			return pixelSelectionModifierString;

		case Qt::EditRole:
			return ult(_pixelSelectionModifier);

		case Qt::ToolTipRole:
			return QString("Pixel selection modifier: %1").arg(pixelSelectionModifierString);

		default:
			break;
	}

	return QVariant();
}

void SelectionLayer::setPixelSelectionModifier(const SelectionModifier& pixelSelectionModifier)
{
	_pixelSelectionModifier = pixelSelectionModifier;
}

QVariant SelectionLayer::brushRadius(const int& role) const
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

QVariant SelectionLayer::autoZoomToSelection(const int& role) const
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

void SelectionLayer::computeImage()
{
	auto points = dynamic_cast<Points*>(&Layer::imageViewerPlugin->requestData<Points>(_datasetName));

	if (points == nullptr)
		return;

	const auto imageSize	= _imagesDataset->imageSize();
	const auto width		= imageSize.width();
	const auto height		= imageSize.height();
	const auto noPixels		= width * height;
	const auto noChannels	= 4;
	const auto noElements	= noPixels * noChannels;

	if (noElements !=_imageData.count()) {
		_imageData.resize(noElements);
	}

	_imageData.fill(0, noElements);

	for (const auto& selectionId : _selection)
	{
		const auto x		= selectionId % width;
		const auto y		= static_cast<std::uint32_t>(floorf(static_cast<float>(selectionId) / static_cast<float>(width)));
		const auto pixelId	= (y * width) + x;
		const auto offset	= pixelId * noChannels;

		for (int c = 0; c < noChannels; c++)
			_imageData[offset + c] = 255;
	}

	if (_image.isNull() || imageSize != _image.size())
		_image = QImage(width, height, QImage::Format_RGBA8888);

	memcpy(_image.bits(), _imageData.data(), _imageData.size() * sizeof(std::uint8_t));

	emit imageChanged(_image);
}

void SelectionLayer::selectAll()
{
	std::vector<std::uint32_t> indices(noPixels());
	std::iota(std::begin(indices), std::end(indices), 0);

	_imagesDataset->setIndices(indices);
}

void SelectionLayer::selectNone()
{
	_imagesDataset->setIndices(std::vector<std::uint32_t>());
}

void SelectionLayer::invertSelection()
{
	std::set<std::uint32_t> selectionSet(_selection.begin(), _selection.end());

	std::vector<std::uint32_t> indices;
	
	indices.reserve(noPixels());

	for (int index = 0; index < noPixels(); index++) {
		if (selectionSet.find(index) == selectionSet.end())
			indices.push_back(index);
	}

	qDebug() << indices.size();

	_imagesDataset->setIndices(indices);
}

void SelectionLayer::zoomToSelection()
{
}