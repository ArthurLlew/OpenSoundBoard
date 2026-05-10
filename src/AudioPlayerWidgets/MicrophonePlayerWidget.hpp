#pragma once


// Qt widgets
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
// Audio player widget
#include <AudioPlayerWidgets/AudioPlayerWidget.hpp>
// Microphone rerouter
#include <AudioPlayers/MicrophonePlayer.hpp>


/**
 * Microphone player widget.
 */
class MicrophonePlayerWidget : public AudioPlayerWidget
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

    // Start/Stop button.
    QPushButton *buttonStartStop = nullptr;

public:

    /**
     * Constructor.
     * 
     * @param player Audio player
     * @param name player name
     */
    explicit MicrophonePlayerWidget(QTabWidget const *devices, QString name, QWidget *parent = nullptr);
    /**
     * Destructor.
     */
    ~MicrophonePlayerWidget();

private:

    /**
     * Start/Stop player.
     */
    void startStop() override;

public:

    /**
     * Stop player.
     */
    void stop() override;

signals:
    /**
     * Signals player to stop.
     */
    void askToStop();
};