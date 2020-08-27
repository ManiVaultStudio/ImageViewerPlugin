#include "Layer.h"
#include "ImageViewerPlugin.h"
#include "Renderer.h"

#include "ImageData/Images.h"
#include "PointData.h"

#include <QFont>
#include <QDebug>
#include <QPainter>
#include <QTextDocument>

ImageViewerPlugin* Layer::imageViewerPlugin	= nullptr;
const QColor Layer::hintsColor				= QColor(255, 174, 66, 200);
const qreal Layer::textMargins				= 10.0;

Layer::Layer(const QString& datasetName, const Type& type, const QString& id, const QString& name, const int& flags) :
	Node(id, name, flags),
	_datasetName(datasetName),
	_dataName(),
	_type(type),
	_selection(),
	_mousePositions(),
	_mouseButtons(),
	_keys()
{
}

Layer::~Layer() = default;

void Layer::matchScaling(const QSize& targetImageSize)
{
	const auto layerImageSize	= imageSize();
	const auto widthScaling		= static_cast<float>(targetImageSize.width()) / layerImageSize.width();
	const auto heightScaling	= static_cast<float>(targetImageSize.height()) / layerImageSize.height();

	const auto scale = std::min(widthScaling, heightScaling);

	setScale(scale);
}

void Layer::paint(QPainter* painter)
{
	drawTitle(painter);
	drawHints(painter);
}

void Layer::zoomExtents()
{
	renderer->zoomToRectangle(boundingRectangle());
}

Qt::ItemFlags Layer::flags(const QModelIndex& index) const
{
	Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;

	const auto type = static_cast<Type>(_type);

	switch (static_cast<Column>(index.column())) {
		case Column::Name:
		{
			flags |= Qt::ItemIsUserCheckable;

			if (flag(Layer::Flag::Renamable, Qt::EditRole).toBool())
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::DatasetName:
		case Column::DataName:
		case Column::Type:
		case Column::ID:
		case Column::ImageSize:
		case Column::ImageWidth:
		case Column::ImageHeight:
			break;

		case Column::Opacity:
			flags |= Qt::ItemIsEditable;
			break;

		case Column::Scale:
			flags |= Qt::ItemIsEditable;
			break;

		case Column::Flags:
			break;

		case Column::Selection:
		case Column::SelectionSize:
			break;

		default:
			break;
	}

	return flags;
}

QVariant Layer::data(const QModelIndex& index, const int& role) const
{
	switch (static_cast<Column>(index.column())) {
		case Column::Name:
			return name(role);

		case Column::DatasetName:
			return datasetName(role);

		case Column::DataName:
			return dataName(role);

		case Column::Type:
			return type(role);

		case Column::ID:
			return id(role);

		case Column::ImageSize:
			return imageSize(role);

		case Column::ImageWidth:
			return imageWidth(role);

		case Column::ImageHeight:
			return imageHeight(role);

		case Column::Opacity:
			return opacity(role);

		case Column::Scale:
			return scale(role);

		case Column::Flags:
			return Node::flags(role);

		case Column::Selection:
			return selection(role);

		case Column::SelectionSize:
			return selectionSize(role);

		default:
			break;
	}

	return QVariant();
}

QModelIndexList Layer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	QModelIndexList affectedIndices{index};

	const auto column = static_cast<Column>(index.column());

	switch (role)
	{
		case Qt::CheckStateRole:
		{
			switch (column) {
				case Column::Name:
				{
					setFlag(Layer::Flag::Enabled, value.toBool());

					for (int column = ult(Column::Type); column <= ult(Column::End); ++column) {
						affectedIndices << index.siblingAtColumn(column);
					}

					auto parent = index.parent();

					while (parent.isValid()) {
						affectedIndices << parent;
						parent = parent.parent();
					}
					
					break;
				}

				default:
					break;
			}

			break;
		}

		case Qt::EditRole:
		{
			switch (column) {
				case Column::Name:
					setName(value.toString());
					break;

				case Column::DatasetName:
				case Column::DataName:
					break;

				case Column::Type:
					setType(static_cast<Type>(value.toInt()));
					break;

				case Column::ID:
					setId(value.toString());
					break;

				case Column::ImageSize:
				case Column::ImageWidth:
				case Column::ImageHeight:
					break;

				case Column::Opacity:
					setOpacity(value.toFloat());
					break;

				case Column::Scale:
					setScale(value.toFloat());
					break;

				case Column::Flags:
					setFlags(value.toInt());
					break;

				case Column::Selection:
					setSelection(value.value<Indices>());
					affectedIndices << index.siblingAtColumn(ult(Column::SelectionSize));
					break;

				case Column::SelectionSize:
					break;

				default:
					break;
			}

			break;
		}

		default:
			break;
	}

	Renderable::renderer->render();

	return affectedIndices;
}

