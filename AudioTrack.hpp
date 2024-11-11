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


/** Describes audio track loaded by user.
*/
class AudioTrack: public QWidget
{
    /** Media file path.*/
    QString filepath;
    /** Media file name.*/
    QString name;
    /** Media files player manager.*/
    MediaFilesPlayerManager *player;

    public:

    /** Constructor.
     * 
     *  @param filepath Path to the media file.
     * 
     *  @param player Media files player manager.
    */
    AudioTrack(QString filepath, MediaFilesPlayerManager *player, QWidget *parent = nullptr);

    /** Send this track to media files player manager*/
    void submitTrack();
};