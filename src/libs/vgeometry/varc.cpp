/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2017  Seamly, LLC                                       *
 *                                                                         *
 *   https://github.com/fashionfreedom/seamly2d                            *
 *                                                                         *
 ***************************************************************************
 **
 **  Seamly2D is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  Seamly2D is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with Seamly2D.  If not, see <http://www.gnu.org/licenses/>.
 **
 **************************************************************************

 ************************************************************************
 **
 **  @file   varc.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   November 15, 2013
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentine project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2013-2015 Seamly2D project
 **  <https://github.com/fashionfreedom/seamly2d> All Rights Reserved.
 **
 **  Seamly2D is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  Seamly2D is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with Seamly2D.  If not, see <http://www.gnu.org/licenses/>.
 **
 *************************************************************************/

#include "varc.h"

#include <QLineF>
#include <QPointF>

#include "../vmisc/def.h"
#include "../vmisc/vmath.h"
#include "../ifc/ifcdef.h"
#include "vabstractcurve.h"
#include "varc_p.h"
#include "vspline.h"

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VArc default constructor.
 */
VArc::VArc ()
    : VAbstractArc(GOType::Arc),
      d (new VArcData)
{}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VArc constructor.
 * @param center center point.
 * @param radius arc radius.
 * @param f1 start angle (degree).
 * @param f2 end angle (degree).
 */

#ifdef Q_COMPILER_RVALUE_REFS
VArc &VArc::operator=(VArc &&arc) Q_DECL_NOTHROW
{ Swap(arc); return *this; }
#endif

void VArc::Swap(VArc &arc) Q_DECL_NOTHROW
{ VAbstractArc::Swap(arc); std::swap(d, arc.d); }

VArc::VArc (const VPointF &center, qreal radius, const QString &formulaRadius, qreal f1, const QString &formulaF1,
            qreal f2, const QString &formulaF2, quint32 idObject, Draw mode)
    : VAbstractArc(GOType::Arc, center, f1, formulaF1, f2, formulaF2, idObject, mode),
      d (new VArcData(radius, formulaRadius))
{
    CreateName();
}

//---------------------------------------------------------------------------------------------------------------------
VArc::VArc(const VPointF &center, qreal radius, qreal f1, qreal f2)
    : VAbstractArc(GOType::Arc, center, f1, f2, NULL_ID, Draw::Calculation),
      d (new VArcData(radius))
{
    CreateName();
}

//---------------------------------------------------------------------------------------------------------------------
VArc::VArc(qreal length, const QString &formulaLength, const VPointF &center, qreal radius,
           const QString &formulaRadius, qreal f1, const QString &formulaF1, quint32 idObject, Draw mode)
    : VAbstractArc(GOType::Arc, formulaLength, center, f1, formulaF1, idObject, mode),
      d (new VArcData(radius, formulaRadius))
{
    CreateName();
    FindF2(length);
}

