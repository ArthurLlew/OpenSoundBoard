#pragma once

// Qt
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QRunnable>
#include <QtWidgets/QWidget>
#include <QtWidgets/QTabWidget>
// Custom
#include <DeviceTab.hpp>
#include <AudioPlayers/AudioPlayer.hpp>
#include <AudioPlayers/AudioTrackContext.hpp>


/** Player that manages media files.*/
class  MediaFilesPlayer : public AudioPlayer
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

    /** Current track.*/
    AudioTrackContext *track = nullptr;
    /** Requested track state.*/
    AudioTrackContext::TrackState newTrackState = AudioTrackContext::STOPPED;
    /** Audio volume.*/
    float volume = 0.5;

public:

    /** Constructor.
     * 
     *  @param devices Tab widget that describes available devices.
    */
    explicit MediaFilesPlayer(QTabWidget const *devices);
    /** Destructor.*/
    ~MediaFilesPlayer();

    /** Player cycle.*/
    void run();
    
    /** Gets audio track state.*/
    AudioTrackContext::TrackState getTrackState();

    /** Sets new audio track to play.*/
    void setNewTrack(QString filepath);
    /** Sets new audio track state.*/
    void setNewTrackState(AudioTrackContext::TrackState state);
    /** Sets audio track volume.*/
    void setNewTrackVolume(float volume);
    

signals:
    /** Emitted to update audio track state.*/
    void signalNewTrackState(AudioTrackContext::TrackState state);
};