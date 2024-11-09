#pragma once

// FFMPEG
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/samplefmt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
#define __STDC_CONSTANT_MACROS
}
// Qt5
#include <QtCore/Qt>
#include <QtCore/QRect>
#include <QtCore/QString>
#include <QtWidgets/QWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QCheckBox>
// Custom
#include "WidgetMessages.hpp"

using namespace std;


// Describes track state
enum TrackState{
    STOPPED,
    PLAYING,
    PAUSED
};


// Holds data to be played
struct TrackFrame {
    // Pointer to data (can be NULL)
    float *data = NULL;
    // Number of samples (if <= 0 then frame has no data)
    int nb_samples = 0;
    // Sample rate of the frame
    int sample_rate = 0;

    // Constructor (nb_samples <= 0)
    TrackFrame(int nb_samples);
    // Constructor (audio data)
    TrackFrame(uint8_t **data, int nb_channels, int mem_size, int nb_samples, int sample_rate, float volume);
    // Destructor
    ~TrackFrame();
};


class AudioTrack: public QWidget, WidgetWarnings
{
    // Media file path
    QString filepath;
    // Volume adjustments in dB
    float volume = 0.5;
    // Track duration info
    int duration_cur = 0;
    int duration_total = 0;
    // Play button
    QPushButton *button_play;
    // Track progress bar
    QProgressBar *progress;
    // Volume label
    QLabel *volume_label;

    // Media file format context
    AVFormatContext *format_ctx = NULL;
    // Media file codec
    const AVCodec *decoder = NULL;
    // Media file codec contex
    AVCodecContext *decoder_ctx = NULL;
    // Media file packet
    AVPacket *packet = NULL;
    // Media file frame
    AVFrame *frame = NULL;
    // Media resampling context
    struct SwrContext *swr_ctx = NULL;
    // Media data desired channel
    AVChannelLayout ch_layout = AV_CHANNEL_LAYOUT_STEREO;
    // Media data desired sample format
    AVSampleFormat sample_format = AV_SAMPLE_FMT_FLT;
    // This vars are filled when track starts playing
    int audio_stream_index, swr_nb_samples = 1024, swr_bufsize, swr_linesize;
    // Next data sample to play
    uint8_t **swr_data = NULL;

    public:

    // Tells if track is currently playing
    TrackState track_state = STOPPED;

    // Constructor
    AudioTrack(QString filepath, QRect *screean_rect, QWidget *parent = nullptr);
    // Destructor
    ~AudioTrack();

    // Frees FFMPEG data
    void free_ffmpeg_data();

    // Plays track
    void play();
    // Pauses track
    void pause();
    // Resumes track
    void resume();
    // Stops track
    void stop();
    // Play-pause cycle
    void play_pause();

    // Sets track volume adjustment
    void set_volume(int value);
    // Updates track progress
    void update_progress();

    TrackFrame read_samples();
};