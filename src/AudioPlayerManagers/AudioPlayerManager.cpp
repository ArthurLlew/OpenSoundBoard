#include <AudioPlayerManagers/AudioPlayerManager.hpp>


AudioPlayerManager::AudioPlayerManager(AudioPlayer *player, QString name, QWidget *parent)
: QWidget(parent)
{
    this->player = player;
    this->name = name;

    // Create threadpool and disable thread expiry (only we terminate threads explicitly)
    threadpool = new QThreadPool();
    threadpool->setExpiryTimeout(-1);

    // Connect signal to player
    connect(this->player, AudioPlayer::signalError, this, &AudioPlayerManager::playerError);

    // Main layout
    layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop);
    setLayout(layout);
}


AudioPlayerManager::~AudioPlayerManager()
{
    // Delete player and threadpool
    delete player;
    delete threadpool;
}


void AudioPlayerManager::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


void AudioPlayerManager::playerError(QString message)
{
    displayWarning(name + " error:\n" + message);
    // Update player state
    stop();
}