//---------------------------------------------------------------------------------------------------------------------
VArc::VArc(qreal length, const VPointF &center, qreal radius, qreal f1)
    : VAbstractArc(GOType::Arc, center, f1, NULL_ID, Draw::Calculation),
      d (new VArcData(radius))
{
    CreateName();
    FindF2(length);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VArc copy constructor
 * @param arc arc
 */
VArc::VArc(const VArc &arc)
    : VAbstractArc(arc), d (arc.d)
{}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief operator = assignment operator
 * @param arc arc
 * @return arc
 */
VArc &VArc::operator =(const VArc &arc)
{
    if ( &arc == this )
    {
        return *this;
    }
    VAbstractArc::operator=(arc);
    d = arc.d;
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
VArc VArc::Rotate(const QPointF &originPoint, qreal degrees, const QString &prefix) const
{
    const VPointF center = GetCenter().Rotate(originPoint, degrees);

    const QPointF p1 = VPointF::RotatePF(originPoint, GetP1(), degrees);
    const QPointF p2 = VPointF::RotatePF(originPoint, GetP2(), degrees);

    const qreal f1 = QLineF(static_cast<QPointF>(center), p1).angle();
    const qreal f2 = QLineF(static_cast<QPointF>(center), p2).angle();

    VArc arc(center, GetRadius(), f1, f2);
    arc.setName(name() + prefix);
    arc.SetColor(GetColor());
    arc.SetPenStyle(GetPenStyle());
    arc.SetFlipped(IsFlipped());
    return arc;
}

//---------------------------------------------------------------------------------------------------------------------
VArc VArc::Flip(const QLineF &axis, const QString &prefix) const
{
    const VPointF center = GetCenter().Flip(axis);

    const QPointF p1 = VPointF::FlipPF(axis, GetP1());
    const QPointF p2 = VPointF::FlipPF(axis, GetP2());

    const qreal f1 = QLineF(static_cast<QPointF>(center), p1).angle();
    const qreal f2 = QLineF(static_cast<QPointF>(center), p2).angle();

    VArc arc(center, GetRadius(), f1, f2);
    arc.setName(name() + prefix);
    arc.SetColor(GetColor());
    arc.SetPenStyle(GetPenStyle());
    arc.SetFlipped(not IsFlipped());
    return arc;
}

//---------------------------------------------------------------------------------------------------------------------
VArc VArc::Move(qreal length, qreal angle, const QString &prefix) const
{
    const VPointF center = GetCenter().Move(length, angle);

    const QPointF p1 = VPointF::MovePF(GetP1(), length, angle);
    const QPointF p2 = VPointF::MovePF(GetP2(), length, angle);

    const qreal f1 = QLineF(static_cast<QPointF>(center), p1).angle();
    const qreal f2 = QLineF(static_cast<QPointF>(center), p2).angle();

    VArc arc(center, GetRadius(), f1, f2);
    arc.setName(name() + prefix);
    arc.SetColor(GetColor());
    arc.SetPenStyle(GetPenStyle());
    arc.SetFlipped(IsFlipped());
    return arc;
}

//---------------------------------------------------------------------------------------------------------------------
VArc::~VArc()
{}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief GetLength return arc length.
 * @return length.
 */
qreal VArc::GetLength() const
{
    qreal length = d->radius * qDegreesToRadians(AngleArc());
    if (IsFlipped())
    {
        length *= -1;
    }

    return length;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief GetP1 return point associated with start angle.
 * @return point.
 */
QPointF VArc::GetP1() const
{
    QPointF p1 ( GetCenter().x () + d->radius, GetCenter().y () );
    QLineF centerP1(static_cast<QPointF>(GetCenter()), p1);
    centerP1.setAngle(GetStartAngle());
    return centerP1.p2();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief GetP2 return point associated with end angle.
 * @return точку point.
 */
QPointF VArc::GetP2 () const
{
    QPointF p2 ( GetCenter().x () + d->radius, GetCenter().y () );
    QLineF centerP2(static_cast<QPointF>(GetCenter()), p2);
    centerP2.setAngle(GetEndAngle());
    return centerP2.p2();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief GetPoints return list of points needed for drawing arc.
 * @return list of points
 */
QVector<QPointF> VArc::getPoints() const
{
    QVector<QPointF> points;
    QVector<qreal> sectionAngle;

    QPointF pStart;
    IsFlipped() ? pStart = GetP2() : pStart = GetP1();

    {
        qreal angle = AngleArc();

        if (qFuzzyIsNull(angle))
        {
            points.append(pStart);
            return points;
        }

        if (angle > 360 || angle < 0)
        {// Filter incorect value
            QLineF dummy(0,0, 100, 0);
            dummy.setAngle(angle);
            angle = dummy.angle();
        }

        const qreal angleInterpolation = 45; //degree
        const int sections = qFloor(angle / angleInterpolation);
        for (int i = 0; i < sections; ++i)
        {
            sectionAngle.append(angleInterpolation);
        }

        const qreal tail = angle - sections * angleInterpolation;
        if (tail > 0)
        {
            sectionAngle.append(tail);
        }
    }

    for (int i = 0; i < sectionAngle.size(); ++i)
    {
        const qreal lDistance = GetRadius() * 4.0/3.0 * qTan(qDegreesToRadians(sectionAngle.at(i)) * 0.25);

        const QPointF center = static_cast<QPointF>(GetCenter());

        QLineF lineP1P2(pStart, center);
        lineP1P2.setAngle(lineP1P2.angle() - 90.0);
        lineP1P2.setLength(lDistance);

        QLineF lineP4P3(center, pStart);
        lineP4P3.setAngle(lineP4P3.angle() + sectionAngle.at(i));
        lineP4P3.setLength(GetRadius());//in case of computing error
        lineP4P3 = QLineF(lineP4P3.p2(), center);
        lineP4P3.setAngle(lineP4P3.angle() + 90.0);
        lineP4P3.setLength(lDistance);

        VSpline spl(VPointF(pStart), lineP1P2.p2(), lineP4P3.p2(), VPointF(lineP4P3.p1()), 1.0);
        QVector<QPointF> splPoints = spl.getPoints();
        if (not splPoints.isEmpty() && i != sectionAngle.size() - 1)
        {
            splPoints.removeLast();
        }

        points << splPoints;
        pStart = lineP4P3.p1();
    }
    return points;
}

//---------------------------------------------------------------------------------------------------------------------
QVector<QLineF> VArc::getSegments() const
{
    QVector<QPointF> points = getPoints();
    QVector<QLineF> lines;
    if (points.size() >= 2)
    {
        for (int i=0; i < points.size()-1; ++i)
        {
            QLineF segment = QLineF(points.at(i), points.at(i+1));
            if (segment.length() > 0)
            {
                lines.append(segment);
            }
        }
    }
    return lines;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief CutArc cut arc into two segments.
 * @param length length first arc segment.
 * @param segment1 first arc segment.
 * @param segment2 second arc segment.
 * @return point cutting
 */
QPointF VArc::CutArc(qreal length, VArc &segment1, VArc &segment2) const
{
    //Always need to return two arc segments, so we must correct wrong length.
    const qreal minLength = ToPixel(2, Unit::Mm);
    const qreal fullLength = GetLength();

    if (qAbs(fullLength) <= minLength)
    {
        segment1 = VArc();
        segment2 = VArc();
        return QPointF();
    }

    QLineF line(static_cast<QPointF>(GetCenter()), GetP1());

    if (not IsFlipped())
    {
        if (length < 0)
        {
            length = fullLength + length;
        }
        length = qBound(ToPixel(1, Unit::Mm), length, fullLength - ToPixel(1, Unit::Mm));

        line.setAngle(line.angle() + qRadiansToDegrees(length/d->radius));
    }
    else
    {
        if (length > 0)
        {
            length = fullLength + length;
        }
        length = qBound(fullLength + ToPixel(1, Unit::Mm), length, ToPixel(-1, Unit::Mm));

        line.setAngle(line.angle() - qRadiansToDegrees(qAbs(length)/d->radius));
    }

    segment1 = VArc (GetCenter(), d->radius, d->formulaRadius, GetStartAngle(), GetFormulaF1(), line.angle(),
                 QString().setNum(line.angle()), getIdObject(), getMode());
    segment1.SetFlipped(IsFlipped());

    segment2 = VArc (GetCenter(), d->radius, d->formulaRadius, line.angle(), QString().setNum(line.angle()), GetEndAngle(),
                 GetFormulaF2(), getIdObject(), getMode());
    segment2.SetFlipped(IsFlipped());

    return line.p2();
}


//---------------------------------------------------------------------------------------------------------------------
QPointF VArc::CutArc(qreal length) const
{
    VArc segment1;
    VArc segment2;
    return this->CutArc(length, segment1, segment2);
}

//---------------------------------------------------------------------------------------------------------------------
void VArc::CreateName()
{
    QString name = ARC_ + QString("%1").arg(this->GetCenter().name());

    if (VAbstractCurve::id() != NULL_ID)
    {
        name += QString("_%1").arg(VAbstractCurve::id());
    }

    if (GetDuplicate() > 0)
    {
        name += QString("_%1").arg(GetDuplicate());
    }

    setName(name);
}

//---------------------------------------------------------------------------------------------------------------------
void VArc::FindF2(qreal length)
{
    SetFlipped(length < 0);

    if (length >= MaxLength())
    {
        length = MaxLength();
    }

    qreal arcAngle = qAbs(qRadiansToDegrees(length/d->radius));

    if (IsFlipped())
    {
        arcAngle = arcAngle * -1;
    }

    QLineF startAngle(0, 0, 100, 0);
    startAngle.setAngle(GetStartAngle() + arcAngle);// We use QLineF just because it is easy way correct angle value

    SetFormulaF2(QString().number(startAngle.angle()), startAngle.angle());
}

//---------------------------------------------------------------------------------------------------------------------
qreal VArc::MaxLength() const
{
    return M_2PI*d->radius;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief GetRadius return arc radius.
 * @return radius.
 */
QString VArc::GetFormulaRadius() const
{
    return d->formulaRadius;
}

//---------------------------------------------------------------------------------------------------------------------
void VArc::SetFormulaRadius(const QString &formula, qreal value)
{
    d->formulaRadius = formula;
    d->radius = value;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief GetRadius return formula for radius.
 * @return string with formula.
 */
qreal VArc::GetRadius() const
{
    return d->radius;
}
