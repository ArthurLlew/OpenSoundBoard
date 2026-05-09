#pragma once

// Qt core (defines Q_OS_WIN among other things)
#include <QtCore/Qt>
// Qt
#include <QtCore/QRect>
#include <QtCore/QString>
#include <QtCore/QThreadPool>
#include <QtGui/QScreen>
#include <QtGui/QAction>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QAbstractItemView>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
// Custom
#include <WidgetMessages.hpp>
#include <DeviceTab.hpp>
#include <AudioTrack.hpp>
#include <AudioPlayerWidgets/MicrophonePlayerWidget.hpp>
#include <AudioPlayerWidgets/MediaFilesPlayerWidget.hpp>


/** Application main window.*/
class MainWindow: public QMainWindow, WidgetWarnings
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

    /** Mouse position picked up by mousePressEvent.*/
    QPoint mouseClickedPos;

    /** App title widget.*/
    QWidget *titleBar = nullptr;
    /** List of tracks.*/
    QTableWidget *tracks = nullptr;
    /** Devices tab.*/
    QTabWidget *devices = nullptr;

    /** Microphone player manager.*/
    MicrophonePlayerWidget *microphonePlayerWidget = nullptr;
    /** Media files player manager.*/
    MediaFilesPlayerWidget *mediafilesPlayerWidget1 = nullptr;
    /** Media files player manager.*/
    MediaFilesPlayerWidget *mediafilesPlayerWidget2 = nullptr;

public:

    /** Constructor.
     * 
     *  @param app QT application.
    */
    explicit MainWindow(const QApplication *app, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    /** Destructor.*/
    ~MainWindow();

protected:

    /** Redifinition allows usage of QSS.*/
    void paintEvent(QPaintEvent *event) override;

    /** Select directory with media files.*/
    void selectDirectory();

    /** Updates devices in active players.*/
    void updateDevices();

    /** Refreshes lists of input/output devices.*/
    void refreshDevices();
};