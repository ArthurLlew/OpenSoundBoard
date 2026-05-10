#pragma once


// Qt core
#include <QtCore/QMimeData>
#include <QtCore/QThreadPool>
// Qt GUI
#include <QtGui/QPainter>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
// Qt widgets
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
// Message boxes
#include <WidgetMessageBoxing/WidgetWarning.cpp>
// Audio player
#include <AudioPlayers/AudioPlayer.hpp>


/**
 * Audio player widget.
 */
class AudioPlayerWidget : public QWidget, WidgetWarning
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

protected:

    // Manager name.
    QString name;
    // Widget main layout.
    QVBoxLayout *layout = nullptr;

    // Thread pool where player will run.
    QThreadPool *threadpool = nullptr;
    // Audio player.
    AudioPlayer *player = nullptr;

public:

    /**
     * Constructor.
     * 
     * @param player Audio player
     * @param name player name
     */
    explicit AudioPlayerWidget(AudioPlayer *player, QString name, QWidget *parent = nullptr);
    /**
     * Destructor.
     */
    ~AudioPlayerWidget();

protected:

    /**
     * Reimplemented to allow usage of QSS.
     */
    void paintEvent(QPaintEvent *) override;

    /**
     * Display player error.
     * 
     * @param message error message
     */
    void playerError(QString message);

    /**
     * Start/Stop player.
     */
    virtual void startStop() = 0;

public:

    /**
     * Stop player.
     */
    virtual void stop() = 0;

    /**
     * Updates devices in running player.
     */
    void updateDevices();

signals:
    /**
     * Signals player to update devices.
     */
    void askToUpdateDevices();
};