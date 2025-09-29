#include <AudioPlayerManagers/MediaFilesPlayerManager.hpp>


MediaFilesPlayerManager::MediaFilesPlayerManager(QTabWidget const *devices, QString name, QWidget *parent)
// Init of the player happens here
: AudioPlayerManager(new MediaFilesPlayer(devices), name, parent)
{
    // Allow dropping of draggable widgets
    setAcceptDrops(true);

    // Connect signals to player
    connect(this, &MediaFilesPlayerManager::askToUpdateDevices, (MediaFilesPlayer*)this->player, MediaFilesPlayer::updateAudioStreams);
    connect((MediaFilesPlayer*)this->player, MediaFilesPlayer::signalNewTrackState, this, &MediaFilesPlayerManager::onTrackStateChanged);
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
    connect(buttonPlay, &QPushButton::pressed, this, &MediaFilesPlayerManager::startStop);
    box_layout3->addWidget(buttonPlay);
    // Stop buttom
    QPushButton *button_stop = new QPushButton("Stop");
    connect(button_stop, &QPushButton::pressed, this, &MediaFilesPlayerManager::stop);
    box_layout3->addWidget(button_stop);
    // Volume slider and label
    QSlider *volume_slider = new QSlider(Qt::Orientation::Horizontal);
    volumeLabel = new QLabel(QString::number(volume_slider->value()));
    connect(volume_slider, &QSlider::valueChanged, this, &MediaFilesPlayerManager::setVolume);
    volume_slider->setRange(0,100);
    volume_slider->setValue(30);
    QRect screeanGeometry = static_cast<QApplication*>(QApplication::instance())->primaryScreen()->availableGeometry();
    volume_slider->setMinimumWidth(screeanGeometry.width()/14);
    volume_slider->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    box_layout3->addWidget(volume_slider);
    box_layout3->addWidget(volumeLabel);
}


MediaFilesPlayerManager::~MediaFilesPlayerManager()
{
    // Stop player
    stop();
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


void MediaFilesPlayerManager::stop()
{
    // Kill player only if he is working
    if (((MediaFilesPlayer*)player)->getTrackState() != AudioTrackContext::STOPPED)
    {
        // Stop track
        emit askNewTrackState(AudioTrackContext::STOPPED);
        threadpool->waitForDone(-1);
    }
}


void MediaFilesPlayerManager::startStop()
{
    // Play-pause track
    switch (((MediaFilesPlayer*)player)->getTrackState())
    {
        case AudioTrackContext::STOPPED:
            // Start player
            threadpool->start(player);
        case AudioTrackContext::PAUSED:
            // Set new track state
            emit askNewTrackState(AudioTrackContext::PLAYING);
            break;
        case AudioTrackContext::PLAYING:
            // Set new track state
            emit askNewTrackState(AudioTrackContext::PAUSED);
            break;
    }
}


void MediaFilesPlayerManager::onTrackStateChanged(AudioTrackContext::TrackState state)
{
    switch (state)
    {
        case AudioTrackContext::STOPPED:
        case AudioTrackContext::PAUSED:
            // Update button
            buttonPlay->setText("Play");
            break;
        case AudioTrackContext::PLAYING:
            // Update button
            buttonPlay->setText("Pause");
            break;
    }
}