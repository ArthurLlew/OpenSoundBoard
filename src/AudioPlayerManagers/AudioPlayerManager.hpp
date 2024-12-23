#pragma once

// Qt
#include <QtCore/QObject>
#include <QtCore/QMimeData>
#include <QtCore/QString>
#include <QtCore/QThreadPool>
#include <QtCore/QRunnable>
#include <QtGui/QPainter>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressbar>
// Custom
#include <WidgetMessages.hpp>
#include <AudioPlayers/AudioPlayer.hpp>


/** Audio player manager widget. Can run, stop and wait provided audio player.*/
class AudioPlayerManager : public QWidget, WidgetWarnings
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

protected:

    /** Manager name.*/
    QString name;
    /** Widget main layout.*/
    QVBoxLayout *layout = nullptr;

    /** Thread pool where player will run.*/
    QThreadPool *threadpool = nullptr;
    /** Audio player.*/
    AudioPlayer *player = nullptr;

public:

    /** Constructor.
     * 
     *  @param player Audio player.
     *  @param name player name.
    */
    explicit AudioPlayerManager(AudioPlayer *player, QString name, QWidget *parent = nullptr);
    /** Destructor.*/
    ~AudioPlayerManager();

protected:

    /** Handles paint event.*/
    void paintEvent(QPaintEvent *) override;

    /** Display player error.
     * 
     *  @param message error message.
    */
    void playerError(QString message);

    /** Start/Stop player.*/
    virtual void playerStartStop() = 0;

public:

    /** Stop player.*/
    virtual void playerStop() = 0;

    /** Updates devices in running player.*/
    virtual void updateDevices() = 0;

signals:
    /** Ask player to update devices.*/
    void askToUpdateDevices();
};