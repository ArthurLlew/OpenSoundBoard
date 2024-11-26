#include "AudioPlayerManagers.hpp"


AudioPlayerManager::AudioPlayerManager(AudioPlayer *player, QString name, QWidget *parent)
: QWidget(parent)
{
    this->player = player;
    this->name = name;

    // Create threadpool and disable thread expiry (only we terminate threads explicitly)
    threadpool = new QThreadPool();
    threadpool->setExpiryTimeout(-1);

    // Connect signal to player
    connect(this->player, AudioPlayer::signalError, this, &AudioPlayerManager::playerError);

    // Main layout
    layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop);
    setLayout(layout);
}


AudioPlayerManager::~AudioPlayerManager()
{
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


void AudioPlayerManager::playerError(QString message)
{
    displayWarning(name + " error:\n" + message);
    // Update player state
    playerStop();
}


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


MediaFilesPlayerManager::MediaFilesPlayerManager(QTabWidget const *devices, QString name, QRect *screeanGeometry, QWidget *parent)
// Init of the player happens here
: AudioPlayerManager(new MediaFilesPlayer(devices), name, parent)
{
    // Allow dropping of draggable widgets
    setAcceptDrops(true);

    // Connect signals to player
    connect(this, &MediaFilesPlayerManager::askToUpdateDevices, (MediaFilesPlayer*)this->player, MediaFilesPlayer::updateAudioStreams);
    connect((MediaFilesPlayer*)this->player, MediaFilesPlayer::signalTrackEnd, this, &MediaFilesPlayerManager::playerTrackEnded);
    connect(this, &MediaFilesPlayerManager::askNewTrack, (MediaFilesPlayer*)this->player, MediaFilesPlayer::setNewTrack);
    connect(this, &MediaFilesPlayerManager::askNewTrackState, (MediaFilesPlayer*)this->player, MediaFilesPlayer::setNewTrackState);
    connect(this, &MediaFilesPlayerManager::askNewTrackVolume, (MediaFilesPlayer*)this->player, MediaFilesPlayer::setNewTrackVolume);

    /*
    // Header label:
    */
    QLabel *label = new QLabel(name);
    layout->addWidget(label);
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
    // Track name
    trackName = new QLabel("<No track>");
    box_layout1->addWidget(trackName);
    // Play/Pause button
    buttonPlay = new QPushButton("Play");
    connect(buttonPlay, &QPushButton::pressed, this, &MediaFilesPlayerManager::playerStartStop);
    box_layout3->addWidget(buttonPlay);
    // Stop buttom
    QPushButton *button_stop = new QPushButton("Stop");
    connect(button_stop, &QPushButton::pressed, this, &MediaFilesPlayerManager::playerStop);
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


MediaFilesPlayerManager::~MediaFilesPlayerManager()
{
    // Stop player
    playerStop();
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


void MediaFilesPlayerManager::setVolume(int value)
{
    // Ask player to change volume
    emit askNewTrackVolume(((float)value)/100);
    // Update volume label
    volumeLabel->setText(QString::number(value));
}


void MediaFilesPlayerManager::updateDevices()
{
    emit askToUpdateDevices();
}


void MediaFilesPlayerManager::playerStop()
{
    // Kill player only if he is working
    if (trackState != AudioTrackContext::STOPPED)
    {
        // New track state
        emit askNewTrackState(AudioTrackContext::STOPPED);
        threadpool->waitForDone(-1);
        // Update state
        trackState = AudioTrackContext::STOPPED;
        // Update button
        buttonPlay->setText("Play");
    }
}


void MediaFilesPlayerManager::playerStartStop()
{
    // Play-pause track
    switch (trackState)
    {
        case AudioTrackContext::STOPPED:
            // Start player
            threadpool->start(player);
        case AudioTrackContext::PAUSED:
            // New track state
            emit askNewTrackState(AudioTrackContext::PLAYING);
            trackState = AudioTrackContext::PLAYING;
            // Update button
            buttonPlay->setText("Pause");
            break;
        case AudioTrackContext::PLAYING:
            // New track state
            emit askNewTrackState(AudioTrackContext::PAUSED);
            trackState = AudioTrackContext::PAUSED;
            // Update button
            buttonPlay->setText("Play");
            break;
    }
}


void MediaFilesPlayerManager::playerTrackEnded()
{
    threadpool->waitForDone(-1);
    // New track state
    trackState = AudioTrackContext::STOPPED;
    // Update button
    buttonPlay->setText("Play");
}