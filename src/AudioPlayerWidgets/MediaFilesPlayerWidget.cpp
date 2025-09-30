#include <AudioPlayerWidgets/MediaFilesPlayerWidget.hpp>


MediaFilesPlayerWidget::MediaFilesPlayerWidget(QTabWidget const *devices, QString name, QWidget *parent)
// Init of the player happens here
: AudioPlayerWidget(new MediaFilesPlayer(devices), name, parent)
{
    // Allow dropping of draggable widgets
    setAcceptDrops(true);

    // Connect signals to player
    connect(this, &MediaFilesPlayerWidget::askToUpdateDevices, (MediaFilesPlayer*)this->player, MediaFilesPlayer::updateAudioDevices);
    connect((MediaFilesPlayer*)this->player, MediaFilesPlayer::updateTrackState, this, &MediaFilesPlayerWidget::onTrackStateChanged);
    connect(this, &MediaFilesPlayerWidget::askNewTrack, (MediaFilesPlayer*)this->player, MediaFilesPlayer::setTrack);
    connect(this, &MediaFilesPlayerWidget::askNewTrackState, (MediaFilesPlayer*)this->player, MediaFilesPlayer::setTrackState);
    connect(this, &MediaFilesPlayerWidget::askNewTrackVolume, (MediaFilesPlayer*)this->player, MediaFilesPlayer::setTrackVolume);

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
    connect(buttonPlay, &QPushButton::pressed, this, &MediaFilesPlayerWidget::startStop);
    box_layout3->addWidget(buttonPlay);
    // Stop buttom
    QPushButton *button_stop = new QPushButton("Stop");
    connect(button_stop, &QPushButton::pressed, this, &MediaFilesPlayerWidget::stop);
    box_layout3->addWidget(button_stop);
    // Volume slider and label
    QSlider *volume_slider = new QSlider(Qt::Orientation::Horizontal);
    volumeLabel = new QLabel(QString::number(volume_slider->value()));
    connect(volume_slider, &QSlider::valueChanged, this, &MediaFilesPlayerWidget::setVolume);
    volume_slider->setRange(0,100);
    volume_slider->setValue(30);
    QRect screeanGeometry = static_cast<QApplication*>(QApplication::instance())->primaryScreen()->availableGeometry();
    volume_slider->setMinimumWidth(screeanGeometry.width()/14);
    volume_slider->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    box_layout3->addWidget(volume_slider);
    box_layout3->addWidget(volumeLabel);
}


MediaFilesPlayerWidget::~MediaFilesPlayerWidget()
{
    stop();
}


void MediaFilesPlayerWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("filepath&name"))
        event->acceptProposedAction();
}


void MediaFilesPlayerWidget::dropEvent(QDropEvent *event)
{
    // Get file path and name
    QStringList list = QString::fromUtf8(event->mimeData()->data("filepath&name")).split("?");
    
    // Ask player to change track
    stop();
    emit askNewTrack(list[0]);
    // Update track name
    trackName->setText(list[1]);

    // Exit event
    event->acceptProposedAction();
}


void MediaFilesPlayerWidget::setVolume(int value)
{
    // Ask player to change volume
    emit askNewTrackVolume(QtAudio::convertVolume(value / qreal(100.0), QtAudio::LogarithmicVolumeScale, QtAudio::LinearVolumeScale));
    // Update volume label
    volumeLabel->setText(QString::number(value));
}


void MediaFilesPlayerWidget::updateDevices()
{
    emit askToUpdateDevices();
}


void MediaFilesPlayerWidget::stop()
{
    // Kill player only if he is working
    if (((MediaFilesPlayer*)player)->getTrackState() != AudioTrackContext::STOPPED)
    {
        // Stop track
        emit askNewTrackState(AudioTrackContext::STOPPED);
        threadpool->waitForDone(-1);
    }
}


void MediaFilesPlayerWidget::startStop()
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


void MediaFilesPlayerWidget::onTrackStateChanged(AudioTrackContext::TrackState state)
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