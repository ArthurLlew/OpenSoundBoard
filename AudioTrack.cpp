#include "AudioTrack.hpp"


TrackFrame::TrackFrame(int nb_samples)
{
    this->nb_samples = nb_samples;
}
TrackFrame::TrackFrame(uint8_t **data, int nb_channels, int mem_size, int nb_samples, int sample_rate, float volume)
{
    // Copy audio data
    int size = nb_channels * mem_size;
    this->data = (float*)malloc(size);
    memcpy(this->data, data[0], size);

    // Apply volume
    for (int i = 0; i < nb_channels*nb_samples; i++)
        this->data[i] *= volume;

    // Save other params
    this->nb_samples = nb_samples;
    this->sample_rate = sample_rate;
}
TrackFrame::~TrackFrame()
{
    // Do not forget to free data
    if (data)
        free(data);
}


AudioTrack::AudioTrack(QString filepath, QRect *screean_rect, QWidget *parent) : QWidget(parent)
{
    this->filepath = filepath;

    // Grid layout
    QGridLayout *grid = new QGridLayout();
    setLayout(grid);
    // Additional layouts
    QHBoxLayout *box_layout1 = new QHBoxLayout();
    QHBoxLayout *box_layout2 = new QHBoxLayout();
    QHBoxLayout *box_layout3 = new QHBoxLayout();
    // Add them to grid
    grid->addLayout(box_layout1, 0, 0, 1, 1);
    grid->addLayout(box_layout2, 1, 0, 1, 1);
    grid->addLayout(box_layout3, 2, 0, 1, 1);
    box_layout3->setAlignment(Qt::AlignLeft);
    // Other widgets:
    // Name
    box_layout1->addWidget(new QLabel(filepath.mid(filepath.lastIndexOf('/') + 1)));
    // Track progress
    progress = new QProgressBar();
    progress->setMinimum(0);
    progress->setMaximum(duration_total);
    box_layout2->addWidget(progress);
    // Play button
    button_play = new QPushButton("Play");
    connect(button_play, &QPushButton::pressed, this, &AudioTrack::play_pause);
    box_layout3->addWidget(button_play);
    // Stop buttom
    QPushButton *button_stop = new QPushButton("Stop");
    connect(button_stop, &QPushButton::pressed, this, &AudioTrack::stop);
    box_layout3->addWidget(button_stop);
    // Volume slider and label
    QSlider *volume_slider = new QSlider(Qt::Orientation::Horizontal);
    volume_label = new QLabel(QString::number(volume_slider->value()));
    connect(volume_slider, &QSlider::valueChanged, this, &AudioTrack::set_volume);
    volume_slider->setRange(0,100);
    volume_slider->setValue(90);
    volume_slider->setMinimumWidth(screean_rect->width()/14);
    volume_slider->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    box_layout3->addWidget(volume_slider);
    box_layout3->addWidget(volume_label);
}


AudioTrack::~AudioTrack()
{
    // Dispose FFMPEG data
    free_ffmpeg_data();
}


void AudioTrack::free_ffmpeg_data()
{
    // Free data sample
    if (swr_data)
    {
        av_freep(&swr_data[0]);
    }
    av_freep(&swr_data);
    // Free resampling complex
    if (swr_ctx)
        swr_free(&swr_ctx);
    // Free frame
    if (frame)
        av_frame_free(&frame);
    // Free packet
    if (packet)
        av_packet_free(&packet);
    // Free codec contex
    if (decoder_ctx)
        avcodec_free_context(&decoder_ctx);
    // Close format contex (as well as file stream)
    if (format_ctx)
        avformat_close_input(&format_ctx);
}


