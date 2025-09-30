#include <AudioPlayerWidgets/AudioPlayerWidget.hpp>


AudioPlayerWidget::AudioPlayerWidget(AudioPlayer *player, QString name, QWidget *parent)
: QWidget(parent)
{
    this->player = player;
    this->name = name;

    // Create threadpool and disable thread expiry (only we terminate threads explicitly)
    threadpool = new QThreadPool();
    threadpool->setExpiryTimeout(-1);

    // Connect signal to player
    connect(this->player, AudioPlayer::signalError, this, &AudioPlayerWidget::playerError);

    // Main layout
    layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop);
    setLayout(layout);
}


AudioPlayerWidget::~AudioPlayerWidget()
{
    // Delete player and threadpool
    delete player;
    delete threadpool;
}


void AudioPlayerWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


void AudioPlayerWidget::playerError(QString message)
{
    displayWarning(name + " error:\n" + message);
    // Update player state
    stop();
}