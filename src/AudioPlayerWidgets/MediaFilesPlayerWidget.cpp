#include <AudioPlayerWidgets/MediaFilesPlayerWidget.hpp>


MediaFilesPlayerWidget::MediaFilesPlayerWidget(QTabWidget const *devices, QString name, QWidget *parent)
// Init of the player happens here
: AudioPlayerWidget(new MediaFilesPlayer(devices), name, parent)
{
    // Allow dropping of draggable widgets
    setAcceptDrops(true);

    // Connect signals to player
    connect(this, &MediaFilesPlayerWidget::askToUpdateDevices, (MediaFilesPlayer*)this->player, MediaFilesPlayer::updateAudioDevices);
    connect((MediaFilesPlayer*)this->player, MediaFilesPlayer::signalState, this, &MediaFilesPlayerWidget::onTrackStateChanged);
    connect(this, &MediaFilesPlayerWidget::askNewTrack, (MediaFilesPlayer*)this->player, MediaFilesPlayer::setTrack);
    connect(this, &MediaFilesPlayerWidget::askNewState, (MediaFilesPlayer*)this->player, MediaFilesPlayer::setPlannedState);
    connect(this, &MediaFilesPlayerWidget::askNewVolume, (MediaFilesPlayer*)this->player, MediaFilesPlayer::setVolume);

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
    
    // Stop player and ask to change track
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
    emit askNewVolume(QtAudio::convertVolume(value / qreal(100.0), QtAudio::LogarithmicVolumeScale, QtAudio::LinearVolumeScale));
    // Update volume label
    volumeLabel->setText(QString::number(value));
}


void MediaFilesPlayerWidget::stop()
{
    // Kill player only if it is working
    if (((MediaFilesPlayer*)player)->getState() != MediaFilesPlayer::STOPPED)
    {
        // Stop track
        emit askNewState(MediaFilesPlayer::STOPPED);
        threadpool->waitForDone(-1);
    }
}


void MediaFilesPlayerWidget::startStop()
{
    // Play-pause track
    switch (((MediaFilesPlayer*)player)->getState())
    {
        // Start player
        case MediaFilesPlayer::STOPPED:
            threadpool->waitForDone(-1);
            threadpool->start(player);
        // Set playing
        case MediaFilesPlayer::PAUSED:
            emit askNewState(MediaFilesPlayer::PLAYING);
            break;
        // Set paused
        case MediaFilesPlayer::PLAYING:
            emit askNewState(MediaFilesPlayer::PAUSED);
            break;
    }
}


void MediaFilesPlayerWidget::onTrackStateChanged(MediaFilesPlayer::State state)
{
    switch (state)
    {
        // Stopped and paused state
        case MediaFilesPlayer::STOPPED:
        case MediaFilesPlayer::PAUSED:
            buttonPlay->setText("Play");
            break;
        // Playing state
        case MediaFilesPlayer::PLAYING:
            buttonPlay->setText("Pause");
            break;
    }
}