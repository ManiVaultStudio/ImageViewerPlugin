#include "ScalarChannelAction.h"
#include "ImageSettingsAction.h"
#include "Layer.h"

#include <util/Exception.h>

#include <PointData/PointData.h>
#include <ClusterData/ClusterData.h>

using namespace mv;

const QMap<ScalarChannelAction::Identifier, QString> ScalarChannelAction::channelIndexes = {
    { ScalarChannelAction::Channel1, "Channel 1" },
    { ScalarChannelAction::Channel2, "Channel 2" },
    { ScalarChannelAction::Channel3, "Channel 3" }
};

ScalarChannelAction::ScalarChannelAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _layer(nullptr),
    _identifier(Channel1),
    _enabledAction(this, "Enabled"),
    _dimensionAction(this, "Dimension"),
    _windowLevelAction(this, "Window/Level"),
    _scalarData(),
    _scalarDataRange({ 0.0f, 0.0f }),
    _displayRange({ 0.0f, 1.0f }),
    _fixedDisplayRange(false) 
{
    setDefaultWidgetFlags(GroupAction::Horizontal);
    setShowLabels(false);

    addAction(&_dimensionAction);
    addAction(&_windowLevelAction);

    _windowLevelAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);

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

    const auto resizeScalars = [this]() {
        _scalarData.resize(getImages()->getNumberOfPixels());
    };

    resizeScalars();

    connect(&_layer->getImageSettingsAction().getSubsampleFactorAction(), &IntegralAction::valueChanged, this, [this]() {
        computeScalarData();
    });

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

void ScalarChannelAction::setFixedDisplayRange(bool status, float lower, float upper)
{
    _fixedDisplayRange = status;
    _displayRange.first = lower;
    _displayRange.second = upper;
}

QPair<float, float> ScalarChannelAction::getDisplayRange()
{
    if (!_fixedDisplayRange)
    {
        const auto range            = _scalarDataRange.second - _scalarDataRange.first;
        const auto maxWindow        = range;
        const auto windowNormalized = _windowLevelAction.getWindowAction().getValue();
        const auto levelNormalized  = _windowLevelAction.getLevelAction().getValue();
        const auto level            = std::clamp(0.f + (levelNormalized * maxWindow), 0.f, range);
        const auto window           = std::clamp(windowNormalized * maxWindow, 0.f, range);

        _displayRange.first  = std::clamp(level - (window / 2.0f), 0.f, range);
        _displayRange.second = std::clamp(level + (window / 2.0f), 0.f, range);

        _displayRange.first  += _scalarDataRange.first;
        _displayRange.second += _scalarDataRange.first;
    }

    return _displayRange;
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
    auto publicScalarChannelAction = dynamic_cast<ScalarChannelAction*>(publicAction);

    Q_ASSERT(publicScalarChannelAction != nullptr);

    if (publicScalarChannelAction == nullptr)
        return;

    if (recursive) {
        actions().connectPrivateActionToPublicAction(&_dimensionAction, &publicScalarChannelAction->getDimensionAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_windowLevelAction, &publicScalarChannelAction->getWindowLevelAction(), recursive);
    }

    GroupAction::connectToPublicAction(publicAction, recursive);
}

void ScalarChannelAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    if (recursive) {
        actions().disconnectPrivateActionFromPublicAction(&_dimensionAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_windowLevelAction, recursive);
    }

    GroupAction::disconnectFromPublicAction(recursive);
}

void ScalarChannelAction::fromVariantMap(const QVariantMap& variantMap)
{
    GroupAction::fromVariantMap(variantMap);

    _dimensionAction.fromParentVariantMap(variantMap);
    _windowLevelAction.fromParentVariantMap(variantMap);
}

QVariantMap ScalarChannelAction::toVariantMap() const
{
    auto variantMap = GroupAction::toVariantMap();

    _dimensionAction.insertIntoVariantMap(variantMap);
    _windowLevelAction.insertIntoVariantMap(variantMap);

    return variantMap;
}