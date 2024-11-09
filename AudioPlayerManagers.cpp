#include "AudioPlayerManagers.hpp"


AudioPlayerManager::AudioPlayerManager(QString name, QWidget *parent) : QWidget(parent)
{
    this->name = name;

    /*
    // Main layout:
    */
    layout = new QVBoxLayout();
    setLayout(layout);

    /*
    // Header layout:
    */
    QHBoxLayout *header_layout = new QHBoxLayout();
    layout->addLayout(header_layout);
    // Start/Stop button
    button_start_stop = new QPushButton("Start");
    connect(button_start_stop, &QPushButton::pressed, this, &AudioPlayerManager::start_stop);
    header_layout->addWidget(button_start_stop);
    // Label
    QLabel *label = new QLabel(name);
    header_layout->addWidget(label);
}


AudioPlayerManager::~AudioPlayerManager()
{
    // Delete threadpool and player
    delete threadpool;
    delete player;
}


void AudioPlayerManager::start_stop()
{
    if (is_player_alive)
    {
        // Stop player and wait for it to finish
        stop();
        wait_player();
    }
    else
    {
        start();
    }
}


void AudioPlayerManager::start()
{
    // Start player only if he is not working
    if (!is_player_alive)
    {
        // Start player
        threadpool->start(player);
        // Update button
        button_start_stop->setText("Stop");
        // Update status
        is_player_alive = true;
    }
}


void AudioPlayerManager::stop()
{
    // Kill player only if he is working
    if (is_player_alive)
    {
        player->kill();
        // Update button
        button_start_stop->setText("Start");
        // Update status
        is_player_alive = false;
    }
}


void AudioPlayerManager::wait_player()
{
    threadpool->waitForDone(-1);
}


void AudioPlayerManager::player_error(QString message)
{
    show_warning(name + "error:\n" + message);
}


MicrophonePlayerManager::MicrophonePlayerManager(QTabWidget *devices, QString name, QWidget *parent)
: AudioPlayerManager(name, parent)
{
    // Create microphone player
    this->player = new MicrophonePlayer(devices);
}


MediaFilesPlayerManager::MediaFilesPlayerManager(QTabWidget *devices, QListWidget *tracks, QString name, QWidget *parent)
: AudioPlayerManager(name, parent)
{
    // Create media files player
    this->player = new MediaFilesPlayer(devices, tracks);
}