QVariant Layer::datasetName(const int& role) const
{
	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return _datasetName;

		case Qt::ToolTipRole:
			return QString("Dataset name: %1").arg(_datasetName);

		default:
			break;
	}

	return QVariant();
}

QVariant Layer::dataName(const int& role) const
{
	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return _dataName;

		case Qt::ToolTipRole:
			return QString("Data name: %1").arg(_dataName);

		default:
			break;
	}

	return QVariant();
}

QVariant Layer::type(const int& role) const
{
	const auto typeName = Layer::typeName(_type);

	switch (role)
	{
		case Qt::FontRole:
			return QFont("Font Awesome 5 Free Solid", 9);

		case Qt::EditRole:
			return static_cast<int>(_type);

		case Qt::ToolTipRole:
			return QString("Type: %1").arg(typeName);

		case Qt::DisplayRole:
		{
			switch (_type) {
				case Type::Selection:
					return u8"\uf245";

				case Type::Points:
					return u8"\uf58d";

				default:
					break;
			}

			break;
		}

		default:
			break;
	}

	return QVariant();
}

void Layer::setType(const Type& type)
{
	_type = type;
}

QVariant Layer::imageSize(const int& role /*= Qt::DisplayRole*/) const
{
	const auto imageSize		= this->imageSize();
	const auto imageSizeString	= QString::number(imageSize.width()) + " x" + QString::number(imageSize.height());

	switch (role)
	{
		case Qt::DisplayRole:
			return imageSizeString;

		case Qt::EditRole:
			return imageSize;

		case Qt::ToolTipRole:
			return QString("Image size: %1").arg(imageSizeString);

		default:
			break;
	}

	return QVariant();
}

QVariant Layer::imageWidth(const int& role) const
{
	const auto imageSize	= this->imageSize(Qt::EditRole).toSize();
	const auto widthString	= QString::number(imageSize.width());

	switch (role)
	{
		case Qt::DisplayRole:
			return widthString;

		case Qt::EditRole:
			return imageSize.width();

		case Qt::ToolTipRole:
			return QString("Image width: %1 pixels").arg(widthString);

		default:
			break;
	}

	return QVariant();
}

QVariant Layer::imageHeight(const int& role) const
{
	const auto imageSize	= this->imageSize(Qt::EditRole).toSize();
	const auto heightString = QString::number(imageSize.height());

	switch (role)
	{
		case Qt::DisplayRole:
			return heightString;

		case Qt::EditRole:
			return imageSize.height();

		case Qt::ToolTipRole:
			return QString("Image height: %1 pixels").arg(heightString);

		default:
			break;
	}

	return QVariant();
}

