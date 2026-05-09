#ifndef AUDIO_TRACK_CONTEXT
#define AUDIO_TRACK_CONTEXT


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


/** Holds samples to be played.*/
struct AudioTrackFrame {
    /** Pointer to data (can be NULL).*/
    float *data = nullptr;
    /** Data size (if <= 0 then frame has no data).*/
    int size = 0;
    /** Sample rate of the frame.*/
    int sampleRate = 0;

    /** Constructor.
     * 
     *  @param size memory size.
    */
    AudioTrackFrame(int size)
    {
        this->size = size;
    }

    /** Constructor.
     * 
     *  @param data Pointer to audio samples.
     *  @param nb_channels Number of channels.
     *  @param mem_size Data memory size in bytes.
     *  @param nb_samples Number of samples.
     *  @param sampleRate Sample rate.
    */
    explicit AudioTrackFrame(uint8_t **data, int nb_channels, int mem_size, int nb_samples, int sampleRate)
    {
        // Copy audio data
        this->size = nb_channels * mem_size;
        this->data = (float*)malloc(size);
        memcpy(this->data, data[0], size);

        // Save sample rate
        this->sampleRate = sampleRate;
    }

    /**
     * Destructor.
     */
    ~AudioTrackFrame()
    {
        // Free any existing data
        if (data)
            free(data);
    }
};


/** Describes FFPEG media file context. Can read samples from media file.*/
class AudioTrackContext
{
    /** Media file path.*/
    QString filepath;
    /** Media file format context.*/
    AVFormatContext *format_ctx = nullptr;
    /** Media file codec.*/
    const AVCodec *decoder = nullptr;
    /** Media file codec contex.*/
    AVCodecContext *decoder_ctx = nullptr;
    /** Media file packet.*/
    AVPacket *packet = nullptr;
    /** Media file frame.*/
    AVFrame *frame = nullptr;
    /** Media resampling context.*/
    SwrContext *swr_ctx = nullptr;
    /** Media data desired channel.*/
    AVChannelLayout ch_layout = AV_CHANNEL_LAYOUT_STEREO;
    /** Media data desired sample format.*/
    AVSampleFormat sample_format = AV_SAMPLE_FMT_FLT;
    /** This vars are filled when track starts playing.*/
    int audio_stream_index, swr_nb_samples = 1024, swr_bufsize, swr_linesize;
    /** Next data sample to play.*/
    uint8_t **swr_data = nullptr;

public:

    /** Constructor.
     * 
     *  @param filepath Media file path.
    */
    explicit AudioTrackContext(QString filepath)
    {
        this->filepath = filepath;
    }

    /** 
     * Destructor.
     */
    ~AudioTrackContext()
    {
        close();
    }

    /**
     * @return audio track sample rate.
     */
    int getSampleRate() const
    {
        return (decoder_ctx) ? decoder_ctx->sample_rate : 0;
    }
    /**
     * @return audio track channel count.
     */
    int getChannelCount() const
    {
        return (decoder_ctx) ? decoder_ctx->ch_layout.nb_channels : 0;
    }

    /**
     * Opens track.
     */
    void open()
    {
        // Open file and get info about media streams in file
        const char* f = filepath.toStdString().c_str();
        if (avformat_open_input(&format_ctx, f, nullptr, nullptr) < 0)
        {
            close();
            throw std::runtime_error("Unable to open input file");
        }
        if (avformat_find_stream_info(format_ctx, nullptr) < 0)
        {
            close();
            throw std::runtime_error("Unable to find file streams info");
        }

        // Find index of the audio stream
        audio_stream_index = av_find_best_stream(format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &decoder, 0);
        if (audio_stream_index < 0)
        {
            close();
            throw std::runtime_error("Unable to find an audio stream");
        }

        // Init decoding context
        decoder_ctx = avcodec_alloc_context3(decoder);
        if (!decoder_ctx)
        {
            close();
            throw std::runtime_error("Unable to open create decoding context");
        }
        avcodec_parameters_to_context(decoder_ctx, format_ctx->streams[audio_stream_index]->codecpar);

        // Init audio decoder
        if (avcodec_open2(decoder_ctx, decoder, nullptr) < 0)
        {
            close();
            throw std::runtime_error("Unable to open audio decoder");
        }

        // DEBUG
        printf("%s\n", decoder->name);
        printf("channels: %d\n", decoder_ctx->ch_layout.nb_channels);
        printf("sample format: %d\n", decoder_ctx->sample_fmt);
        printf("sample rate: %d\n", decoder_ctx->sample_rate);

        // Allocate media data packet
        packet = av_packet_alloc();
        if (!packet)
        {
            close();
            throw std::runtime_error("Unable to allocate data packet");
        }
        // Allocate media data frame
        frame = av_frame_alloc();
        if (!frame)
        {
            close();
            throw std::runtime_error("Unable to allocate data frame");
        }

        // Allocate resampler context
        swr_ctx = swr_alloc();
        if (!swr_ctx)
        {
            close();
            throw std::runtime_error("Unable to allocate resampler context");
        }
        // Set resampler input samples parameters
        av_opt_set_chlayout(swr_ctx,   "in_chlayout",    &decoder_ctx->ch_layout, 0);
        av_opt_set_int(swr_ctx,        "in_sample_rate", decoder_ctx->sample_rate, 0);
        av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt",  decoder_ctx->sample_fmt, 0);
        // Set resampler output samples parameters
        av_opt_set_chlayout(swr_ctx,   "out_chlayout",    &ch_layout, 0);
        av_opt_set_int(swr_ctx,        "out_sample_rate", decoder_ctx->sample_rate, 0);
        av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt",  sample_format, 0);
        // Init resampler context
        if (swr_init(swr_ctx) < 0)
        {
            close();
            throw std::runtime_error("Unable to init resampling context");
        }

        // Buffer will be used as it is, no alignment
        if (av_samples_alloc_array_and_samples(&swr_data, &swr_linesize, ch_layout.nb_channels, swr_nb_samples, sample_format, 0) < 0)
        {
            close();
            throw std::runtime_error("Could not allocate destination samples");
        }
    }

