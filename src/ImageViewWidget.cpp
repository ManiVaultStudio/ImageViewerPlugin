#include <QtWidgets>
#include <QGraphicsProxyWidget>
#include <QMenu>

#include "ImageViewWidget.h"
#include "ImageViewerPlugin.h"
#include "ImageGraphicsView.h"
#include "ImageWidget.h"

ImageViewWidget::ImageViewWidget(ImageViewerPlugin* imageViewerPlugin) :
	_imageGraphicsView(nullptr)
{
	QVBoxLayout *mainLayout = new QVBoxLayout;

	mainLayout->setSpacing(0);
	mainLayout->setMargin(0);

	_imageGraphicsView = new ImageGraphicsView(imageViewerPlugin);
	
	mainLayout->addWidget(_imageGraphicsView);
	setLayout(mainLayout);

	setWindowTitle(tr("GraphicsView + OpenGL Test Unit"));
}