/***************************************************************************
**                                                                        **
**  QcGauge, for instrumentation, and real time data measurement          **
**  visualization widget for Qt.                                          **
**  Copyright (C) 2015 Hadj Tahar Berrima                                 **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU Lesser General Public License as        **
**  published by the Free Software Foundation, either version 3 of the    **
**  License, or (at your option) any later version.                       **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU Lesser General Public License for more details.                   **
**                                                                        **
**  You should have received a copy of the GNU Lesser General Public      **
**  License along with this program.                                      **
**  If not, see http://www.gnu.org/licenses/.                             **
**                                                                        **
****************************************************************************
**           Author:  Hadj Tahar Berrima                                  **
**           Website: http://pytricity.com/                               **
**           Contact: berrima_tahar@yahoo.com                             **
**           Date:    1 dec 2014                                          **
**           Version:  1.0                                                **
****************************************************************************/



#include <QStyleOption>
#include <search.h>
#include "qcgaugewidget.h"

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

QcGaugeWidget::QcGaugeWidget(QWidget *parent) :
    QWidget(parent)
{
    setMinimumSize(250,250);
}

QcBackgroundItem *QcGaugeWidget::addBackground(float position)
{
    auto item = new QcBackgroundItem(this);
    item->setPosition(position);
    mItems.append(item);
    return item;
}

QcDegreesItem *QcGaugeWidget::addDegrees(float position)
{
    auto item = new QcDegreesItem(this);
    item->setPosition(position);

    mItems.append(item);
    return item;
}


QcValuesItem *QcGaugeWidget::addValues(float position)
{
    QcValuesItem * item = new QcValuesItem(this);
    item->setPosition(position);
    mItems.append(item);
    return item;
}

QcArcItem *QcGaugeWidget::addArc(float position)
{
    auto item = new QcArcItem(this);
    item->setPosition(position);
    mItems.append(item);
    return item;
}

QcColorBand *QcGaugeWidget::addColorBand(float position)
{
    auto item = new QcColorBand(this);
    item->setPosition(position);
    mItems.append(item);
    return item;
}

QcNeedleItem *QcGaugeWidget::addNeedle(float position)
{
    auto item = new QcNeedleItem(this);
    item->setPosition(position);
    mItems.append(item);
    return item;
}

QcLabelItem *QcGaugeWidget::addLabel(float position)
{
    auto item = new QcLabelItem(this);
    item->setPosition(position);
    mItems.append(item);
    return item;
}

QcGlassItem *QcGaugeWidget::addGlass(float position)
{
    auto item = new QcGlassItem(this);
    item->setPosition(position);
    mItems.append(item);
    return item;
}

QcAttitudeMeter *QcGaugeWidget::addAttitudeMeter(float position)
{
    auto item = new QcAttitudeMeter(this);
    item->setPosition(position);
    mItems.append(item);
    return item;
}

void QcGaugeWidget::addItem(QcItem *item,float position)
{
    // takes parentship of the item
    item->setParent(this);
    item->setPosition(position);
    mItems.append(item);
}

int QcGaugeWidget::removeItem(QcItem *item)
{
   return mItems.removeAll(item);
}

QList<QcItem *> QcGaugeWidget::items()
{
    return mItems;
}


