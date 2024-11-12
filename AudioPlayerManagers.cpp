#include "AudioPlayerManagers.hpp"


AudioPlayerManager::AudioPlayerManager(AudioPlayer *player, QString name, QWidget *parent)
: QWidget(parent)
{
    this->player = player;
    this->name = name;

    // Create threadpool and disable thread expiry (only we terminate threads explicitly)
    threadpool = new QThreadPool();
    threadpool->setExpiryTimeout(-1);

    // Connect signals to player
    connect(this, &AudioPlayerManager::ask_player_stop, this->player, AudioPlayer::stop);
    connect(this->player, AudioPlayer::signalError, this, &AudioPlayerManager::playerError);

    /*
    // Main layout:
    */
    layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop);
    setLayout(layout);

    /*
    // Header layout:
    */
    QHBoxLayout *header_layout = new QHBoxLayout();
    header_layout->setAlignment(Qt::AlignLeft);
    layout->addLayout(header_layout);
    // Start/Stop button
    buttonStartStop = new QPushButton("Start");
    connect(buttonStartStop, &QPushButton::pressed, this, &AudioPlayerManager::player_run_stop);
    header_layout->addWidget(buttonStartStop);
    // Label
    QLabel *label = new QLabel(name);
    header_layout->addWidget(label);
}


AudioPlayerManager::~AudioPlayerManager()
{
    // Delete threadpool and player
    delete player;
    delete threadpool;
}


bool AudioPlayerManager::player_state()
{
    return isPlayerAlive;
}


void AudioPlayerManager::player_run()
{
    // Start player only if he is not working
    if (!isPlayerAlive)
    {
        // Start player
        threadpool->start(player);
        // Update button
        buttonStartStop->setText("Stop");
        // Update status
        isPlayerAlive = true;
    }
}


void AudioPlayerManager::player_stop()
{
    // Kill player only if he is working
    if (isPlayerAlive)
    {
        emit ask_player_stop();
        // Update button
        buttonStartStop->setText("Start");
        // Update status
        isPlayerAlive = false;
    }
}


void AudioPlayerManager::player_wait()
{
    threadpool->waitForDone(-1);
}


void AudioPlayerManager::player_run_stop()
{
    if (isPlayerAlive)
    {
        // Stop player and wait for it to finish
        player_stop();
        player_wait();
    }
    else
    {
        player_run();
    }
}


void AudioPlayerManager::playerError(QString message)
{
    displayWarning(name + " error:\n" + message);
    // Update player state
    player_stop();
}


MicrophonePlayerManager::MicrophonePlayerManager(QTabWidget *devices, QString name, QWidget *parent)
// Init of the player happens here
: AudioPlayerManager(new MicrophonePlayer(devices), name, parent) {}


MediaFilesPlayerManager::MediaFilesPlayerManager(QTabWidget *devices, QString name, QRect *screeanGeometry, QWidget *parent)
// Init of the player happens here
: AudioPlayerManager(new MediaFilesPlayer(devices, &volume), name, parent)
{
    // Connect signals to player
    connect((MediaFilesPlayer*)this->player, MediaFilesPlayer::signalTrackEnd, this, &MediaFilesPlayerManager::playerTrackEnded);
    connect(this, &MediaFilesPlayerManager::askNewTrack, (MediaFilesPlayer*)this->player, MediaFilesPlayer::setNewTrack);
    connect(this, &MediaFilesPlayerManager::askNewTrackState, (MediaFilesPlayer*)this->player, MediaFilesPlayer::setNewTrackState);

    /*
    // Additional layouts:
    */
    QHBoxLayout *box_layout1 = new QHBoxLayout();
    layout->addLayout(box_layout1);
    QHBoxLayout *box_layout2 = new QHBoxLayout();
    layout->addLayout(box_layout2);
    QHBoxLayout *box_layout3 = new QHBoxLayout();
    box_layout3->setAlignment(Qt::AlignLeft);
    layout->addLayout(box_layout3);
    /*
    // Other widgets:
    */
    // Name
    trackName = new QLabel("<No track>");
    box_layout1->addWidget(trackName);
    // Track progress
    progress = new QProgressBar();
    progress->setMinimum(0);
    progress->setMaximum(duration_total);
    box_layout2->addWidget(progress);
    // Play/Pause button
    buttonPlay = new QPushButton("Play");
    connect(buttonPlay, &QPushButton::pressed, this, &MediaFilesPlayerManager::trackPlayPause);
    box_layout3->addWidget(buttonPlay);
    // Stop buttom
    QPushButton *button_stop = new QPushButton("Stop");
    connect(button_stop, &QPushButton::pressed, this, &MediaFilesPlayerManager::trackStop);
    box_layout3->addWidget(button_stop);
    // Volume slider and label
    QSlider *volume_slider = new QSlider(Qt::Orientation::Horizontal);
    volumeLabel = new QLabel(QString::number(volume_slider->value()));
    connect(volume_slider, &QSlider::valueChanged, this, &MediaFilesPlayerManager::setVolume);
    volume_slider->setRange(0,100);
    volume_slider->setValue(30);
    volume_slider->setMinimumWidth(screeanGeometry->width()/14);
    volume_slider->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    box_layout3->addWidget(volume_slider);
    box_layout3->addWidget(volumeLabel);
}


void MediaFilesPlayerManager::playerError(QString message)
{
    // Update track state
    playerTrackEnded();
    // Call parent method
    AudioPlayerManager::playerError(message);
}


void MediaFilesPlayerManager::setVolume(int value)
{
    // Update volume var and label
    volume = ((float)value)/100;
    volumeLabel->setText(QString::number(value));
}


void MediaFilesPlayerManager::updateDuration()
{
    progress->setValue(duration_cur);
}


void MediaFilesPlayerManager::trackInsert(QString filepath, QString name)
{
    // Ask player to change track
    emit askNewTrack(filepath);
    // Update track name
    trackName->setText(name);
}


void MediaFilesPlayerManager::trackStop()
{
    // New track state
    trackState = STOPPED;
    emit askNewTrackState(STOPPED);
    // Update button
    buttonPlay->setText("Play");
}


void MediaFilesPlayerManager::trackPlayPause()
{
    // Play-pause track
    switch (trackState)
    {
        case STOPPED:
        case PAUSED:
            // New track state
            trackState = PLAYING;
            emit askNewTrackState(PLAYING);
            // Update button
            buttonPlay->setText("Pause");
            break;
        case PLAYING:
            // New track state
            trackState = PAUSED;
            emit askNewTrackState(PAUSED);
            // Update button
            buttonPlay->setText("Play");
            break;
    }
}


void MediaFilesPlayerManager::playerTrackEnded()
{
    // New track state
    trackState = STOPPED;
    // Update button
    buttonPlay->setText("Play");
}