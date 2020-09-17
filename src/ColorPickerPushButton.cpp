#include "ColorPickerPushButton.h"

#include <QDebug>
#include <QColorDialog>
#include <QPainter>

const QSize ColorPickerPushButton::_iconSize = QSize(12, 12);

ColorPickerPushButton::ColorPickerPushButton(QWidget* parent) :
	QPushButton(parent),
	_colorDialog(new QColorDialog(parent))
{
	setIconSize(_iconSize);
	this->setStyleSheet("text-align:left; margin: -1px; padding-left: 5px; padding-top: 4px; padding-bottom: 3px;");

	QObject::connect(this, &QPushButton::clicked, [this]() {
		QObject::connect(_colorDialog, &QColorDialog::currentColorChanged, [this](const QColor& color) {
			setCurrentColor(color);
		});

		_colorDialog->show();
	});
}

QColor ColorPickerPushButton::getCurrentColor() const
{
	return _colorDialog->currentColor();
}

void ColorPickerPushButton::setCurrentColor(const QColor& color)
{
	if (color != getCurrentColor())
		_colorDialog->setCurrentColor(color);

	auto icon = QPixmap(_iconSize);

	icon.fill(color);

	auto painter = QPainter(&icon);

	painter.setPen(QPen(QBrush(QColor(30, 30, 30)), 1.5f));

	QPointF points[5] = {
		QPointF(0.0f, 0.0f),
		QPointF(_iconSize.width(), 0.0f),
		QPointF(_iconSize.width(), _iconSize.height()),
		QPointF(0.0f, _iconSize.height()),
		QPointF(0.0f, 0.0f)
	};

	painter.drawPolyline(points, 5);

	setIcon(icon);
		
	const auto colorString = QString("rgb(%1, %2, %3)").arg(QString::number(color.red()), QString::number(color.green()), QString::number(color.blue()));

	setText(colorString);

	emit currentColorChanged(color);
}