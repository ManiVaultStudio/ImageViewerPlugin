#include "SelectionAction.h"
#include "LayerAction.h"
#include "Layer.h"

#include "util/PixelSelectionTool.h"

#include "Application.h"

using namespace hdps::util;

SelectionAction::SelectionAction(LayerAction& layerAction, QWidget* targetWidget, PixelSelectionTool& pixelSelectionTool) :
    PixelSelectionAction(&layerAction, targetWidget, pixelSelectionTool),
    _layerAction(layerAction),
    _targetWidget(targetWidget),
    _pixelSelectionTool(pixelSelectionTool),
    _groupAction(this, true)
{
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("mouse-pointer"));

    auto allowedPixelSelectionTypes = defaultPixelSelectionTypes;

    // Add 'sample 'pixel selection type
    allowedPixelSelectionTypes << PixelSelectionType::Sample;

    // Assign allowed types
    setAllowedTypes(allowedPixelSelectionTypes);

    _groupAction.setText("Selection");

    // Populate group action
    _groupAction << _typeAction;
    _groupAction << _brushRadiusAction;
    _groupAction << _overlayColor;
    _groupAction << _overlayOpacity;
    _groupAction << _notifyDuringSelectionAction;

    const auto render = [this]() {
        _layerAction.getLayer().invalidate();
    };

    // Re-render when the overlay color or opacity changes
    connect(&_overlayColor, &ColorAction::colorChanged, this, render);
    connect(&_overlayOpacity, &DecimalAction::valueChanged, this, render);
}

SelectionAction::Widget::Widget(QWidget* parent, SelectionAction* selectionAction, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, selectionAction, state)
{
    setAutoFillBackground(true);
    setMinimumWidth(300);

    auto layout = new QHBoxLayout();

    layout->setMargin(4);
    layout->setSpacing(3);

    const auto getVerticalDivider = []() -> QFrame* {
        auto verticalDivider = new QFrame();
        
        verticalDivider->setFrameShape(QFrame::VLine);
        verticalDivider->setFrameShadow(QFrame::Sunken);

        return verticalDivider;
    };

    layout->addWidget(selectionAction->getRectangleAction().createWidget(this, ToggleAction::PushButtonIcon));
    layout->addWidget(selectionAction->getBrushAction().createWidget(this, ToggleAction::PushButtonIcon));
    layout->addWidget(selectionAction->getLassoAction().createWidget(this, ToggleAction::PushButtonIcon));
    layout->addWidget(selectionAction->getPolygonAction().createWidget(this, ToggleAction::PushButtonIcon));
    layout->addWidget(selectionAction->getSampleAction().createWidget(this, ToggleAction::PushButtonIcon));
    layout->addWidget(selectionAction->getBrushRadiusAction().createWidget(this));
    layout->addWidget(getVerticalDivider());
    layout->addWidget(selectionAction->getModifierAddAction().createWidget(this));
    layout->addWidget(selectionAction->getModifierSubtractAction().createWidget(this));
    layout->addWidget(getVerticalDivider());
    layout->addWidget(selectionAction->getOverlayOpacity().createWidget(this));
    layout->addWidget(getVerticalDivider());
    layout->addWidget(selectionAction->getSelectAllAction().createWidget(this));
    layout->addWidget(selectionAction->getClearSelectionAction().createWidget(this));
    layout->addWidget(selectionAction->getInvertSelectionAction().createWidget(this));
    layout->addWidget(getVerticalDivider());
    layout->addWidget(selectionAction->getNotifyDuringSelectionAction().createWidget(this));

    layout->addStretch(1);

    setLayout(layout);
}