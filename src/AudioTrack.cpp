#include <AudioTrack.hpp>


AudioTrack::AudioTrack(QString filepath, QWidget *parent) : QWidget(parent)
{
    this->filepath = filepath;

    // Layout
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setAlignment(Qt::AlignLeft);
    setLayout(layout);
    // Name
    name = filepath.mid(filepath.lastIndexOf('/') + 1);
    layout->addWidget(new QLabel(name));
}


void AudioTrack::mousePressEvent(QMouseEvent *event)
{
    // Handle left mouse button
    if (event->buttons() == Qt::LeftButton)
    {
        // Save click position
        dragStartPosition = event->pos();
    }
}


void AudioTrack::mouseMoveEvent(QMouseEvent *event)
{
    // Do nothing if left mouse button is not pressed
    if (!(event->buttons() & Qt::LeftButton))
        return;
    // We only handle sufficient movement
    if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;

    // Create draggable object and fill MIME data with fila path and name ('?' is not allowed in file
    // path so use it as delimiter)
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("filepath&name", (filepath + "?" + name).toUtf8());
    drag->setMimeData(mimeData);

    // Invoke drag action
    Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction);
}