#pragma once

// FFMPEG
extern "C"
{
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#define __STDC_CONSTANT_MACROS
}
// Qt
#include <QtCore/QString>
// Exceptions
#include <stdexcept>


/** Describes track state.*/
enum TrackState{
    STOPPED,
    PLAYING,
    PAUSED
};


/** Holds samples to be played.*/
struct AudioTrackFrame {
    /** Pointer to data (can be NULL).*/
    float *data = NULL;
    /** Number of samples (if <= 0 then frame has no data).*/
    int nb_samples = 0;
    /** Sample rate of the frame.*/
    int sampleRate = 0;

    /** Constructor.
     * 
     *  @param nb_samples exit code (should be <= 0).
    */
    AudioTrackFrame(int nb_samples);
    /** Constructor.
     * 
     *  @param data Pointer to audio samples.
     *  @param nb_channels Number of channels.
     *  @param mem_size Data memory size in bytes.
     *  @param nb_samples Number of samples.
     *  @param sampleRate Sample rate.
     *  @param volume What volume should be applied to samples.
    */
    explicit AudioTrackFrame(uint8_t **data, int nb_channels, int mem_size, int nb_samples, int sampleRate, float volume);
    /** Destructor.*/
    ~AudioTrackFrame();
};


/** Describes FFPEG media file context. Can read samples from media file.*/
class AudioTrackContext
{
    /** Media file path.*/
    QString filepath;
    /** Media file format context.*/
    AVFormatContext *format_ctx = NULL;
    /** Media file codec.*/
    const AVCodec *decoder = NULL;
    /** Media file codec contex.*/
    AVCodecContext *decoder_ctx = NULL;
    /** Media file packet.*/
    AVPacket *packet = NULL;
    /** Media file frame.*/
    AVFrame *frame = NULL;
    /** Media resampling context.*/
    struct SwrContext *swr_ctx = NULL;
    /** Media data desired channel.*/
    AVChannelLayout ch_layout = AV_CHANNEL_LAYOUT_STEREO;
    /** Media data desired sample format.*/
    AVSampleFormat sample_format = AV_SAMPLE_FMT_FLT;
    /** This vars are filled when track starts playing.*/
    int audio_stream_index, swr_nb_samples = 1024, swr_bufsize, swr_linesize;
    /** Next data sample to play.*/
    uint8_t **swr_data = NULL;

public:

    /** Track current state.*/
    TrackState state = STOPPED;

    /** Constructor.
     * 
     *  @param filepath Media file path.
    */
    explicit AudioTrackContext(QString filepath);
    /** Destructor.*/
    ~AudioTrackContext();

    /** Stop track.*/
    void stop();
    /** Play track.*/
    void play();
    /** Update track state.
     * 
     *  @param state New track state.
    */
    void setState(TrackState state);

    /** Get next audio frame.
     * 
     *  @param volume Audio volume.
     * 
     *  @return Audio samples read from media file.
    */
    AudioTrackFrame readSamples(float volume);
};