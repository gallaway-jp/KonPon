// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QtWidgets>

#include "DragWidget.h"

//! [0]
DragWidget::DragWidget(QWidget* parent)
    : QFrame(parent)
{
    setMinimumSize(200, 200);
    setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
    setAcceptDrops(true);

    QLabel* boatIcon = new QLabel(this);
    //boatIcon->setPixmap(QPixmap(":/images/boat.png"));
    boatIcon->setText("Boat");
    boatIcon->move(10, 10);
    boatIcon->show();
    boatIcon->setAttribute(Qt::WA_DeleteOnClose);

    QLabel* carIcon = new QLabel(this);
    //carIcon->setPixmap(QPixmap(":/images/car.png"));
    carIcon->setText("Car");
    carIcon->move(100, 10);
    carIcon->show();
    carIcon->setAttribute(Qt::WA_DeleteOnClose);

    QLabel* houseIcon = new QLabel(this);
    //houseIcon->setPixmap(QPixmap(":/images/house.png"));
    houseIcon->setText("House");
    houseIcon->move(10, 80);
    houseIcon->show();
    houseIcon->setAttribute(Qt::WA_DeleteOnClose);
}
//! [0]

void DragWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }
        else {
            event->acceptProposedAction();
        }
    }
    else {
        event->ignore();
    }
}

void DragWidget::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }
        else {
            event->acceptProposedAction();
        }
    }
    else {
        event->ignore();
    }
}

void DragWidget::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        //QPixmap pixmap;
        QString text;
        QPoint offset;
        dataStream /*>> pixmap*/ >> text >> offset;

        QLabel* newIcon = new QLabel(this);
        //newIcon->setPixmap(pixmap);
        newIcon->setText(text);
        newIcon->move(event->position().toPoint() - offset);
        newIcon->show();
        newIcon->setAttribute(Qt::WA_DeleteOnClose);

        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }
        else {
            event->acceptProposedAction();
        }
    }
    else {
        event->ignore();
    }
}

//! [1]
void DragWidget::mousePressEvent(QMouseEvent* event)
{
    QLabel* child = static_cast<QLabel*>(childAt(event->position().toPoint()));
    if (!child)
        return;

    //QPixmap pixmap = child->pixmap();
    QString text = child->text();

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream /*<< pixmap*/ << text << QPoint(event->position().toPoint() - child->pos());
    //! [1]

    //! [2]
    QMimeData* mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);
    //! [2]

    //! [3]
    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);
    //drag->setPixmap()
    //drag->setPixmap(pixmap);
    drag->setHotSpot(event->position().toPoint() - child->pos());
    //! [3]

    /*QPixmap tempPixmap = pixmap;
    QPainter painter;
    painter.begin(&tempPixmap);
    painter.fillRect(pixmap.rect(), QColor(127, 127, 127, 127));
    painter.end();

    child->setPixmap(tempPixmap);*/

    if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction) {
        child->close();
    }
    else {
        child->show();
        child->setText(text);
        //child->setPixmap(pixmap);
    }
}
