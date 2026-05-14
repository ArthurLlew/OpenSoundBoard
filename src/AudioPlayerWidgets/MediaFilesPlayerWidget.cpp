#include <AudioPlayerWidgets/MediaFilesPlayerWidget.hpp>


// Scaling of volume slider for better time control
#define VOLUME_SLIDER_SCALE 10
// Default labels values
#define NO_TRACK_STR "<No track>"
#define NO_DURATION_STR "0:00:00/0:00:00"


MediaFilesPlayerWidget::MediaFilesPlayerWidget(QTabWidget const *devices, QString name, QWidget *parent)
// Init of the player happens here
: AudioPlayerWidget(new MediaFilesPlayer(devices), name, parent)
{
    // Allow dropping of draggable widgets
    setAcceptDrops(true);

    // Connect signals to player
    connect(this, &MediaFilesPlayerWidget::askToUpdateDevices, (MediaFilesPlayer*)this->player, MediaFilesPlayer::updateAudioDevices);
    connect(this, &MediaFilesPlayerWidget::askNewTrack, (MediaFilesPlayer*)this->player, MediaFilesPlayer::setTrack);
    connect(this, &MediaFilesPlayerWidget::askRemoveTrack, (MediaFilesPlayer*)this->player, MediaFilesPlayer::removeTrack);
    connect(this, &MediaFilesPlayerWidget::askNewVolume, (MediaFilesPlayer*)this->player, MediaFilesPlayer::setVolume);
    connect(this, &MediaFilesPlayerWidget::askNewState, (MediaFilesPlayer*)this->player, MediaFilesPlayer::scheduleState);
    connect(this, &MediaFilesPlayerWidget::askNewTime, (MediaFilesPlayer*)this->player, &MediaFilesPlayer::scheduleTime);
    connect((MediaFilesPlayer*)this->player, MediaFilesPlayer::signalState, this, &MediaFilesPlayerWidget::onStateChanged);
    connect((MediaFilesPlayer*)this->player, &MediaFilesPlayer::signalDuration, this, &MediaFilesPlayerWidget::onDurationChanged);
    connect((MediaFilesPlayer*)this->player, &MediaFilesPlayer::signalTime, this, &MediaFilesPlayerWidget::onTimeChanged);

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
    QHBoxLayout *box_layout2 = new QHBoxLayout();
    layout->addLayout(box_layout2);
    QHBoxLayout *box_layout3 = new QHBoxLayout();
    box_layout3->setAlignment(Qt::AlignLeft);
    layout->addLayout(box_layout3);
    /*
    // Other widgets:
    */
    // Track name
    trackName = new QLabel(NO_TRACK_STR);
    box_layout1->addWidget(trackName);
    // Track duration
    trackDuration = new QLabel(NO_DURATION_STR);
    trackDuration->setAlignment(Qt::AlignRight);  // snap to the right
    box_layout1->addWidget(trackDuration);

    // Time slider
    timeSlider = new QSlider(Qt::Horizontal);
    timeSlider->setTracking(false);            // Only fire valueChanged event after releasing slider
    timeSlider->setRange(0, 0);                // Will be updated when audio is loaded
    connect(timeSlider, &QSlider::sliderPressed, this, &MediaFilesPlayerWidget::pauseOnTimeChange);        // pause player when slider was pressed
    connect(timeSlider, &QSlider::sliderReleased, this, &MediaFilesPlayerWidget::resumeAfterTimeChange);   // resume player after slider was released
    connect(timeSlider, &QSlider::valueChanged, this, &MediaFilesPlayerWidget::setTime);                   // set new time on value change
    box_layout2->addWidget(timeSlider);

    // Play/Pause button
    buttonPlay = new QPushButton();
    buttonPlay->setFixedSize(28, 28); 
    setButtonIcon(buttonPlay, "player_play");
    connect(buttonPlay, &QPushButton::pressed, this, &MediaFilesPlayerWidget::startStop);
    box_layout3->addWidget(buttonPlay);
    // Stop buttom
    QPushButton *button_stop = new QPushButton();
    button_stop->setFixedSize(28, 28); 
    setButtonIcon(button_stop, "player_stop");
    connect(button_stop, &QPushButton::pressed, this, &MediaFilesPlayerWidget::stop);
    box_layout3->addWidget(button_stop);
    // Volume slider and label
    QSlider *volume_slider = new QSlider(Qt::Orientation::Horizontal);
    volumeLabel = new QLabel(QString::number(volume_slider->value()));
    connect(volume_slider, &QSlider::valueChanged, this, &MediaFilesPlayerWidget::setVolume);
    volume_slider->setRange(0,100);
    volume_slider->setValue(50);
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


void MediaFilesPlayerWidget::setButtonIcon(QPushButton *button, std::string icon_name)
{
    button->setStyleSheet(QString::fromStdString(
    "QPushButton {"
    "   background: transparent;"
    "   background-repeat: no-repeat; background-position: center;"
    "   background-image: url(:/resources/buttons/" + icon_name + ".png);"
    "   padding: 2px 2px 2px 2px;"
    "}"
    "QPushButton:hover {"
    "   background-image: url(:/resources/buttons/" + icon_name + "_hover.png);"
    "}"
    "QPushButton:pressed {"
    "   background-image: url(:/resources/buttons/" + icon_name + "_pressed.png);"
    "}"
    ));
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


void MediaFilesPlayerWidget::contextMenuEvent(QContextMenuEvent *event)
{
    // Init menu
    QMenu menu(this);
    
    // Add remove track action
    QAction *removeAction = menu.addAction("Remove track");
    connect(removeAction, &QAction::triggered, this, [this]()
    {
        // Stop player and remove track
        this->stop();
        emit this->askRemoveTrack();
        // Update labels
        this->trackName->setText(NO_TRACK_STR);
        this->trackDuration->setText(NO_DURATION_STR);
    });

    // Place menu at correct position (of cursor)
    menu.exec(event->globalPos());
}


double MediaFilesPlayerWidget::convertLogToLinear(float value) {
    // Avoid non-zero from log
    if (value <= 0.0) return 0.0;

    // 10^(2 * (value - 1))
    return std::pow(10.0, 2.0 * (value - 1.0));
}


void MediaFilesPlayerWidget::setVolume(int value)
{
    // Ask player to change volume
    emit askNewVolume(convertLogToLinear(value / 100.0));
    // Update volume label
    volumeLabel->setText(QString::number(value));
}


void MediaFilesPlayerWidget::setTime(int value)
{
    // Ask player to change time
    emit askNewTime(static_cast<double>(value) / VOLUME_SLIDER_SCALE);
}


void MediaFilesPlayerWidget::pauseOnTimeChange()
{
    // Pause player only if it is playing
    if (((MediaFilesPlayer*)player)->getState() == MediaFilesPlayer::PLAYING)
    {
        // Pause player
        emit askNewState(MediaFilesPlayer::PAUSED);
        // Save our messing around with player
        wasPausedByTimeSlider = true;
    }
}


void MediaFilesPlayerWidget::resumeAfterTimeChange()
{
    // Resume player only if it was paused by pressing slider
    if (wasPausedByTimeSlider && ((MediaFilesPlayer*)player)->getState() == MediaFilesPlayer::PAUSED)
    {
        // Resume player
        emit askNewState(MediaFilesPlayer::PLAYING);
        // Release flag
        wasPausedByTimeSlider = false;
    }
}


void MediaFilesPlayerWidget::stop()
{
    // Kill player only if it is working
    if (((MediaFilesPlayer*)player)->getState() != MediaFilesPlayer::STOPPED)
    {
        // Stop player
        emit askNewState(MediaFilesPlayer::STOPPED);
        threadpool->waitForDone(-1);
        // Update this flag on stopping player
        wasPausedByTimeSlider = false;
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
            break;
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


void MediaFilesPlayerWidget::onStateChanged(MediaFilesPlayer::State state)
{
    // Avoid button changes when moving time slider
    if (!wasPausedByTimeSlider)
    {
        switch (state)
        {
            // Stopped and paused state
            case MediaFilesPlayer::STOPPED:
            case MediaFilesPlayer::PAUSED:
                setButtonIcon(buttonPlay, "player_play");
                break;
            // Playing state
            case MediaFilesPlayer::PLAYING:
                buttonPlay->setIcon(QIcon(":/resources/button_pause.png"));
                setButtonIcon(buttonPlay, "player_pause");
                break;
        }
    }
}


std::string MediaFilesPlayerWidget::formatTime(int seconds) {
    // Convert from slider value
    seconds /= VOLUME_SLIDER_SCALE;

    // Compute time components
    std::chrono::seconds s{seconds};
    std::chrono::hours h = std::chrono::duration_cast<std::chrono::hours>(s);
    s -= h;
    std::chrono::minutes m = std::chrono::duration_cast<std::chrono::minutes>(s);
    s -= m;

    // Construct and return time string
    std::ostringstream time_str;
    time_str << h.count() << ":" 
             << std::setfill('0') << std::setw(2) << m.count() << ":" 
             << std::setfill('0') << std::setw(2) << s.count();
    return time_str.str();
}


QString MediaFilesPlayerWidget::getDurationLabel(int value, int maximum)
{
    return QString::fromStdString(formatTime(value) + '/' + formatTime(maximum));
}


void MediaFilesPlayerWidget::onDurationChanged(double seconds)
{
    timeSlider->setMaximum(static_cast<int>(seconds * VOLUME_SLIDER_SCALE));
    trackDuration->setText(getDurationLabel(timeSlider->value(), timeSlider->maximum()));
}


void MediaFilesPlayerWidget::onTimeChanged(double seconds)
{
    // Updtae only if user is not holding slider
    if (!timeSlider->isSliderDown())
    {
        timeSlider->blockSignals(true);
        timeSlider->setValue(static_cast<int>(seconds * VOLUME_SLIDER_SCALE));
        trackDuration->setText(getDurationLabel(timeSlider->value(), timeSlider->maximum()));
        timeSlider->blockSignals(false);
    }
}