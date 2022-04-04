#include "ScalarChannelAction.h"
#include "ImageAction.h"
#include "Layer.h"

#include <util/Exception.h>

#include <PointData.h>
#include <ClusterData.h>

#include <QHBoxLayout>
#include <QCheckBox>

using namespace hdps;

const QMap<ScalarChannelAction::Identifier, QString> ScalarChannelAction::channelIndexes = {
    { ScalarChannelAction::Channel1, "Channel 1" },
    { ScalarChannelAction::Channel2, "Channel 2" },
    { ScalarChannelAction::Channel3, "Channel 3" }
};

ScalarChannelAction::ScalarChannelAction(ImageAction& imageAction, const Identifier& index, const QString& name) :
    WidgetAction(reinterpret_cast<QObject*>(&imageAction)),
    _imageAction(imageAction),
    _identifier(index),
    _enabledAction(this, "Enabled"),
    _dimensionAction(this, "Dimension"),
    _windowLevelAction(this),
    _scalarData(),
    _scalarDataRange({0.0f, 0.0f})
{
    setText(name);
    setDefaultWidgetFlags(ScalarChannelAction::ComboBox | ScalarChannelAction::WindowLevelWidget);

    connect(&_dimensionAction, &OptionAction::isPublishedChanged, this, &ScalarChannelAction::isPublishedChanged);

    switch (_identifier)
    {
        case Channel1:
            _enabledAction.setChecked(true);
            break;

        default:
            break;
    }

    connect(&_dimensionAction, &OptionAction::currentIndexChanged, this, [this]() {
        computeScalarData();
    });

    connect(&_imageAction.getSubsampleFactorAction(), &IntegralAction::valueChanged, this, [this]() {
        computeScalarData();
    });

    // Compute scalar data when the action is enabled
    connect(this, &QAction::changed, this, [this]() -> void {
        computeScalarData();
    });

    // Allocate space for the data
    const auto resizeScalars = [this]() {
        _scalarData.resize(getImages()->getNumberOfPixels());
    };

    const auto updateEnabled = [this]() -> void {
        setEnabled(_enabledAction.isChecked());
    };

    connect(&_enabledAction, &ToggleAction::toggled, this, updateEnabled);

    // Flag as changed when the window level settings change
    connect(&_windowLevelAction, &WindowLevelAction::changed, this, [this]() {
        emit changed(*this);
    });

    // Flag as changed when the window level settings change
    connect(&_imageAction.getSubsampleFactorAction(), &IntegralAction::valueChanged, this, resizeScalars);

    updateEnabled();
    resizeScalars();

    computeScalarData();
}

const ScalarChannelAction::Identifier ScalarChannelAction::getIdentifier() const
{
    return _identifier;
}

QSize ScalarChannelAction::getImageSize()
{
    const auto imageSize        = getImages()->getImageSize();
    const auto subsampleFactor  = _imageAction.getSubsampleFactorAction().getValue();
    
    return QSize(static_cast<int>(floorf(imageSize.width() / subsampleFactor)), static_cast<int>(floorf(imageSize.width() / subsampleFactor)));
}

const QVector<float>& ScalarChannelAction::getScalarData() const
{
    return _scalarData;
}

const QPair<float, float>& ScalarChannelAction::getScalarDataRange() const
{
    return _scalarDataRange;
}

QPair<float, float> ScalarChannelAction::getDisplayRange()
{
    QPair<float, float> displayRange;

    const auto maxWindow        = _scalarDataRange.second - _scalarDataRange.first;
    const auto windowNormalized = _windowLevelAction.getWindowAction().getValue();
    const auto levelNormalized  = _windowLevelAction.getLevelAction().getValue();
    const auto level            = std::clamp(_scalarDataRange.first + (levelNormalized * maxWindow), _scalarDataRange.first, _scalarDataRange.second);
    const auto window           = std::clamp(windowNormalized * maxWindow, _scalarDataRange.first, _scalarDataRange.second);

    displayRange.first  = std::clamp(level - (window / 2.0f), _scalarDataRange.first, _scalarDataRange.second);
    displayRange.second = std::clamp(level + (window / 2.0f), _scalarDataRange.first, _scalarDataRange.second);

    return displayRange;
}

void ScalarChannelAction::computeScalarData()
{
    
    try
    {
        if (!_enabledAction.isChecked())
            return;

        if (!getImages().isValid())
            throw std::runtime_error("Images dataset is not valid");

        switch (_identifier)
        {
            case Channel1:
            case Channel2:
            case Channel3:
            {
                if (_dimensionAction.getCurrentIndex() < 0)
                    break;

                getImages()->getScalarData(_dimensionAction.getCurrentIndex(), _scalarData, _scalarDataRange);

                break;
            }

            default:
                break;
        }

        // Publish scalar data change
        emit changed(*this);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to compute scalar data", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to compute scalar data");
    }
}

Dataset<Images> ScalarChannelAction::getImages()
{
    return _imageAction.getLayer().getImages();
}

bool ScalarChannelAction::mayPublish() const
{
    return true;
}

bool ScalarChannelAction::isPublic() const
{
    return _dimensionAction.isPublic();
}

void ScalarChannelAction::publish(const QString& name)
{
    _dimensionAction.publish(name);
}

void ScalarChannelAction::connectToPublicAction(WidgetAction* publicAction)
{
    _dimensionAction.connectToPublicAction(publicAction);

    WidgetAction::connectToPublicAction(publicAction);
}

void ScalarChannelAction::disconnectFromPublicAction()
{
    _dimensionAction.disconnectFromPublicAction();

    WidgetAction::disconnectFromPublicAction();
}

WidgetAction* ScalarChannelAction::getPublicCopy() const
{
    return new OptionAction(parent(), text(), _dimensionAction.getOptions(), _dimensionAction.getCurrentText(), _dimensionAction.getDefaultText());
}

QWidget* ScalarChannelAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(3);

    if (widgetFlags & ScalarChannelAction::ComboBox)
        layout->addWidget(_dimensionAction.createWidget(widget));

    if (widgetFlags & WidgetFlag::WindowLevelWidget)
        layout->addWidget(_windowLevelAction.createCollapsedWidget(widget));

    widget->setLayout(layout);

    return widget;
}