#include "ScalarChannelAction.h"
#include "ImageSettingsAction.h"
#include "Layer.h"

#include <util/Exception.h>

#include <PointData/PointData.h>
#include <ClusterData/ClusterData.h>

#include <QHBoxLayout>
#include <QCheckBox>

using namespace hdps;

const QMap<ScalarChannelAction::Identifier, QString> ScalarChannelAction::channelIndexes = {
    { ScalarChannelAction::Channel1, "Channel 1" },
    { ScalarChannelAction::Channel2, "Channel 2" },
    { ScalarChannelAction::Channel3, "Channel 3" }
};

ScalarChannelAction::ScalarChannelAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _layer(nullptr),
    _identifier(Channel1),
    _enabledAction(this, "Enabled"),
    _dimensionAction(this, "Dimension"),
    _windowLevelAction(this, "Window/Level"),
    _scalarData(),
    _scalarDataRange({0.0f, 0.0f})
{
    setDefaultWidgetFlags(ScalarChannelAction::ComboBox | ScalarChannelAction::WindowLevelWidget);

    connect(&_dimensionAction, &OptionAction::isPublishedChanged, this, &ScalarChannelAction::isPublishedChanged);

    const auto updateEnabled = [this]() -> void {
        setEnabled(_enabledAction.isChecked());
    };

    connect(&_enabledAction, &ToggleAction::toggled, this, updateEnabled);

    connect(&_windowLevelAction, &WindowLevelAction::changed, this, [this]() {
        emit changed(*this);
    });

    updateEnabled();
}

void ScalarChannelAction::initialize(Layer* layer, const Identifier& identifier)
{
    Q_ASSERT(layer != nullptr);

    if (layer == nullptr)
        return;

    _layer      = layer;
    _identifier = identifier;

    switch (_identifier)
    {
        case Channel1:
            _enabledAction.setChecked(true);
            break;

        default:
            break;
    }

    connect(&_layer->getImageSettingsAction().getSubsampleFactorAction(), &IntegralAction::valueChanged, this, [this]() {
        computeScalarData();
    });

    const auto resizeScalars = [this]() {
        _scalarData.resize(getImages()->getNumberOfPixels());
    };

    resizeScalars();

    connect(&_layer->getImageSettingsAction().getSubsampleFactorAction(), &IntegralAction::valueChanged, this, resizeScalars);
    connect(&_dimensionAction, &OptionAction::currentIndexChanged, this, &ScalarChannelAction::computeScalarData);

    computeScalarData();

    //connect(this, &QAction::changed, this, &ScalarChannelAction::computeScalarData);
}

const ScalarChannelAction::Identifier ScalarChannelAction::getIdentifier() const
{
    return _identifier;
}

QSize ScalarChannelAction::getImageSize()
{
    if (_layer == nullptr)
        return {};

    const auto imageSize        = getImages()->getImageSize();
    const auto subsampleFactor  = _layer->getImageSettingsAction().getSubsampleFactorAction().getValue();
    
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

    const auto range            = _scalarDataRange.second - _scalarDataRange.first;
    const auto maxWindow        = range;
    const auto windowNormalized = _windowLevelAction.getWindowAction().getValue();
    const auto levelNormalized  = _windowLevelAction.getLevelAction().getValue();
    const auto level            = std::clamp(0.f + (levelNormalized * maxWindow), 0.f, range);
    const auto window           = std::clamp(windowNormalized * maxWindow, 0.f, range);

    displayRange.first  = std::clamp(level - (window / 2.0f), 0.f, range);
    displayRange.second = std::clamp(level + (window / 2.0f), 0.f, range);

    displayRange.first += _scalarDataRange.first;
    displayRange.second += _scalarDataRange.first;

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
    if (_layer == nullptr)
        return {};

    return _layer->getImages();
}

void ScalarChannelAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    //_dimensionAction.connectToPublicAction(publicAction);

    //WidgetAction::connectToPublicAction(publicAction);
}

void ScalarChannelAction::disconnectFromPublicAction(bool recursive)
{
    //_dimensionAction.disconnectFromPublicAction();

    //WidgetAction::disconnectFromPublicAction();
}

void ScalarChannelAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);
}

QVariantMap ScalarChannelAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    //_continuousUpdatesAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

QWidget* ScalarChannelAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(3);

    if (widgetFlags & ScalarChannelAction::ComboBox)
        layout->addWidget(_dimensionAction.createWidget(widget));

    if (widgetFlags & WidgetFlag::WindowLevelWidget)
        layout->addWidget(_windowLevelAction.createCollapsedWidget(widget));

    widget->setLayout(layout);

    return widget;
}