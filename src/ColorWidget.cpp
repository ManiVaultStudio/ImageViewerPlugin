#include "ColorWidget.h"

#include "ui_ColorWidget.h"

#include <QDebug>
#include <QPainter>
#include <QLayout>

ColorWidget::ColorWidget(QWidget* parent) :
	QWidget(parent),
	_ui{ std::make_unique<Ui::ColorWidget>() },
	_color(Qt::yellow),
	_iconSize(60, 12)
{
	_ui->setupUi(this);
	/*
	setStyleSheet("QPushButton { text-align: left; vertical-align: top; padding-left: 5px; padding-top: 3px; padding-bottom: 3px; }");
	update();
	*/
}

void ColorWidget::update()
{
	updateIcon();
	updateText();
}

void ColorWidget::updateIcon()
{
	/*
	auto iconPixmap = QPixmap(_iconSize);
	auto painter	= QPainter(&iconPixmap);

	iconPixmap.fill(_color);

	painter.setPen(QPen(QBrush(QColor(30, 30, 30)), 1.5f));

	QPointF points[5] = {
		QPointF(0.0f, 0.0f),
		QPointF(_iconSize.width(), 0.0f),
		QPointF(_iconSize.width(), _iconSize.height()),
		QPointF(0.0f, _iconSize.height()),
		QPointF(0.0f, 0.0f)
	};

	painter.drawPolyline(points, 5);

	setIcon(iconPixmap);
	setIconSize(_iconSize);
	*/
}

void ColorWidget::updateText()
{
	/*
	setText(QString("(%1, %2, %3)").arg(QString::number(_color.red()), QString::number(_color.green()), QString::number(_color.blue())));
	*/
}