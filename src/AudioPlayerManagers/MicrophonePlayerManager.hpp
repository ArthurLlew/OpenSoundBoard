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
#include <AudioPlayerManagers/AudioPlayerManager.hpp>
#include <AudioPlayers/MicrophonePlayer.hpp>


/** Microphone player manager.*/
class MicrophonePlayerManager : public AudioPlayerManager
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

    /** Audio player state.*/
    bool isRunning = false;

    /** Start/Stop button.*/
    QPushButton *buttonStartStop = nullptr;

public:

    /** Constructor.
     * 
     *  @param player Audio player.
     *  @param name player name.
    */
    explicit MicrophonePlayerManager(QTabWidget const *devices, QString name, QWidget *parent = nullptr);
    /** Destructor.*/
    ~MicrophonePlayerManager();

private:

    /** Start/Stop player.*/
    void startStop();

public:

    /** Stop player.*/
    void stop();

    /** Updates devices in running player.*/
    void updateDevices();

signals:
    /** Ask player to stop.*/
    void askToStop();
};