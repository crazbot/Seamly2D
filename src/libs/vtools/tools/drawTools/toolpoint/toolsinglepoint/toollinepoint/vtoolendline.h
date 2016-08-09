/************************************************************************
 **
 **  @file   vtoolendline.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   November 15, 2013
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentine project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2013-2015 Valentina project
 **  <https://bitbucket.org/dismine/valentina> All Rights Reserved.
 **
 **  Valentina is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  Valentina is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with Valentina.  If not, see <http://www.gnu.org/licenses/>.
 **
 *************************************************************************/

#ifndef VTOOLENDLINE_H
#define VTOOLENDLINE_H

#include <qcompilerdetection.h>
#include <QDomElement>
#include <QGraphicsItem>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QtGlobal>

#include "../ifc/xml/vabstractpattern.h"
#include "../vpatterndb/vformula.h"
#include "../vmisc/def.h"
#include "vtoollinepoint.h"

class DialogTool;
class QDomElement;
class QGraphicsSceneContextMenuEvent;
class VContainer;
class VGObject;
class VMainGraphicsScene;
template <class T> class QSharedPointer;

/**
 * @brief The VToolEndLine class tool for creation point on the line end.
 */
class VToolEndLine : public VToolLinePoint
{
    Q_OBJECT
public:
    virtual ~VToolEndLine() Q_DECL_OVERRIDE;
    virtual void setDialog() Q_DECL_OVERRIDE;
    static VToolEndLine *Create(DialogTool *dialog, VMainGraphicsScene  *scene, VAbstractPattern *doc,
                                VContainer *data);
    static VToolEndLine *Create(const quint32 _id, const QString &pointName, const QString &typeLine,
                                const QString &lineColor, QString &formulaLength, QString &formulaAngle,
                                const quint32 &basePointId, const qreal &mx, const qreal &my,
                                VMainGraphicsScene  *scene, VAbstractPattern *doc, VContainer *data,
                                const Document &parse,
                                const Source &typeCreation);
    static const QString ToolType;
    virtual int  type() const Q_DECL_OVERRIDE {return Type;}
    enum { Type = UserType + static_cast<int>(Tool::EndLine)};

    VFormula     GetFormulaAngle() const;
    void         SetFormulaAngle(const VFormula &value);
    virtual void ShowVisualization(bool show) Q_DECL_OVERRIDE;
protected:
    virtual void contextMenuEvent ( QGraphicsSceneContextMenuEvent * event ) Q_DECL_OVERRIDE;
    virtual void SaveDialog(QDomElement &domElement) Q_DECL_OVERRIDE;
    virtual void SaveOptions(QDomElement &tag, QSharedPointer<VGObject> &obj) Q_DECL_OVERRIDE;
    virtual void ReadToolAttributes(const QDomElement &domElement) Q_DECL_OVERRIDE;
    virtual void SetVisualization() Q_DECL_OVERRIDE;
private:
    Q_DISABLE_COPY(VToolEndLine)

    QString formulaAngle;

    VToolEndLine(VAbstractPattern *doc, VContainer *data, const quint32 &id, const QString &typeLine,
                 const QString &lineColor,
                 const QString &formulaLength, const QString &formulaAngle, const quint32 &basePointId,
                 const Source &typeCreation, QGraphicsItem * parent = nullptr);
};

#endif // VTOOLENDLINE_H
