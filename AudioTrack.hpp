#pragma once

// Qt5
#include <QtCore/QString>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
// Custom
#include "AudioPlayerManagers.hpp"

using namespace std;


class AudioTrack: public QWidget
{
    // Media file path and name
    QString filepath;
    QString name;
    // Media player manager
    MediaFilesPlayerManager *player;

    public:

    // Constructor
    AudioTrack(QString filepath, MediaFilesPlayerManager *player, QWidget *parent = nullptr);

    // Send this track to player
    void submit_to_player();
};