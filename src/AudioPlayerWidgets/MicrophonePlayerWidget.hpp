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
#include <AudioPlayerWidgets/AudioPlayerWidget.hpp>
#include <AudioPlayers/MicrophonePlayer.hpp>


/** Microphone player widget.*/
class MicrophonePlayerWidget : public AudioPlayerWidget
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
    explicit MicrophonePlayerWidget(QTabWidget const *devices, QString name, QWidget *parent = nullptr);
    /** Destructor.*/
    ~MicrophonePlayerWidget();

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