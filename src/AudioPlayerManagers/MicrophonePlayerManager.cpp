#include <AudioPlayerManagers/MicrophonePlayerManager.hpp>


MicrophonePlayerManager::MicrophonePlayerManager(QTabWidget const *devices, QString name, QWidget *parent)
// Init of the player happens here
: AudioPlayerManager(new MicrophonePlayer(devices), name, parent)
{
    // Connect signals to player
    connect(this, &MicrophonePlayerManager::askToStop, (MicrophonePlayer*)this->player, MicrophonePlayer::stop);
    connect(this, &MicrophonePlayerManager::askToUpdateDevices, (MicrophonePlayer*)this->player, MicrophonePlayer::updateAudioStreams);

    /*
    // Header layout:
    */
    QHBoxLayout *header_layout = new QHBoxLayout();
    header_layout->setAlignment(Qt::AlignLeft);
    layout->addLayout(header_layout);
    // Start/Stop button
    buttonStartStop = new QPushButton("Start");
    connect(buttonStartStop, &QPushButton::pressed, this, &MicrophonePlayerManager::playerStartStop);
    header_layout->addWidget(buttonStartStop);
    // Label
    QLabel *label = new QLabel(name);
    header_layout->addWidget(label);
}


MicrophonePlayerManager::~MicrophonePlayerManager()
{
    // Stop player
    playerStop();
}


void MicrophonePlayerManager::playerStop()
{
    // Kill player only if he is working
    if (isRunning)
    {
        // Stop player and wait for it to finish
        emit askToStop();
        threadpool->waitForDone(-1);
        // Update state
        isRunning = false;
        // Update button
        buttonStartStop->setText("Start");
    }
}


void MicrophonePlayerManager::playerStartStop()
{
    if (isRunning)
    {
        playerStop();
    }
    else
    {
        // Start player
        threadpool->start(player);
        // Update state
        isRunning = true;
        // Update button
        buttonStartStop->setText("Stop");
    }
}


void MicrophonePlayerManager::updateDevices()
{
    emit askToUpdateDevices();
}