void QcGaugeWidget::paintEvent(QPaintEvent */*paintEvt*/)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
    painter.setRenderHint(QPainter::Antialiasing);

    foreach (QcItem * item, mItems) {
        item->draw(&painter);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

QcItem::QcItem(QObject *parent) :
    QObject(parent)
{

    parentWidget = qobject_cast<QWidget*>(parent);
    mPosition = 50;
}

int QcItem::type()
{
    return 50;
}

void QcItem::update()
{
    parentWidget->update();
}

float QcItem::position()
{
    return mPosition;
}

QRectF QcItem::rect()
{
    return mRect;
}

void QcItem::setPosition(float position)
{
    if(position>100)
        mPosition = 100;
    else if(position<0)
        mPosition = 0;
    else
        mPosition = position;
    update();
}

QRectF QcItem::adjustRect(float percentage)
{
    float r = getRadius(mRect);
    float offset =   r-(percentage*r)/100.0;
    QRectF tmpRect = mRect.adjusted(offset,offset,-offset,-offset);
    return tmpRect;
}

float QcItem::getRadius(const QRectF &tmpRect)
{
    float r = 0;
    if(tmpRect.width()<tmpRect.height())
        r = tmpRect.width()/2.0;
    else
        r = tmpRect.height()/2.0;
    return r;
}

QRectF QcItem::resetRect()
{
    mRect = parentWidget->rect();
    float r = getRadius(mRect);
    mRect.setWidth(2.0*r);
    mRect.setHeight(2.0*r);
    mRect.moveCenter(parentWidget->rect().center());
    return mRect;
}

QPointF QcItem::getPoint(float deg,const QRectF &tmpRect)
{
    float r = getRadius(tmpRect);
    float xx=cos(qDegreesToRadians(deg))*r;
    float yy=sin(qDegreesToRadians(deg))*r;
    QPointF pt;
    xx=tmpRect.center().x()-xx;
    yy=tmpRect.center().y()-yy;
    pt.setX(xx);
    pt.setY(yy);
    return pt;
}



float QcItem::getAngle(const QPointF&pt, const QRectF &tmpRect)
{
    float xx=tmpRect.center().x()-pt.x();
    float yy=tmpRect.center().y()-pt.y();
    return qRadiansToDegrees( atan2(yy,xx));
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

QcScaleItem::QcScaleItem(QObject *parent) :
    QcItem(parent)
{
    mMinDegree = -45;
    mMaxDegree = 225;
    mMinValue = 0;
    mMaxValue = 100;
    mDegreeOffset = 0;
}

void QcScaleItem::setValueRange(float minValue, float maxValue)
{
    if (minValue < maxValue) {
        mMinValue = minValue;
        mMaxValue = maxValue;

    } else throw (InvalidValueRange);
}

void QcScaleItem::setDegreeRange(float minDegree, float maxDegree)
{
    if (minDegree < maxDegree) {
        mMinDegree = minDegree;
        mMaxDegree = maxDegree;
    } else throw (InvalidValueRange);
}

float QcScaleItem::getDegFromValue(float v) const
{
    float a = (mMaxDegree-mMinDegree)/(mMaxValue-mMinValue);
    float b = -a*mMinValue+mMinDegree;
    return mDegreeOffset+(a*v+b);
}

float QcScaleItem::getDegFromValue()
{
    return getDegFromValue(mMinValue);
}


void QcScaleItem::setMinValue(float minValue)
{
    if(minValue>mMaxValue)
        throw (InvalidValueRange);
    mMinValue = minValue;
    update();
}


void QcScaleItem::setMaxValue(float maxValue)
{
    if(maxValue<mMinValue )
        throw (InvalidValueRange);
    mMaxValue = maxValue;
    update();
}

void QcScaleItem::setMinDegree(float minDegree)
{
    if(minDegree>mMaxDegree)
        throw (InvalidDegreeRange);
    mMinDegree = minDegree;
    update();
}

void QcScaleItem::setMaxDegree(float maxDegree)
{
    if(maxDegree<mMinDegree)
        throw (InvalidDegreeRange);
    mMaxDegree = maxDegree;
    update();
}

void QcScaleItem::setDegreeOffset(float degreeOffset)
{
    mDegreeOffset = degreeOffset;
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

QcBackgroundItem::QcBackgroundItem(QObject *parent) :
    QcItem(parent)
{
    setPosition(88);
    mPen = Qt::NoPen;
    setPosition(88);

    addColor(0.4,Qt::darkGray);
    addColor(0.8,Qt::black);

}


void QcBackgroundItem::draw(QPainter* painter)
{
    QRectF tmpRect = resetRect();
    painter->setBrush(Qt::NoBrush);
    QLinearGradient linearGrad(tmpRect.topLeft(), tmpRect.bottomRight());
    for(int i = 0;i<mColors.size();i++){
        linearGrad.setColorAt(mColors[i].first,mColors[i].second);
    }
    painter->setPen(mPen);
    painter->setBrush(linearGrad);
    painter->drawEllipse(adjustRect(position()));
}

void QcBackgroundItem::addColor(float position, const QColor &color)
{
    if(position<0||position>1)
        return;
      QPair<float,QColor> pair;
      pair.first = position;
      pair.second = color;
      mColors.append(pair);
      update();
}

void QcBackgroundItem::clearrColors()
{
    mColors.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

QcGlassItem::QcGlassItem(QObject *parent) :
    QcItem(parent)
{
    setPosition(88);
}

void QcGlassItem::draw(QPainter *painter)
{
    resetRect();
    QRectF tmpRect1 = adjustRect(position());
    QRectF tmpRect2 = tmpRect1;
    float r = getRadius(tmpRect1);
    tmpRect2.setHeight(r/2.0);
    painter->setPen(Qt::NoPen);

    QColor clr1 = Qt::gray ;
    QColor clr2 = Qt::white;
    clr1.setAlphaF(0.2);
    clr2.setAlphaF(0.4);

    QLinearGradient linearGrad1(tmpRect1.topLeft(), tmpRect1.bottomRight());
    linearGrad1.setColorAt(0.1, clr1);
    linearGrad1.setColorAt(0.5, clr2);

    painter->setBrush(linearGrad1);
    painter->drawPie(tmpRect1,0,16*180);
    tmpRect2.moveCenter(rect().center());
    painter->drawPie(tmpRect2,0,-16*180);
}
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

QcLabelItem::QcLabelItem(QObject *parent) :
    QcItem(parent)
{
    setPosition(50);
    mAngle = 270;
    mText = "%";
    mColor = Qt::black;
    mFont = "Arial";
}

void QcLabelItem::draw(QPainter *painter)
{
    resetRect();
    QRectF tmpRect = adjustRect(position());
    float r = getRadius(rect());
    QFont font(mFont, r/10.0, QFont::Bold);
    painter->setFont(font);
    painter->setPen(QPen(mColor));

    QPointF txtCenter = getPoint(mAngle,tmpRect);
    QFontMetrics fMetrics = painter->fontMetrics();
    QSize sz = fMetrics.size( Qt::TextSingleLine, mText );
    QRectF txtRect(QPointF(0,0), sz );
    txtRect.moveCenter(txtCenter);

    painter->drawText( txtRect, Qt::TextSingleLine,mText );

}

void QcLabelItem::setAngle(float a)
{
    mAngle = a;
    update();
}

float QcLabelItem::angle()
{
    return mAngle;
}

void QcLabelItem::setText(const QString &text, bool repaint)
{
    mText = text;
    if(repaint)
        update();
}

QString QcLabelItem::text()
{
    return mText;
}

void QcLabelItem::setColor(const QColor &color)
{
    mColor = color;
    update();
}

QColor QcLabelItem::color()
{
    return mColor;
}

void QcLabelItem::setFont(const QString &font) {
    mFont = font;
}

QString QcLabelItem::font() {
    return mFont;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

QcArcItem::QcArcItem(QObject *parent) :
    QcScaleItem(parent)
{
    setPosition(80);
    mColor = Qt::black;
}

void QcArcItem::draw(QPainter *painter)
{
    resetRect();
    QRectF tmpRect= adjustRect(position());
    float r = getRadius(tmpRect);

    QPen pen;
    pen.setColor(mColor);
    pen.setWidthF(r/40);
    painter->setPen(pen);
    painter->drawArc(tmpRect,-16*(mMinDegree+180),-16*(mMaxDegree-mMinDegree));
}

void QcArcItem::setColor(const QColor &color)
{
    mColor = color;
}
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

QcColorBand::QcColorBand(QObject *parent) :
    QcScaleItem(parent)
{
    QColor tmpColor;
    tmpColor.setAlphaF(0.1);
    QPair<QColor,float> pair;

    pair.first = Qt::green;
    pair.second = 10;
    mBandColors.append(pair);

    pair.first = Qt::darkGreen;
    pair.second = 50;
    mBandColors.append(pair);

    pair.first = Qt::red;
    pair.second = 100;
    mBandColors.append(pair);

    setPosition(50);
}

QPainterPath QcColorBand::createSubBand(float from, float sweep)
{
    QRectF tmpRect = adjustRect(position());
    QPainterPath path;
    path.arcMoveTo(tmpRect,180+from);
    path.arcTo(tmpRect,180+from,-sweep);
    return path;
}

void QcColorBand::draw(QPainter *painter)
{
    resetRect();
    float r = getRadius(rect());
    QPen pen;
    pen.setCapStyle(Qt::FlatCap);
    pen.setWidthF(r/20.0);
    painter->setBrush(Qt::NoBrush);
    float offset = getDegFromValue();
    for(int i = 0;i<mBandColors.size();i++){
        QColor clr = mBandColors[i].first;
        float sweep=0;
        if(i==0)
            sweep = getDegFromValue(mBandColors[i].second)-getDegFromValue(mMinValue);
        else
            sweep = getDegFromValue(mBandColors[i].second)-getDegFromValue(mBandColors[i-1].second);
        QPainterPath path = createSubBand(-offset,sweep);
        offset += sweep;
        pen.setColor(clr);
        painter->setPen(pen);
        painter->drawPath(path);
    }
}
void QcColorBand::setColors(const QList<QPair<QColor, float> > &colors)
{
    mBandColors = colors;
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

QcDegreesItem::QcDegreesItem(QObject *parent) :
    QcScaleItem(parent)
{
    mStep = 10;
    mColor = Qt::black;
    mSubDegree = false;
    setPosition(90);
}


void QcDegreesItem::draw(QPainter *painter)
{
    resetRect();
    QRectF tmpRect = adjustRect(position());

    painter->setPen(mColor);
    float r = getRadius(tmpRect);
    for(float val = mMinValue;val<=mMaxValue;val+=mStep){
        float deg = getDegFromValue(val);
        QPointF pt = getPoint(deg,tmpRect);
        QPainterPath path;
        path.moveTo(pt);
        path.lineTo(tmpRect.center());
        pt = path.pointAtPercent(0.03);
        QPointF newPt = path.pointAtPercent(0.13);

        QPen pen;
        pen.setColor(mColor);
        if(!mSubDegree)
            pen.setWidthF(r/25.0);

        painter->setPen(pen);
        painter->drawLine(pt,newPt);

    }
}

void QcDegreesItem::setStep(float step)
{
    mStep = step;
    update();
}

void QcDegreesItem::setColor(const QColor& color)
{
    mColor = color;
    update();
}

void QcDegreesItem::setSubDegree(bool b)
{
    mSubDegree = b;
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

QcNeedleItem::QcNeedleItem(QObject *parent) :
    QcScaleItem(parent)
{
    mCurrentValue = 0;
    mColor = Qt::black;
    mLabel = NULL;
    mFormat = "%1";
    mNeedleType = FeatherNeedle;
}

void QcNeedleItem::draw(QPainter *painter)
{
    resetRect();
    QRectF tmpRect = adjustRect(position());
    painter->save();
    painter->translate(tmpRect.center());
    float deg = getDegFromValue( mCurrentValue);
    painter->rotate(deg+90.0);
    painter->setBrush(QBrush(mColor));
    painter->setPen(Qt::NoPen);

    QLinearGradient grad;

    switch (mNeedleType) {
    case QcNeedleItem::FeatherNeedle:
        createFeatherNeedle(getRadius(tmpRect));
        break;
    case QcNeedleItem::DiamonNeedle:
        createDiamonNeedle(getRadius(tmpRect));
        break;
    case QcNeedleItem::TriangleNeedle:
        createTriangleNeedle(getRadius(tmpRect));
        break;
    case QcNeedleItem::AttitudeMeterNeedle:
        createAttitudeNeedle(getRadius(tmpRect));
        break;
    case QcNeedleItem::CompassNeedle:
        createCompassNeedle(getRadius(tmpRect));
        grad.setStart(mNeedlePoly[0]);
        grad.setFinalStop(mNeedlePoly[1]);
        grad.setColorAt(0.9,Qt::red);
        grad.setColorAt(1,Qt::blue);
        painter->setBrush(grad);

        break;

    default:
        break;
    }
    painter->drawConvexPolygon(mNeedlePoly);
    painter->restore();
}

void QcNeedleItem::setCurrentValue(float value)
{
       if(value<mMinValue)
        mCurrentValue = mMinValue;
    else if(value>mMaxValue)
        mCurrentValue = mMaxValue;
    else
        mCurrentValue = value;

    if(mLabel!=0)
        mLabel->setText( QString(mFormat).arg(mCurrentValue),false);

    update();
}

float QcNeedleItem::currentValue()
{
    return mCurrentValue;
}

void QcNeedleItem::setValueFormat(QString format){
    mFormat = format;
    update();
}

QString QcNeedleItem::currentValueFormat(){
    return mFormat;
}

void QcNeedleItem::setColor(const QColor &color)
{
    mColor = color;
    update();
}

QColor QcNeedleItem::color()
{
    return mColor;
}

void QcNeedleItem::setLabel(QcLabelItem *label)
{
    mLabel = label;
    update();
}

QcLabelItem *QcNeedleItem::label()
{
    return mLabel;
}


void QcNeedleItem::setNeedle(QcNeedleItem::NeedleType needleType)
{
    mNeedleType = needleType;
    update();
}


void QcNeedleItem::createDiamonNeedle(float r)
{
    QVector<QPointF> tmpPoints;
    tmpPoints.append(QPointF(0.0, 0.0));
    tmpPoints.append(QPointF(-r/20.0,r/20.0));
    tmpPoints.append(QPointF(0.0, r));
    tmpPoints.append(QPointF(r/20.0,r/20.0));
    mNeedlePoly = tmpPoints;
}

void QcNeedleItem::createTriangleNeedle(float r)
{
    QVector<QPointF> tmpPoints;
    tmpPoints.append(QPointF(0.0, r));
    tmpPoints.append(QPointF(-r/40.0, 0.0));
    tmpPoints.append(QPointF(r/40.0,0.0));
    mNeedlePoly = tmpPoints;
}

void QcNeedleItem::createFeatherNeedle(float r)
{
    QVector<QPointF> tmpPoints;
    tmpPoints.append(QPointF(0.0, r));
    tmpPoints.append(QPointF(-r/40.0, 0.0));
    tmpPoints.append(QPointF(-r/15.0, -r/5.0));
    tmpPoints.append(QPointF(r/15.0,-r/5));
    tmpPoints.append(QPointF(r/40.0,0.0));
    mNeedlePoly = tmpPoints;
}

void QcNeedleItem::createAttitudeNeedle(float r)
{
    QVector<QPointF> tmpPoints;
    tmpPoints.append(QPointF(0.0, r));
    tmpPoints.append(QPointF(-r/20.0, 0.85*r));
    tmpPoints.append(QPointF(r/20.0,0.85*r));
    mNeedlePoly = tmpPoints;
}

void QcNeedleItem::createCompassNeedle(float r)
{
    QVector<QPointF> tmpPoints;
    tmpPoints.append(QPointF(0.0, r));
    tmpPoints.append(QPointF(-r/15.0, 0.0));
    tmpPoints.append(QPointF(0.0, -r));
    tmpPoints.append(QPointF(r/15.0,0.0));
    mNeedlePoly = tmpPoints;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

QcValuesItem::QcValuesItem(QObject *parent) :
    QcScaleItem(parent)
{
    setPosition(70);
    mColor = Qt::black;
    mStep = 10;
    mFont = "Arial";
}


void QcValuesItem::draw(QPainter*painter)
{
    QRectF  tmpRect = resetRect();
    float r = getRadius(adjustRect(99));
    QFont font(mFont,0, QFont::Bold);
    font.setPointSizeF(0.08*r);

    painter->setFont(font);
    painter->setPen(mColor);
    for(float val = mMinValue;val<=mMaxValue;val+=mStep){
        float deg = getDegFromValue(val);
        QPointF pt = getPoint(deg,tmpRect);
        QPainterPath path;
        path.moveTo(pt);
        path.lineTo(    tmpRect.center());
        QString strVal = QString::number(val);
        QFontMetrics fMetrics = painter->fontMetrics();
        QSize sz = fMetrics.size( Qt::TextSingleLine, strVal );
        QRectF txtRect(QPointF(0,0), sz );
        QPointF textCenter = path.pointAtPercent(1.0-position()/100.0);
        txtRect.moveCenter(textCenter);

        painter->drawText( txtRect, Qt::TextSingleLine, strVal );
    }
}

void QcValuesItem::setStep(float step)
{
    mStep = step;
}

float QcValuesItem::step() {
    return mStep;
}

void QcValuesItem::setColor(const QColor& color)
{
    mColor = color;
}

QColor QcValuesItem::color() {
    return mColor;
}

void QcValuesItem::setFont(const QString& font)
{
    mFont = font;
}

QString QcValuesItem::font() {
    return mFont;
}



///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

QcAttitudeMeter::QcAttitudeMeter(QObject *parent) : QcItem(parent)
{
    mPitch = 0;
    mRoll = 0;
}
void QcAttitudeMeter::setCurrentPitch(float pitch)
{
    mPitch=-pitch;
    update();
}
void QcAttitudeMeter::setCurrentRoll(float roll)
{
    mRoll = roll;
    update();
}
QPointF QcAttitudeMeter::getIntersection(float r, const QPointF &pitchPoint, const QPointF &pt)
{
    // refrence it to zero

    Q_UNUSED(r)
    float a = (pitchPoint.y()-pt.y())/(pitchPoint.x()-pt.x());
    float b = pt.y()-a*pt.x();
    return QPointF(0,a*0+b);
}
float QcAttitudeMeter::getStartAngle(const QRectF& tmpRect)
{
    float r = getRadius(tmpRect);
    QPointF pt1 = getPoint(mRoll,tmpRect);
    pt1.setY(pt1.y()-mPitchOffset);
    QPointF pitchPoint = QPointF(tmpRect.center().x(),tmpRect.center().y()-mPitchOffset);


    ///////////////////////////////////////
    QPainterPath path1;
    path1.moveTo(pitchPoint);
    path1.lineTo(getIntersection(r,pitchPoint,pt1)+QPointF(0,5));
    path1.lineTo(getIntersection(r,pitchPoint,pt1)+QPointF(0,-5));

    QPainterPath path2;
    path2.addEllipse(tmpRect);

    QPointF p = path1.intersected(path2).pointAtPercent(.5);
    return getAngle(p,tmpRect);
}
void QcAttitudeMeter::draw(QPainter *painter)
{
    resetRect();
    QRectF tmpRect = adjustRect(position());
    float r = getRadius(tmpRect);
    if(mPitch<0)
        mPitchOffset = 0.0135*r*mPitch;
    else
        mPitchOffset = 0.015*r*mPitch;

    painter->setPen(Qt::NoPen);
    drawUpperEllipse(painter,tmpRect);
    drawLowerEllipse(painter,tmpRect);

    // Steps

    drawPitchSteps(painter,tmpRect);
    drawHandle(painter);

    drawDegrees(painter);

}
void QcAttitudeMeter::drawDegrees(QPainter *painter)
{
    resetRect();
    QRectF tmpRect = adjustRect(position());
    float r = getRadius(tmpRect);
    QPen pen;

    pen.setColor(Qt::white);
    painter->setPen(pen);
    for(int deg = 60;deg<=120;deg+=10){
        if(deg == 90)
            continue;
        drawDegree(painter,tmpRect,deg);
    }

    pen.setWidthF(r/30.0);
    painter->setPen(pen);
    drawDegree(painter,tmpRect,0);
    drawDegree(painter,tmpRect,90);
    drawDegree(painter,tmpRect,180);
    drawDegree(painter,tmpRect,30);
    drawDegree(painter,tmpRect,150);
}
void QcAttitudeMeter::drawDegree(QPainter * painter, const QRectF& tmpRect,float deg)
{
    QPointF pt1 = getPoint(deg,tmpRect);
    QPointF pt2 = tmpRect.center();
    QPainterPath path;
    path.moveTo(pt1);
    path.lineTo(pt2);
    QPointF pt = path.pointAtPercent(0.1);
    painter->drawLine(pt1,pt);
}
void QcAttitudeMeter::drawUpperEllipse(QPainter *painter, const QRectF &tmpRect)
{

    QLinearGradient radialGrad1(tmpRect.topLeft(),tmpRect.bottomRight());
    QColor clr1 = Qt::blue;
    clr1.setAlphaF(0.5);
    QColor clr2 = Qt::darkBlue;
    clr2.setAlphaF(0.5);
    radialGrad1.setColorAt(0, clr1);
    radialGrad1.setColorAt(.8, clr2);


    float offset = getStartAngle(tmpRect);
    float startAngle = 180-offset;
    float endAngle = offset-2*mRoll;
    float span =endAngle-startAngle;

    painter->setBrush(radialGrad1);
    painter->drawChord(tmpRect,16*startAngle,16*span);

}
void QcAttitudeMeter::drawLowerEllipse(QPainter *painter, const QRectF &tmpRect)
{
    QLinearGradient radialGrad2(tmpRect.topLeft(),tmpRect.bottomRight());
    QColor clr1 = QColor(139,119,118);
    QColor clr2 = QColor(139,119,101);
    radialGrad2.setColorAt(0, clr1);
    radialGrad2.setColorAt(.8, clr2);

    float offset = getStartAngle(tmpRect);
    float startAngle = 180+offset;
    float endAngle = offset-2*mRoll;
    float span =endAngle+startAngle;

    painter->setPen(Qt::NoPen);
    painter->setBrush(radialGrad2);
    painter->drawChord(tmpRect,-16*startAngle,16*span);

}
void QcAttitudeMeter::drawPitchSteps(QPainter *painter, const QRectF &tmpRect)
{
    float r = getRadius(tmpRect);
    QPointF center = tmpRect.center();
    painter->save();
    painter->translate(center.x(),center.y()-mPitchOffset);
    painter->rotate(mRoll);
    QPen pen;
    pen.setColor(Qt::white);
    pen.setWidthF(r/40.0);

    painter->setPen(pen);
    for (int i = -30;i<=30;i+=10){
        QPointF pt1;
        pt1.setX(-0.01*r*abs(i));
        pt1.setY(r/70.0*i);
        QPointF pt2;
        pt2.setX(0.01*r*abs(i));
        pt2.setY(r/70.0*i);
        painter->drawLine(pt1,pt2);

        if(i==0)
            continue;

        // draw value
        QFont font("Meiryo UI",0, QFont::Bold);
        font.setPointSizeF(0.08*r);
        painter->setFont(font);
        QString strVal = QString::number(abs(i));
        QFontMetrics fMetrics = painter->fontMetrics();
        QSize sz = fMetrics.size( Qt::TextSingleLine, strVal );
        QRectF leftTxtRect(QPointF(0,0), sz );
        QRectF rightTxtRect(QPointF(0,0), sz );
        leftTxtRect.moveCenter(pt1-QPointF(0.1*r,0));
        rightTxtRect.moveCenter(pt2+QPointF(0.1*r,0));
        painter->drawText( leftTxtRect, Qt::TextSingleLine, strVal );
        painter->drawText( rightTxtRect, Qt::TextSingleLine, strVal );
    }
    painter->restore();
}
void QcAttitudeMeter::drawHandle(QPainter *painter)
{
    QRectF tmpRct = adjustRect(15);
    float r = getRadius(tmpRct);
    QPen pen;
    pen.setColor(Qt::gray);
    pen.setWidthF(0.25*r);
    painter->setPen(pen);
    painter->drawArc(tmpRct,0,-16*180);

    QPointF center = tmpRct.center();
    QPointF leftPt1 = center;
    QPointF leftPt2 = center;
    QPointF rightPt1 = center;
    QPointF rightPt2 = center;
    leftPt1.setX(center.x()-2*r);
    leftPt2.setX(center.x()-r);
    rightPt1.setX(center.x()+2*r);
    rightPt2.setX(center.x()+r);
    painter->drawLine(leftPt1,leftPt2);
    painter->drawLine(rightPt1,rightPt2);
    painter->drawEllipse(adjustRect(2));

    //
    QPointF pt1 = center;
    QPointF pt2 = center;
    // to get the real 100 % radius, without recomputing
    pt1.setY(center.y()+r);
    pt2.setY(center.y()+4*r);
    pen.setColor(Qt::gray);
    painter->setPen(pen);
    painter->drawLine(pt1,pt2);

    // trapezium
    painter->setPen(Qt::gray);
    painter->setBrush(Qt::gray);
    QPolygonF trapPoly;
    QPointF tmpPt = center;
    tmpPt.setX(center.x()-r);
    tmpPt.setY(center.y()+4*r);
    trapPoly.append(tmpPt);
    tmpRct = adjustRect(position());
    trapPoly.append(getPoint(290,tmpRct));
    trapPoly.append(getPoint(250,tmpRct));
    tmpPt = center;
    tmpPt.setX(center.x()+r);
    tmpPt.setY(center.y()+4*r);
    trapPoly.append(tmpPt);
    painter->drawPolygon(trapPoly);
    painter->drawChord(tmpRct,-16*70,-16*40);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QcBar::QcBar(QWidget *parent): QWidget(parent) {}
QcBar::~QcBar() {}
void QcBar::paintEvent(QPaintEvent *)
{
    // draw the preparation work, enable anti-aliasing
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    // draw a gradient background
    drawBg(&painter);
    // draw progress
    drawProgress(&painter);

    if(direction==DirectionEnum::Horizontal) {
        // draw a ruler
        if (rulerTop) drawRulerTop(&painter);
        if (rulerBottom) drawRulerBottom(&painter);
    }
    else
    {
        // draw a ruler
        if (rulerRight) drawRulerRight(&painter);
        if (rulerLeft) drawRulerLeft(&painter);
    }

}
void QcBar::drawBg(QPainter *painter)
{
    painter->save();
    painter->setPen(lineColor);
    painter->setBrush(bgColor);
    painter->drawRect(rect());
    painter->restore();
}
void QcBar::drawProgress(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(progressColor);

    if(direction==DirectionEnum::Horizontal) {
        double length = width();
        double increment = length / (maxValue - minValue);
        double initX = (currentValue - minValue) * increment;

        QRect rect(0, 0, initX, height());
        painter->drawRect(rect);
    }
    else
    {
        double length = height();
        double increment = length / (maxValue - minValue);
        double initX = (currentValue - minValue) * increment;
        QRect rect(0, height()-initX, width(), initX);
        painter->drawRect(rect);
    }
    painter->restore();
}
void QcBar::drawRulerTop(QPainter *painter)
{
    painter->save();
    painter->setPen(lineColor);

    double initX = 0;

    // draw a part of the bottom line on the horizontal ruler
    double initTopY = 0;
    QPointF lineTopLeftPot = QPointF(initX, initTopY);
    QPointF lineTopRightPot = QPointF(width() - initX, initTopY);
    painter->drawLine(lineTopLeftPot, lineTopRightPot);

    // draw the upper part and the lower part of the horizontal ruler scale
    double length = width();
    // Calculate how much each cell moves
    double increment = length / (maxValue - minValue);
    //Long line short line length
    int longLineLen = 15;
    int shortLineLen = 10;

    //Draw scale value and scale value according to range value. Long line needs to move 10 pixels. Short line needs to move 5 pixels.
    for (int i = minValue; i <= maxValue; i = i + shortStep) {
        if (i % longStep == 0) {
            QPointF topPot = QPointF(initX, initTopY);
            QPointF bottomPot = QPointF(initX, initTopY + longLineLen);
            painter->drawLine(topPot, bottomPot);

            // The first value and the last value do not draw
            if (i == minValue || i == maxValue) {
                initX += increment * shortStep;
                continue;
            }

            QString strValue = QString("%1").arg((double)i, 0, 'f', precision);
            double textWidth = fontMetrics().width(strValue);
            double textHeight = fontMetrics().height();

            QPointF textPot = QPointF(initX - textWidth / 2, initTopY + textHeight + longLineLen);
            painter->drawText(textPot, strValue);
        } else {
            if (i % (longStep / 2) == 0) {
                shortLineLen = 10;
            } else {
                shortLineLen = 6;
            }

            QPointF topPot = QPointF(initX, initTopY);
            QPointF bottomPot = QPointF(initX, initTopY + shortLineLen);
            painter->drawLine(topPot, bottomPot);
        }

        initX += increment * shortStep;
    }

    painter->restore();
}
void QcBar::drawRulerBottom(QPainter *painter)
{
    painter->save();
    painter->setPen(lineColor);

    double initX = 0;

    // draw the bottom line of the lower part of the horizontal ruler
    double initBottomY = height();
    QPointF lineBottomLeftPot = QPointF(initX, initBottomY);
    QPointF lineBottomRightPot = QPointF(width() - initX, initBottomY);
    painter->drawLine(lineBottomLeftPot, lineBottomRightPot);

    // draw the upper part and the lower part of the horizontal ruler scale
    double length = width();
    // Calculate how much each cell moves
    double increment = length / (maxValue - minValue);
    //Long line short line length
    int longLineLen = 15;
    int shortLineLen = 10;

    //Draw scale value and scale value according to range value. Long line needs to move 10 pixels. Short line needs to move 5 pixels.
    for (int i = minValue; i <= maxValue; i = i + shortStep) {
        if (i % longStep == 0) {
            QPointF topPot = QPointF(initX, initBottomY);
            QPointF bottomPot = QPointF(initX, initBottomY - longLineLen);
            painter->drawLine(topPot, bottomPot);

            // The first value and the last value do not draw
            if (i == minValue || i == maxValue) {
                initX += increment * shortStep;
                continue;
            }

            QString strValue = QString("%1").arg((double)i, 0, 'f', precision);
            double textWidth = fontMetrics().width(strValue);
            double textHeight = fontMetrics().height();

            QPointF textPot = QPointF(initX - textWidth / 2, initBottomY - textHeight / 2 - longLineLen);
            painter->drawText(textPot, strValue);
        } else {
            if (i % (longStep / 2) == 0) {
                shortLineLen = 10;
            } else {
                shortLineLen = 6;
            }

            QPointF topPot = QPointF(initX, initBottomY);
            QPointF bottomPot = QPointF(initX, initBottomY - shortLineLen);
            painter->drawLine(topPot, bottomPot);
        }

        initX += increment * shortStep;
    }

    painter->restore();
}
void QcBar::drawRulerLeft(QPainter *painter)
{
    painter->save();
    painter->setPen(lineColor);

    double x = 0;
    double y = height();

    QPointF lineTopLeftPot = QPointF(x, y);
    QPointF lineBottomLeftPot = QPointF(x, y+height());
    painter->drawLine(lineBottomLeftPot,lineTopLeftPot);

    double length = height();
    // Calculate how much each cell moves
    double increment = length / (maxValue - minValue);
    //Long line short line length
    int longLineLen = 15;
    int shortLineLen = 10;

    //Draw scale value according to range value. Long line needs to move 10 pixels and short line needs to move 5 pixels.
    for (int i = minValue; i <= maxValue; i = i + shortStep) {
        if (i % longStep == 0) {
            QPointF leftPoint = QPointF(x, y);
            QPointF rightPoint = QPointF(x + longLineLen, y);
            painter->drawLine(leftPoint, rightPoint);

            // The first value and the last value do not draw
            if (i == minValue || i == maxValue) {
                y -= increment * shortStep;
                continue;
            }

            QString strValue = QString("%1").arg((double)i, 0, 'f', precision);
            double textWidth = fontMetrics().width(strValue);
            double textHeight = fontMetrics().height();

            //QPointF textPot = QPointF(x - textWidth / 2, y + textHeight + longLineLen);
            QPointF textPot = QPointF(x + textWidth/3 +longLineLen , y +textHeight/4);

            painter->drawText(textPot, strValue);
        } else {
            if (i % (longStep / 2) == 0) {
                shortLineLen = 10;
            } else {
                shortLineLen = 6;
            }

            QPointF leftP = QPointF(x, y);
            QPointF rightP = QPointF(x+ shortLineLen, y );
            painter->drawLine(leftP, rightP);
        }

        y -= increment * shortStep;
    }

    painter->restore();
}
void QcBar::drawRulerRight(QPainter *painter)
{
    painter->save();
    painter->setPen(lineColor);

    double x = width();
    double y = height();

    double length = height();
    // Calculate how much each cell moves
    double increment = length / (maxValue - minValue);
    //Long line short line length
    int longLineLen = 15;
    int shortLineLen = 10;

    //Draw scale value according to range value. Long line needs to move 10 pixels. Short line needs to move 5 pixels.
    for (int i = minValue; i <= maxValue; i = i + shortStep) {
        if (i % longStep == 0) {

            QPointF leftPoint = QPointF(x - longLineLen, y);
            QPointF rightPoint = QPointF(x, y);
            painter->drawLine(rightPoint, leftPoint);

            // The first value and the last value do not draw
            if (i == minValue || i == maxValue) {
                y -= increment * shortStep;
                continue;
            }

            QString strValue = QString("%1").arg((double)i, 0, 'f', precision);
            double textWidth = fontMetrics().width(strValue);
            double textHeight = fontMetrics().height();

            QPointF textPot = QPointF(x - longLineLen*2.5 - textWidth/2, y+ textHeight/4);

            painter->drawText(textPot, strValue);
        } else {
            if (i % (longStep / 2) == 0) {
                shortLineLen = 10;
            } else {
                shortLineLen = 6;
            }

            QPointF leftP = QPointF(x - shortLineLen, y );
            QPointF rightP = QPointF(x, y);
            painter->drawLine(rightP,leftP);
        }

        y -= increment * shortStep;
    }

    painter->restore();
}

QcBar::DirectionEnum QcBar::getDirection() const {return direction;}
double QcBar::getMinValue() const{ return minValue;}
double QcBar::getMaxValue() const{return maxValue;}
double QcBar::getValue() const{ return value;}
int QcBar::getPrecision() const{return precision;}
int QcBar::getLongStep() const{return longStep;}
int QcBar::getShortStep() const{return shortStep;}
bool QcBar::getRulerTop() const{return rulerTop;}
bool QcBar::getRulerBottom() const{return rulerBottom;}
bool QcBar::getRulerLeft() const{return rulerLeft;}
bool QcBar::getRulerRight() const{return rulerRight;}
QColor QcBar::getBgColor() const{return bgColor;}
QColor QcBar::getLineColor() const{return lineColor;}
QColor QcBar::getProgressColor() const{return progressColor;}

void QcBar::setDirection(DirectionEnum paintDirection) { direction=paintDirection;}
void QcBar::setCurrentValue(int value)
{
    if(value<minValue)
        currentValue=minValue;
    else if(value>maxValue)
        currentValue= maxValue;
    else
        currentValue=value;
    repaint();
}
void QcBar::setRange(double MinValue, double MaxValue){ minValue = MinValue; maxValue = MaxValue;}
void QcBar::setRange(int MinValue, int MaxValue){ minValue = MinValue; maxValue = MaxValue;}
void QcBar::setMinValue(double MinValue){ minValue=MinValue; }
void QcBar::setMaxValue(double MaxValue){ maxValue = MaxValue;}
void QcBar::setPrecision(int Precision){ precision =Precision;}
void QcBar::setLongStep(int LongStep){ longStep = LongStep;}
void QcBar::setShortStep(int ShortStep){ shortStep= ShortStep;}
void QcBar::setRulerTop(bool RulerTop){ rulerTop=RulerTop;}
void QcBar::setRulerBottom(bool RulerBottom){ rulerBottom=RulerBottom;}
void QcBar::setRulerLeft(bool RulerLeft) { rulerLeft=RulerLeft;}
void QcBar::setRulerRight(bool RulerRight) { rulerRight=RulerRight;}
void QcBar::setBgColor(const QColor &BgColor){ bgColor = BgColor;}
void QcBar::setLineColor(const QColor &LineColor){ lineColor = LineColor;}
void QcBar::setProgressColor(const QColor &ProgressColor){ progressColor = ProgressColor;}