    /**
     * Closes track.
     */
    void close()
    {
        // Free frame
        if (frame)
        {
            av_frame_free(&frame);
            frame = nullptr;
        }
        // Free packet
        if (packet)
        {
            av_packet_free(&packet);
            packet = nullptr;
        }
        // Free data sample
        if (swr_data)
        {
            av_freep(&swr_data[0]);
            av_freep(&swr_data);
            swr_data = nullptr;
        }
        // Free resampling complex
        if (swr_ctx)
        {
            swr_free(&swr_ctx);
            swr_ctx = nullptr;
        }
        // Free codec contex
        if (decoder_ctx)
        {
            avcodec_free_context(&decoder_ctx);
            decoder_ctx = nullptr;
        }
        // Close format contex (as well as file stream)
        if (format_ctx)
        {
            avformat_close_input(&format_ctx);
            format_ctx = nullptr;
        }
    }

    /** Get next audio frame.
     * 
     *  @return Audio samples read from media file.
    */
    AudioTrackFrame readSample()
    {
        // Try to find new frame
        while(1)
        {
            // Try to read frame
            int res = avcodec_receive_frame(decoder_ctx, frame);
            // No packet or packet has ended
            if (res == AVERROR(EAGAIN) || res == AVERROR_EOF)
            {
                // If we previously had any packet we should dispose it
                if (packet)
                {
                    av_packet_unref(packet);
                }

                // Try to read new packet
                if (av_read_frame(format_ctx, packet) < 0)
                {
                    // End of file
                    return 0;
                }

                // Check packet stream id (packet can represent video)
                if (packet->stream_index == audio_stream_index)
                {
                    // Send packet to decoder
                    switch (avcodec_send_packet(decoder_ctx, packet))
                    {
                        case AVERROR(EAGAIN):
                            close();
                            throw std::runtime_error("Error while sending a packet to the decoder v1");
                            break;
                        case AVERROR_EOF:
                            close();
                            throw std::runtime_error("Error while sending a packet to the decoder v2");
                            break;
                        case AVERROR(EINVAL):
                            close();
                            throw std::runtime_error("Error while sending a packet to the decoder v3");
                            break;
                        case AVERROR(ENOMEM):
                            close();
                            throw std::runtime_error("Error while sending a packet to the decoder v4");
                            break;
                        default:
                            if (packet==NULL)
                            {
                                close();
                                throw std::runtime_error("Error while sending a packet to the decoder");
                            }
                            break;
                    }
                }
                else
                {
                    // If this packet is not audio dispose it
                    av_packet_unref(packet);
                }
            }
            // Success
            else if (res == 0)
            {
                break;
            }
            // Other errors
            else
            {
                close();
                throw std::runtime_error("Error while receiving a frame from the decoder");
            }
        }

        // Some reallocation might be required
        if (swr_nb_samples != frame->nb_samples)
        {
            // Realloc
            av_freep(&swr_data[0]);
            if (av_samples_alloc(swr_data, &swr_linesize, ch_layout.nb_channels, frame->nb_samples, sample_format, 1) < 0)
            {
                close();
                throw std::runtime_error("Error allocating converted samples");
            }
            swr_nb_samples = frame->nb_samples;
        }

        // Convert samples
        int samples_per_channel = swr_convert(swr_ctx, swr_data, swr_nb_samples, (const uint8_t **)frame->extended_data, frame->nb_samples);
        if (samples_per_channel < 0)
        {
            close();
            throw std::runtime_error("Error while converting");
        }
        // Get resulting buffer size
        swr_bufsize = av_samples_get_buffer_size(&swr_linesize, ch_layout.nb_channels, samples_per_channel, sample_format, 1);
        if (swr_bufsize < 0)
        {
            close();
            throw std::runtime_error("Could not get sample buffer size");
        }

        // Do not forget to dispose processed frame
        av_frame_unref(frame);

        // Return finalized data
        return AudioTrackFrame(swr_data, ch_layout.nb_channels, swr_bufsize, swr_nb_samples, decoder_ctx->sample_rate);
    }
};


#endif // AUDIO_TRACK_CONTEXT