#include "ToolBarAction.h"
#include "ImageViewerPlugin.h"
#include "Application.h"

#include <QHBoxLayout>

using namespace hdps;

ToolBarAction::ToolBarAction(ImageViewerPlugin& imageViewerPlugin) :
    WidgetAction(&imageViewerPlugin),
    _imageViewerPlugin(imageViewerPlugin),
    _panAction(this, "Pan"),
    _zoomAction(this, "Zoom"),
    _zoomAreaAction(this, "Zoom area"),
    _zoomExtentsAction(this, "Zoom extents")
{
    setText("Toolbar");

    //_panAction.setIcon(Application::getIconFont("FontAwesome").getIcon("hand-paper"));
    //_zoomExtentsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("hand-paper"));
    //_zoomAction.setIcon(Application::getIconFont("FontAwesome").getIcon("search-plus"));
    //_zoomAreaAction.setIcon(Application::getIconFont("FontAwesome").getIcon("search-plus"));

    //targetWidget->addAction(&_rectangleAction);

    _zoomExtentsAction.setShortcut(QKeySequence("R"));

    _panAction.setToolTip("Move the view left/right/up/down");
    _zoomAction.setToolTip("Zoom in/out around the cursor");
    _zoomAreaAction.setToolTip("Zoom to area");
    _zoomExtentsAction.setToolTip("Zoom to the extents of all visible layers");

    connect(&_zoomExtentsAction, &TriggerAction::triggered, [this]() {
    });
}

ToolBarAction::Widget::Widget(QWidget* parent, ToolBarAction* toolBarAction, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, toolBarAction, state)
{
    setAutoFillBackground(true);

    auto layout = new QHBoxLayout();

    layout->setSpacing(3);
    layout->setMargin(4);

    layout->addWidget(toolBarAction->getPanAction().createPushButtonWidget(this));
    layout->addWidget(toolBarAction->getZoomAction().createPushButtonWidget(this));
    layout->addWidget(toolBarAction->getZoomAreaAction().createWidget(this));
    layout->addWidget(toolBarAction->getZoomExtentsAction().createWidget(this));
    layout->addStretch(1);

    setLayout(layout);
}
