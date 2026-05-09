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
#include <AudioPlayers/AudioTrackReader.cpp>


/**
 * Player that manages media files.
 */
class  MediaFilesPlayer : public AudioPlayer
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

public:
    /** Describes track state.*/
    enum TrackState{
        STOPPED,
        PLAYING,
        PAUSED
    };

private:
    /** Current track.*/
    AudioTrackContext *track = nullptr;
    /** Current track state.*/
    TrackState state = STOPPED;
    /** Requested track state.*/
    TrackState newTrackState = STOPPED;

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

    /** Sets audio track.*/
    void setTrack(QString filepath);
    
    /**
     * @return audio track state.
     */
    TrackState getTrackState();
    /**
     *  @param state new track state.
    */
    void setTrackState(TrackState state);
    /**
     *  @param state new track state.
    */
    void nextTrackState(TrackState state);

    /** Sets audio track volume.*/
    void setTrackVolume(qreal volume);
    
signals:
    /** Emitted to update audio track state.*/
    void updateTrackState(TrackState state);
};