#include "ViewerWidget.h"
#include "ImageViewerPlugin.h"
#include "Layer.h"

#include "ui_ViewerWidget.h"

#include <QDebug>

ViewerWidget::ViewerWidget(QWidget* parent) :
    QWidget(parent),
    _imageViewerPlugin(dynamic_cast<ImageViewerPlugin*>(parent)),
    _ui{ std::make_unique<Ui::ViewerWidget>() }
{
    setAcceptDrops(true);

    _ui->setupUi(this);
    _ui->canvasWidget->initialize(_imageViewerPlugin);

    setShowHints(_imageViewerPlugin->getSetting("ShowHints").toBool());
}

bool ViewerWidget::getShowHints() const
{
    return Layer::showHints;
}

void ViewerWidget::setShowHints(const bool& showHints)
{
    Layer::showHints = showHints;

    _imageViewerPlugin->setSetting("ShowHints", Layer::showHints);

    _ui->canvasWidget->update();
}

CanvasWidget* ViewerWidget::getCanvasWidget()
{
    return _ui->canvasWidget;
}

StatusbarWidget* ViewerWidget::getStatusbarWidget()
{
    return _ui->statusbarWidget;
}