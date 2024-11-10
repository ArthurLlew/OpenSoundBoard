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
    header_layout->setAlignment(Qt::AlignLeft);
    layout->addLayout(header_layout);
    // Start/Stop button
    button_start_stop = new QPushButton("Start");
    connect(button_start_stop, &QPushButton::pressed, this, &AudioPlayerManager::run_kill);
    header_layout->addWidget(button_start_stop);
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


void AudioPlayerManager::run()
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


void AudioPlayerManager::kill()
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


void AudioPlayerManager::run_kill()
{
    if (is_player_alive)
    {
        // Stop player and wait for it to finish
        kill();
        wait_player();
    }
    else
    {
        run();
    }
}


void AudioPlayerManager::player_error(QString message)
{
    show_warning(name + " error:\n" + message);
    // Update player state
    kill();
}


MicrophonePlayerManager::MicrophonePlayerManager(QTabWidget *devices, QString name, QWidget *parent)
: AudioPlayerManager(name, parent)
{
    // Create microphone player and connect signals
    this->player = new MicrophonePlayer(devices);
    connect((MicrophonePlayer*)this->player, MicrophonePlayer::player_error, this, &MicrophonePlayerManager::player_error);
}


MediaFilesPlayerManager::MediaFilesPlayerManager(QTabWidget *devices, QString name, QRect *screean_rect, QWidget *parent)
: AudioPlayerManager(name, parent)
{
    // Create media files player and connect signals
    this->player = new MediaFilesPlayer(devices, &volume);
    connect((MediaFilesPlayer*)this->player, MediaFilesPlayer::player_error, this, &MediaFilesPlayerManager::player_error);
    connect((MediaFilesPlayer*)this->player, MediaFilesPlayer::track_endeded, this, &MediaFilesPlayerManager::player_track_ended);
    connect(this, &MediaFilesPlayerManager::ask_new_track, (MediaFilesPlayer*)this->player, MediaFilesPlayer::new_track);
    connect(this, &MediaFilesPlayerManager::ask_new_track_state, (MediaFilesPlayer*)this->player, MediaFilesPlayer::new_track_state);

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
    track_name = new QLabel("<No track>");
    box_layout1->addWidget(track_name);
    // Track progress
    progress = new QProgressBar();
    progress->setMinimum(0);
    progress->setMaximum(duration_total);
    box_layout2->addWidget(progress);
    // Play/Pause button
    button_play = new QPushButton("Play");
    connect(button_play, &QPushButton::pressed, this, &MediaFilesPlayerManager::play_pause);
    box_layout3->addWidget(button_play);
    // Stop buttom
    QPushButton *button_stop = new QPushButton("Stop");
    connect(button_stop, &QPushButton::pressed, this, &MediaFilesPlayerManager::stop);
    box_layout3->addWidget(button_stop);
    // Volume slider and label
    QSlider *volume_slider = new QSlider(Qt::Orientation::Horizontal);
    volume_label = new QLabel(QString::number(volume_slider->value()));
    connect(volume_slider, &QSlider::valueChanged, this, &MediaFilesPlayerManager::set_volume);
    volume_slider->setRange(0,100);
    volume_slider->setValue(90);
    volume_slider->setMinimumWidth(screean_rect->width()/14);
    volume_slider->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    box_layout3->addWidget(volume_slider);
    box_layout3->addWidget(volume_label);
}


void MediaFilesPlayerManager::player_error(QString message)
{
    // Update track state
    player_track_ended();
    // Call parent method
    AudioPlayerManager::player_error(message);
}


void MediaFilesPlayerManager::player_track_ended()
{
    // New track state
    track_state = STOPPED;
    // Update button
    button_play->setText("Play");
}


void MediaFilesPlayerManager::set_volume(int value)
{
    // Update volume var and label
    volume = ((float)value)/100;
    volume_label->setText(QString::number(value));
}


void MediaFilesPlayerManager::update_progress()
{
    progress->setValue(duration_cur);
}


void MediaFilesPlayerManager::insert_track(QString filepath, QString name)
{
    // Ask player to change track
    emit ask_new_track(filepath);
    // Update track name
    track_name->setText(name);
}


void MediaFilesPlayerManager::stop()
{
    // New track state
    track_state = STOPPED;
    emit ask_new_track_state(STOPPED);
    // Update button
    button_play->setText("Play");
}


void MediaFilesPlayerManager::play_pause()
{
    // Play-pause cycle
    switch (track_state)
    {
        case STOPPED:
        case PAUSED:
            // New track state
            track_state = PLAYING;
            emit ask_new_track_state(PLAYING);
            // Update button
            button_play->setText("Pause");
            break;
        case PLAYING:
            // New track state
            track_state = PAUSED;
            emit ask_new_track_state(PAUSED);
            // Update button
            button_play->setText("Play");
            break;
    }
}