#pragma once

#include <actions/GroupAction.h>
//#include <actions/IntegralRectangleAction.h>
//#include <actions/DecimalRectangleAction.h>
//#include <actions/VerticalGroupAction.h>

#include <actions/RectangleAction.h>
#include <actions/IntegralRangeAction.h>
#include <actions/DecimalRangeAction.h>

class Layer;
using namespace hdps::gui;

// ######################
//      Helper Actions
// ######################

class IntegralRectangleAction : public RectangleAction<QRect, IntegralRangeAction>
{
    Q_OBJECT

public:

    /**
        * Constructor
        * @param parent Pointer to parent object
        * @param title Title of the action
        * @param rectangle Rectangle
        */
    Q_INVOKABLE IntegralRectangleAction(QObject* parent, const QString& title, const QRect& rectangle = QRect());

    /**
        * Set Axis label text
        * @param xAxis1 Label string of first x axis value
        * @param xAxis2 Label string of second x axis value
        * @param yAxis1 Label string of first y axis value
        * @param yAxis2 Label string of second y axis value
        */
    void setAxisNames(const QString& xAxis1, const QString& xAxis2, const QString& yAxis1, const QString& yAxis2)
    {
        getRangeAction(Axis::X).getRangeMinAction().setPrefix(xAxis1);
        getRangeAction(Axis::X).getRangeMaxAction().setPrefix(xAxis2);

        getRangeAction(Axis::Y).getRangeMinAction().setPrefix(yAxis1);
        getRangeAction(Axis::Y).getRangeMaxAction().setPrefix(yAxis2);
    }

    /**
        * Get rectangle
        * @return Rectangle
        */
    QRect getRectangle() const
    {
        return _rectangle;
    }

    /**
        * Set rectangle to \p rectangle
        * @param rectangle Rectangle
        */
    void setRectangle(const QRect& rectangle)
    {
        if (rectangle == _rectangle)
            return;

        _rectangle = rectangle;

        _rectangleChanged();
    }

protected: // Linking

    /**
        * Connect this action to a public action
        * @param publicAction Pointer to public action to connect to
        * @param recursive Whether to also connect descendant child actions
        */
    void connectToPublicAction(WidgetAction* publicAction, bool recursive) override;

    /**
        * Disconnect this action from its public action
        * @param recursive Whether to also disconnect descendant child actions
        */
    void disconnectFromPublicAction(bool recursive) override;

public: // Serialization

    /**
        * Load widget action from variant map
        * @param Variant map representation of the widget action
        */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
        * Save widget action to variant map
        * @return Variant map representation of the widget action
        */
    QVariantMap toVariantMap() const override;

signals:

    /**
        * Signals that the rectangle changed
        * @param rectangle Rectangle
        */
    void rectangleChanged(const QRect& rectangle);

    friend class AbstractActionsManager;

private:
    QRect   _rectangle;

};


Q_DECLARE_METATYPE(IntegralRectangleAction)
inline const auto intRectangleActionMetaTypeId = qRegisterMetaType<IntegralRectangleAction*>("IntegralRectangleAction");


class DecimalRectangleAction : public RectangleAction<QRectF, DecimalRangeAction>
{
    Q_OBJECT

public:

    /**
        * Constructor
        * @param parent Pointer to parent object
        * @param title Title of the action
        * @param rectangle Rectangle
        */
    Q_INVOKABLE DecimalRectangleAction(QObject* parent, const QString& title, const QRectF& rectangle = QRectF());

    /**
        * Set Axis label text
        * @param xAxis1 Label string of first x axis value
        * @param xAxis2 Label string of second x axis value
        * @param yAxis1 Label string of first y axis value
        * @param yAxis2 Label string of second y axis value
        */
    void setAxisNames(const QString& xAxis1, const QString& xAxis2, const QString& yAxis1, const QString& yAxis2)
    {
        getRangeAction(Axis::X).getRangeMinAction().setPrefix(xAxis1);
        getRangeAction(Axis::X).getRangeMaxAction().setPrefix(xAxis2);

        getRangeAction(Axis::Y).getRangeMinAction().setPrefix(yAxis1);
        getRangeAction(Axis::Y).getRangeMaxAction().setPrefix(yAxis2);
    }

    /**
        * Get rectangle
        * @return Rectangle
        */
    QRectF getRectangle() const
    {
        return _rectangle;
    }

    /**
        * Set rectangle to \p rectangle
        * @param rectangle Rectangle
        */
    void setRectangle(const QRectF& rectangle)
    {
        if (rectangle == _rectangle)
            return;

        _rectangle = rectangle;

        _rectangleChanged();
    }

protected: // Linking

    /**
        * Connect this action to a public action
        * @param publicAction Pointer to public action to connect to
        * @param recursive Whether to also connect descendant child actions
        */
    void connectToPublicAction(WidgetAction* publicAction, bool recursive) override;

    /**
        * Disconnect this action from its public action
        * @param recursive Whether to also disconnect descendant child actions
        */
    void disconnectFromPublicAction(bool recursive) override;

public: // Serialization

    /**
        * Load widget action from variant map
        * @param Variant map representation of the widget action
        */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
        * Save widget action to variant map
        * @return Variant map representation of the widget action
        */
    QVariantMap toVariantMap() const override;

signals:

    /**
        * Signals that the rectangle changed
        * @param rectangle Rectangle
        */
    void rectangleChanged(const QRectF& rectangle);

    friend class AbstractActionsManager;

private:
    QRectF   _rectangle;
};

Q_DECLARE_METATYPE(DecimalRectangleAction)
inline const auto decRectangleActionMetaTypeId = qRegisterMetaType<DecimalRectangleAction*>("DecimalRectangleAction");


// #######################
//   MiscellaneousAction
// #######################

/**
 * Miscellaneous action class
 *
 * Group action class for various actions
 *
 * @author Thomas Kroes
 */
class MiscellaneousAction : public GroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title
     */
    Q_INVOKABLE MiscellaneousAction(QObject* parent, const QString& title, Layer& layer);

public: // Action getters

    IntegralRectangleAction& getRoiLayerAction() { return _roiLayerAction; }
    DecimalRectangleAction& getRoiViewAction() { return _roiViewAction; }
    IntegralRectangleAction& getRoiDetailAction() { return _roiDetailAction; }

protected:
    Layer& _layer;             /** Reference to layer */
    IntegralRectangleAction     _roiLayerAction;    /** Layer region of interest action */
    DecimalRectangleAction     _roiViewAction;     /** View region of interest action */
    IntegralRectangleAction     _roiDetailAction;   /** Layer region of interest of another viewer action */
};

Q_DECLARE_METATYPE(MiscellaneousAction)

inline const auto miscellaneousActionMetaTypeId = qRegisterMetaType<MiscellaneousAction*>("MiscellaneousAction");

