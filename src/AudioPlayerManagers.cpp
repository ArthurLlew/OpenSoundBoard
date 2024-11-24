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
    connect(this, &AudioPlayerManager::askPlayerStop, this->player, AudioPlayer::stop);
    connect(this->player, AudioPlayer::signalError, this, &AudioPlayerManager::playerError);

    // Allow dropping of draggable widgets
    setAcceptDrops(true);

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
    connect(buttonStartStop, &QPushButton::pressed, this, &AudioPlayerManager::playerRunStop);
    header_layout->addWidget(buttonStartStop);
    // Label
    QLabel *label = new QLabel(name);
    header_layout->addWidget(label);
}


AudioPlayerManager::~AudioPlayerManager()
{
    // Stop and wait player
    playerStop();
    playerWait();

    // Delete player and threadpool
    delete player;
    delete threadpool;
}


void AudioPlayerManager::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


bool AudioPlayerManager::playerState()
{
    return isPlayerAlive;
}


void AudioPlayerManager::playerRun()
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


void AudioPlayerManager::playerStop()
{
    // Kill player only if he is working
    if (isPlayerAlive)
    {
        emit askPlayerStop();
        // Update button
        buttonStartStop->setText("Start");
        // Update status
        isPlayerAlive = false;
    }
}


void AudioPlayerManager::playerWait()
{
    threadpool->waitForDone(-1);
}


void AudioPlayerManager::playerRunStop()
{
    if (isPlayerAlive)
    {
        // Stop player and wait for it to finish
        playerStop();
        playerWait();
    }
    else
    {
        playerRun();
    }
}


void AudioPlayerManager::playerError(QString message)
{
    displayWarning(name + " error:\n" + message);
    // Update player state
    playerStop();
}


MicrophonePlayerManager::MicrophonePlayerManager(QTabWidget *devices, QString name, QWidget *parent)
// Init of the player happens here
: AudioPlayerManager(new MicrophonePlayer(devices), name, parent) {}


MediaFilesPlayerManager::MediaFilesPlayerManager(QTabWidget *devices, QString name, QRect *screeanGeometry, QWidget *parent)
// Init of the player happens here
: AudioPlayerManager(new MediaFilesPlayer(devices), name, parent)
{
    // Connect signals to player
    connect((MediaFilesPlayer*)this->player, MediaFilesPlayer::signalTrackEnd, this, &MediaFilesPlayerManager::playerTrackEnded);
    connect(this, &MediaFilesPlayerManager::askNewTrack, (MediaFilesPlayer*)this->player, MediaFilesPlayer::setNewTrack);
    connect(this, &MediaFilesPlayerManager::askNewTrackState, (MediaFilesPlayer*)this->player, MediaFilesPlayer::setNewTrackState);
    connect(this, &MediaFilesPlayerManager::askNewTrackVolume, (MediaFilesPlayer*)this->player, MediaFilesPlayer::setNewTrackVolume);

    /*
    // Additional layouts:
    */
    QHBoxLayout *box_layout1 = new QHBoxLayout();
    layout->addLayout(box_layout1);
    //QHBoxLayout *box_layout2 = new QHBoxLayout();
    //layout->addLayout(box_layout2);
    QHBoxLayout *box_layout3 = new QHBoxLayout();
    box_layout3->setAlignment(Qt::AlignLeft);
    layout->addLayout(box_layout3);
    /*
    // Other widgets:
    */
    // Name
    trackName = new QLabel("<No track>");
    box_layout1->addWidget(trackName);
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


void MediaFilesPlayerManager::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("filepath&name"))
        event->acceptProposedAction();
}


void MediaFilesPlayerManager::dropEvent(QDropEvent *event)
{
    // Get file path and name
    QStringList list = QString::fromUtf8(event->mimeData()->data("filepath&name")).split("?");
    
    // Ask player to change track
    emit askNewTrack(list[0]);
    // Update track name
    trackName->setText(list[1]);

    // Exit event
    event->acceptProposedAction();
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
    // Ask player to change volume
    emit askNewTrackVolume(((float)value)/100);
    // Update volume label
    volumeLabel->setText(QString::number(value));
}


void MediaFilesPlayerManager::trackStop()
{
    // New track state
    trackState = AudioTrackContext::STOPPED;
    emit askNewTrackState(AudioTrackContext::STOPPED);
    // Update button
    buttonPlay->setText("Play");
}


void MediaFilesPlayerManager::trackPlayPause()
{
    // Play-pause track
    switch (trackState)
    {
        case AudioTrackContext::STOPPED:
        case AudioTrackContext::PAUSED:
            // New track state
            trackState = AudioTrackContext::PLAYING;
            emit askNewTrackState(AudioTrackContext::PLAYING);
            // Update button
            buttonPlay->setText("Pause");
            break;
        case AudioTrackContext::PLAYING:
            // New track state
            trackState = AudioTrackContext::PAUSED;
            emit askNewTrackState(AudioTrackContext::PAUSED);
            // Update button
            buttonPlay->setText("Play");
            break;
    }
}


void MediaFilesPlayerManager::playerTrackEnded()
{
    // New track state
    trackState = AudioTrackContext::STOPPED;
    // Update button
    buttonPlay->setText("Play");
}