void AudioTrack::play()
{
    // Open file and get info about media streams in file
    const char* f = filepath.toStdString().c_str();
    if (avformat_open_input(&format_ctx, f, NULL, NULL) < 0)
    {
        show_warning("Unable to open input file");
        stop();
        return;
    }
    if (avformat_find_stream_info(format_ctx, NULL) < 0)
    {
        show_warning("Unable to find file streams info");
        stop();
        return;
    }

    // Find index of the audio stream
    audio_stream_index = av_find_best_stream(format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &decoder, 0);
    if (audio_stream_index < 0)
    {
        show_warning("Unable to find an audio stream");
        stop();
        return;
    }

    // Init decoding context
    decoder_ctx = avcodec_alloc_context3(decoder);
    if (!decoder_ctx)
    {
        show_warning("Unable to open create decoding context");
        stop();
        return;
    }
    avcodec_parameters_to_context(decoder_ctx, format_ctx->streams[audio_stream_index]->codecpar);

    // Init audio decoder
    if (avcodec_open2(decoder_ctx, decoder, NULL) < 0)
    {
        show_warning("Unable to open audio decoder");
        stop();
        return;
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
        show_warning("Unable to allocate data packet");
        stop();
        return;
    }
    // Allocate media data frame
    frame = av_frame_alloc();
    if (!frame)
    {
        show_warning("Unable to allocate data frame");
        stop();
        return;
    }

    // Allocate resampler context
    swr_ctx = swr_alloc();
    if (!swr_ctx)
    {
        show_warning("Unable to allocate resampler context");
        stop();
        return;
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
        show_warning("Unable to init resampling context");
        stop();
        return;
    }

    // Buffer will be used as it is, no alignment
    if (av_samples_alloc_array_and_samples(&swr_data, &swr_linesize, ch_layout.nb_channels, swr_nb_samples, sample_format, 0) < 0)
    {
        show_warning("Could not allocate destination samples");
        stop();
        return;
    }

    track_state = PLAYING;
    // Update button
    button_play->setText("Pause");
}


void AudioTrack::pause()
{
    track_state = PAUSED;
    // Update button
    button_play->setText("Play");
}


void AudioTrack::resume()
{
    track_state = PLAYING;
    // Update button
    button_play->setText("Pause");
}


void AudioTrack::stop()
{
    track_state = STOPPED;

    // Dispose FFMPEG data
    free_ffmpeg_data();

    duration_cur = 0;
    // Update duration
    update_progress();
    // Update button
    button_play->setText("Play");
}


void AudioTrack::play_pause()
{
    // Play-pause cycle
    switch (track_state)
    {
        case STOPPED:
            play();
            break;
        case PLAYING:
            pause();
            break;
        case PAUSED:
            resume();
            break;
    }
}


void AudioTrack::set_volume(int value)
{
    // Update volume var and label
    volume = ((float)value)/100;
    volume_label->setText(QString::number(value));
}


void AudioTrack::update_progress()
{
    progress->setValue(duration_cur);
}


TrackFrame AudioTrack::read_samples()
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
                stop();
                return 0;
            }

            // Check packet stream id (packet can represent video)
            if (packet->stream_index == audio_stream_index)
            {
                // Send packet to decoder
                switch (avcodec_send_packet(decoder_ctx, packet))
                {
                    case AVERROR(EAGAIN):
                        stop();
                        printf("Error while sending a packet to the decoder v1");
                        return -1;
                        break;
                    case AVERROR_EOF:
                        stop();
                        printf("Error while sending a packet to the decoder v2");
                        return -1;
                        break;
                    case AVERROR(EINVAL):
                        stop();
                        printf("Error while sending a packet to the decoder v3");
                        return -1;
                        break;
                    case AVERROR(ENOMEM):
                        stop();
                        printf("Error while sending a packet to the decoder v4");
                        return -1;
                        break;
                    default:
                        if (packet==NULL)
                        {
                            stop();
                            printf("Error while sending a packet to the decoder");
                            return -1;
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
        // Other errors
        else if (res < 0)
        {
            stop();
            printf("Error while receiving a frame from the decoder");
            return -1;
        }
        // Success
        else
        {
            break;
        }
    }

    // Some reallocation might be required
    if (swr_nb_samples != frame->nb_samples)
    {
        // Realloc
        av_freep(&swr_data[0]);
        if (av_samples_alloc(swr_data, &swr_linesize, ch_layout.nb_channels, frame->nb_samples, sample_format, 1) < 0)
        {
            stop();
            printf("Error allocating converted samples");
            return -1;
        }
        swr_nb_samples = frame->nb_samples;
    }

    // Convert samples
    int samples_per_channel = swr_convert(swr_ctx, swr_data, swr_nb_samples, (const uint8_t **)frame->extended_data, frame->nb_samples);
    if (samples_per_channel < 0)
    {
        stop();
        printf("Error while converting");
        return -1;
    }
    // Get resulting buffer size
    swr_bufsize = av_samples_get_buffer_size(&swr_linesize, ch_layout.nb_channels, samples_per_channel, sample_format, 1);
    if (swr_bufsize < 0)
    {
        stop();
        printf("Could not get sample buffer size");
        return -1;
    }

    // Do not forget to dispose processed frame
    av_frame_unref(frame);

    // Return finalized data
    return TrackFrame(swr_data, ch_layout.nb_channels, swr_bufsize, swr_nb_samples, decoder_ctx->sample_rate, volume);
}