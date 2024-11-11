#include "AudioTrack.hpp"


AudioTrack::AudioTrack(QString filepath, MediaFilesPlayerManager *player, QWidget *parent) : QWidget(parent)
{
    this->filepath = filepath;
    this->player = player;

    // Layout
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setAlignment(Qt::AlignLeft);
    setLayout(layout);
    // Submit button
    QPushButton *button_submit = new QPushButton("Submit");
    connect(button_submit, &QPushButton::pressed, this, &AudioTrack::submit_to_player);
    layout->addWidget(button_submit);
    // Name
    name = filepath.mid(filepath.lastIndexOf('/') + 1);
    layout->addWidget(new QLabel(name));
}


void AudioTrack::submit_to_player()
{
    emit player->track_insert(filepath, name);
}