QVariant Layer::selection(const int& role /*= Qt::DisplayRole*/) const
{
	auto selection = QStringList();

	if (_selection.size() <= 2) {
		for (const auto& id : _selection)
			selection << QString::number(id);
	}
	else {
		selection << QString::number(_selection.first());
		selection << "...";
		selection << QString::number(_selection.last());
	}

	const auto selectionString = QString("[%1]").arg(selection.join(", "));

	switch (role)
	{
		case Qt::DisplayRole:
			return selectionString;

		case Qt::EditRole:
			return QVariant::fromValue(_selection);

		case Qt::ToolTipRole:
			return QString("Selection: %1").arg(selectionString);

		default:
			break;
	}

	return QVariant();
}

void Layer::setSelection(const Indices& selection)
{
	_selection = selection;
}

QVariant Layer::selectionSize(const int& role /*= Qt::DisplayRole*/) const
{
	const auto selectionSizeString = QString::number(_selection.size());

	switch (role)
	{
		case Qt::DisplayRole:
			return selectionSizeString;

		case Qt::EditRole:
			return _selection.size();

		case Qt::ToolTipRole:
		{
			return QString("Selection size: %1").arg(selectionSizeString);
		}

		default:
			break;
	}

	return QVariant();
}

QVariant Layer::keys(const int& role /*= Qt::DisplayRole*/) const
{
	const auto keysString = "";

	switch (role)
	{
		case Qt::DisplayRole:
			return keysString;

		case Qt::EditRole:
			return _keys;

		case Qt::ToolTipRole:
		{
			return QString("Keys: %1").arg(keysString);
		}

		default:
			break;
	}

	return QVariant();
}

void Layer::setKeys(const int& keys)
{
	_keys = keys;
}

QVector<QPoint> Layer::mousePositions() const
{
	return _mousePositions;
}

int Layer::noPixels() const
{
	return imageSize().width() * imageSize().height();
}

Layer::Hints Layer::hints() const
{
	return Hints({
		Hint(),
		Hint(),
		Hint(),
		Hint("Space + Scroll up", "Zoom in"),
		Hint("Space + Scroll down", "Zoom out"),
		Hint("Space + Move mouse", "Pan view")
	});
}

void Layer::drawTitle(QPainter* painter)
{
	QTextDocument titleDocument;

	const auto color = QString("rgba(%1, %2, %3, %4)").arg(QString::number(hintsColor.red()), QString::number(hintsColor.green()), QString::number(hintsColor.blue()), QString::number(isFlagSet(Flag::Enabled) ? hintsColor.alpha() : 80));

	QString titleHtml = QString("<div style='width: 100%; text-align: center; color: %1; font-weight: bold;'>%2 (%3x%4)<div>").arg(color, _name, QString::number(imageSize().width()), QString::number(imageSize().height()));
	
	titleDocument.setTextWidth(painter->viewport().width());
	titleDocument.setDocumentMargin(textMargins);
	titleDocument.setHtml(titleHtml);
	titleDocument.drawContents(painter, painter->viewport());
}

void Layer::drawHints(QPainter* painter)
{
	QTextDocument hintsDocument;

	QString hintsHtml;

	const auto color = QString("rgba(%1, %2, %3, %4)").arg(QString::number(hintsColor.red()), QString::number(hintsColor.green()), QString::number(hintsColor.blue()), QString::number(isFlagSet(Flag::Enabled) ? hintsColor.alpha() : 80));

	hintsHtml += QString("<div style='height: 100%'><table style='color: %1;'>").arg(color);

	for (auto hint : hints()) {
		if (hint.title().isEmpty())
			hintsHtml += "<tr><td></td><td></td></tr>";
		else
			hintsHtml += QString("<tr style='font-weight: %1'><td width=120>%2</td><td>: %3</td></tr>").arg(hint.isActive() ? "bold" : "normal", hint.title(), hint.description());
	}

	hintsHtml += "</table></div>";

	hintsDocument.setTextWidth(painter->viewport().width());
	hintsDocument.setDocumentMargin(textMargins);
	hintsDocument.setHtml(hintsHtml);
	hintsDocument.drawContents(painter);
}