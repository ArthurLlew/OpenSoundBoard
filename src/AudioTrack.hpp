#pragma once


// Qt core
#include <QtCore/QPoint>
#include <QtCore/QMimeData>
#include <QtCore/QString>
// Qt GUI
#include <QtGui/QMouseEvent>
#include <QtGui/QDrag>
// Qt widgets
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>


/**
 * Info of loaded audio track.
 */
class AudioTrack: public QWidget
{
    // Media file path.
    QString filepath;
    // Media file name.
    QString name;
    // Saved value of mouse position when widget as clicked.
    QPoint dragStartPosition;

public:

    /**
     * Constructor.
     * 
     * @param filepath path to the media file
     */
    explicit AudioTrack(QString filepath, QWidget *parent = nullptr);

protected:

    /**
     * Invoked when mouse is pressed.
     */
    void mousePressEvent(QMouseEvent *event);
    /**
     * Invoked when mouse was moved.
     */
    void mouseMoveEvent(QMouseEvent *event);
};