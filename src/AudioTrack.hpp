#pragma once

// Qt
#include <QtCore/QPoint>
#include <QtCore/QMimeData>
#include <QtCore/QString>
#include <QtGui/QMouseEvent>
#include <QtGui/QDrag>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>


/** Describes audio track loaded by user.*/
class AudioTrack: public QWidget
{
    /** Media file path.*/
    QString filepath;
    /** Media file name.*/
    QString name;
    /** Saved value of mouse position when widget as clicked.*/
    QPoint dragStartPosition;

public:

    /** Constructor.
     * 
     *  @param filepath Path to the media file.
     *  @param player Media files player manager.
    */
    explicit AudioTrack(QString filepath, QWidget *parent = nullptr);

protected:

    /** Invoked when mouse is pressed.*/
    void mousePressEvent(QMouseEvent *event);
    /** Invoked when mouse was moved.*/
    void mouseMoveEvent(QMouseEvent *event);
};