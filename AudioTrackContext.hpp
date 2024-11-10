#pragma once

// FFMPEG
extern "C"
{
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libavutil/channel_layout.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#define __STDC_CONSTANT_MACROS
}
// Qt5
#include <QtCore/QString>
// Exceptions
#include <stdexcept>

using namespace std;


// Describes track state
enum TrackState{
    STOPPED,
    PLAYING,
    PAUSED
};


// Holds data to be played
struct AudioTrackFrame {
    // Pointer to data (can be NULL)
    float *data = NULL;
    // Number of samples (if <= 0 then frame has no data)
    int nb_samples = 0;
    // Sample rate of the frame
    int sample_rate = 0;

    // Constructor (nb_samples <= 0)
    AudioTrackFrame(int nb_samples);
    // Constructor (audio data)
    AudioTrackFrame(uint8_t **data, int nb_channels, int mem_size, int nb_samples, int sample_rate, float volume);
    // Destructor
    ~AudioTrackFrame();
};


class AudioTrackContext
{
    // Media file path
    QString filepath;
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

    // Tells track current state
    TrackState state = STOPPED;

    // Constructor
    AudioTrackContext(QString filepath);
    // Destructor
    ~AudioTrackContext();

    // Stops track
    void stop();
    // Plays track
    void play();
    // Updates track state
    void set_state(TrackState new_state);

    // Returns next audio frame
    AudioTrackFrame read_samples(float volume);
};