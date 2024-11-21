// Windows headers (very important to put them in front of any other includes!)
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
// Header
#include "MainWindow.hpp"


// Constructor
MainWindow::MainWindow(const QApplication *app, QWidget *parent, Qt::WindowFlags flags) : QMainWindow(parent, flags)
{
    try
    {
        // Initialize PortAudio
        startPortaudio();

        // Prepair to act as a frameless window
        setWindowFlags(flags | Qt::Window);

        // Win32 windo handler setup
        HWND hwnd = HWND(winId());
        DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);
        ::SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | !WS_BORDER);
        // Margins for shadow
        const MARGINS shadow = { 1, 1, 1, 1 };
        DwmExtendFrameIntoClientArea(hwnd, &shadow);

        // Get primary screen geometry
        screeanGeometry = new QRect(app->primaryScreen()->availableGeometry());
        // Set geomenty depending on screen resolution
        setMinimumSize(screeanGeometry->width()/1.5, screeanGeometry->height()/1.5);
        setGeometry(screeanGeometry->width()/10, screeanGeometry->height()/8,
                    screeanGeometry->width()/1.5, screeanGeometry->height()/1.5);

        /*
        // Title bar:
        */
        /* Title */
        QLabel *app_title = new QLabel("OpenSoundBoard");
        /* Minimize button */
        QPushButton *button_minimize = new QPushButton("-");
        connect(button_minimize, &QPushButton::pressed, this, &MainWindow::onMinimizeClicked);
        /* Maximize button */
        QPushButton *button_maximize = new QPushButton("o");
        connect(button_maximize, &QPushButton::pressed, this, &MainWindow::onMaximizeClicked);
        /* Close button */
        QPushButton *button_close = new QPushButton("x");
        connect(button_close, &QPushButton::pressed, this, &MainWindow::onCloseClicked);
        /* Title bar widget */
        titleBar = new QWidget();
        titleBar->setObjectName("TitleBar");
        QHBoxLayout *title_bar_layout = new QHBoxLayout();
        titleBar->setLayout(title_bar_layout);
        title_bar_layout->addWidget(app_title);
        title_bar_layout->addStretch(); // Spacing
        title_bar_layout->addWidget(button_minimize);
        title_bar_layout->addWidget(button_maximize);
        title_bar_layout->addWidget(button_close);
        setMenuWidget(titleBar);

        /*
        // Central widget:
        */
        /* Grid layout for the central widget */
        QGridLayout *grid = new QGridLayout();
        grid->setColumnStretch(0, 2);
        grid->setColumnStretch(1, 1);
        /* Central widget */
        QWidget *central_widget = new QWidget();
        setCentralWidget(central_widget);
        central_widget->setLayout(grid);
        /* Additional layouts inside grid */
        QVBoxLayout *left_vertbox = new QVBoxLayout();
        QVBoxLayout *right_vertbox = new QVBoxLayout();
        grid->addLayout(left_vertbox, 0, 0, 1, 1);
        grid->addLayout(right_vertbox, 0, 1, 1, 1);

        /*
        // Toolbar:
        */
        /* Button to add tracks */
        QAction *button_select_dir = new QAction("Select Directory");
        connect(button_select_dir, &QAction::triggered, this, &MainWindow::selectDirectory);
        /* Button to refresh devices */
        QAction *button_refresh_devices = new QAction("Refresh Devices");
        connect(button_refresh_devices, &QAction::triggered, this, &MainWindow::refreshDevices);
        /* Toolbar */
        QToolBar *toolbar = new QToolBar("Toolbar");
        toolbar->setMovable(false);
        toolbar->addAction(button_select_dir);
        toolbar->addAction(button_refresh_devices);
        addToolBar(toolbar);

        /*
        // Tracks table:
        */
        tracks = new QTableWidget();
        // Table header should streach to the and of table
        tracks->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        // Disable horisontal scroll bar
        tracks->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        // Table headers
        tracks->setColumnCount(1);
        tracks->setHorizontalHeaderLabels(QStringList() << "Tracks");
        tracks->verticalHeader()->setVisible(false);
        tracks->horizontalHeader()->setDisabled(true);
        // Disable selection
        tracks->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tracks->setFocusPolicy(Qt::NoFocus);
        tracks->setSelectionMode(QAbstractItemView::NoSelection);
        // Add to layout
        left_vertbox->addWidget(tracks);

        /*
        // Devices tabs on the right:
        */
        devices = new QTabWidget();
        right_vertbox->addWidget(devices);
        /* Input device */
        QComboBox *combobox_devices = new QComboBox();
        void (QComboBox:: *indexChangedSignal)(int) = &QComboBox::currentIndexChanged;
        devices->addTab(new DeviceTab("Listen to input", true, DeviceTab::INPUT, combobox_devices), "Input Device");
        // Connect after device tab creation to ensure MainWindow::restartPlayers won't be called inside constructor (causing crash)
        connect(combobox_devices, indexChangedSignal, this, &MainWindow::restartPlayers);
        /* Virtual Output Cable */
        combobox_devices = new QComboBox();
        devices->addTab(new DeviceTab("Feed to virtual output", true, DeviceTab::OUTPUT, combobox_devices), "Virtual Output Cable");
        connect(combobox_devices, indexChangedSignal, this, &MainWindow::restartPlayers);
        /* Output Device */
        combobox_devices = new QComboBox();
        devices->addTab(new DeviceTab("Feed to ouput", false, DeviceTab::OUTPUT, combobox_devices), "Output Device");
        connect(combobox_devices, indexChangedSignal, this, &MainWindow::restartPlayers);

        /*
        // Player managers:
        */
        microphonePlayerManager = new MicrophonePlayerManager(devices, "Microphone Rerouter");
        right_vertbox->addWidget(microphonePlayerManager);
        mediafilesPlayerManager1 = new MediaFilesPlayerManager(devices, "Media Files Player", screeanGeometry);
        right_vertbox->addWidget(mediafilesPlayerManager1);
        mediafilesPlayerManager2 = new MediaFilesPlayerManager(devices, "Media Files Player", screeanGeometry);
        right_vertbox->addWidget(mediafilesPlayerManager2);
        // Add streatch to stick widgets to the top
        right_vertbox->addStretch();
    }
    catch(...)
    {
        // Terminate PortAudio on any error (since destructor will not run for not yet constructed object)
        Pa_Terminate();
    }
}


// Destructor
MainWindow::~MainWindow()
{
    // Delete player managers (players are stopped and deleted automatically inside destructor)
    delete microphonePlayerManager;
    delete mediafilesPlayerManager1;
    delete mediafilesPlayerManager2;

    // Terminate PortAudio
    Pa_Terminate();
}


void MainWindow::paintEvent(QPaintEvent *event)
{
    // Avoid compiler warnings
    Q_UNUSED(event);

    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    // Handle left mouse button
    if (event->buttons() == Qt::LeftButton)
    {
        // Check if click happened on frame header
        if (childAt(event->pos()) == titleBar)
        {
            // Save mouse position
            mouseClickedPos = event->pos();
            // Adjust it if window is maximized using size difference
            if (isMaximized())
            {
                QRect geo_curr = geometry();
                QRect geo_norm = normalGeometry();
                mouseClickedPos.setX(mouseClickedPos.x() * geo_norm.width() / geo_curr.width());
                mouseClickedPos.setY(mouseClickedPos.y() * geo_norm.height() / geo_curr.height());
            }
        }
        else
        {
            mouseClickedPos = QPoint(-1,-1);
        }
    }
}


void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    // Do nothing if left mouse button is not pressed
    if (!(event->buttons() & Qt::LeftButton))
        return;
    // Handle movement only if left mouse button was pressed on header
    if (mouseClickedPos.x() != -1 && mouseClickedPos.y() != -1)
    {
        // Move to new position
        move(event->globalPosition().toPoint() - mouseClickedPos);
        /*if(isMaximized())
        {
            ??????????????????????????????????
        }*/
    }
}


void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    // Handle left mouse button
    if (event->buttons() == Qt::LeftButton)
    {
        // Check if click happened on frame header
        if (childAt(event->pos()) == titleBar)
        {
            if (isMaximized())
            {
                //maximum->setIcon(QIcon(maximizeIcon));
                showNormal();
            }
            else
            {
                //maximum->setIcon(QIcon(defaultSizeIcon));
                showMaximized();
            }
        }
    }
}


bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    // Avoid compiler warnings
    Q_UNUSED(eventType)

    // Parse message
    MSG *msg = static_cast<MSG*>(message);

    // Handle event
    switch (msg->message)
    {
        case WM_NCCALCSIZE:
        {
            // Redraw for extra safety
            *result = WVR_REDRAW;
            return true;
        }
        case WM_NCHITTEST:
        {
            // Get cursor positions (also take screen scaling into account)
            QPoint globalPos(GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam));
            globalPos.setX(qRound(globalPos.x() / devicePixelRatio()));
            globalPos.setY(qRound(globalPos.y() / devicePixelRatio()));
            QPoint localPos = mapFromGlobal(globalPos);
            int localX = localPos.x();
            int localY = localPos.y();
            // Add some spacing, so user can actually hit that gap with mouse
            int borderPad = 4;

            // Left side
            if (localX >= 0 && localX <= borderPad)
            {
                // Top left
                if (localY >= 0 && localY <= borderPad)
                {
                    *result = HTTOPLEFT;
                }
                // Bottom left
                else if (localY >= height() - borderPad)
                {
                    *result = HTBOTTOMLEFT;
                }
                // Middle left
                else
                {
                    *result = HTLEFT;
                }
            }
            // Right side
            else if (localX >= width() - borderPad)
            {
                // Top right
                if (localY >= 0 && localY <= borderPad)
                {
                    *result = HTTOPRIGHT;
                }
                // Bottom right
                else if (localY >= height() - borderPad)
                {
                    *result = HTBOTTOMRIGHT;
                }
                // Middle right
                else
                {
                    *result = HTRIGHT;
                }
            }
            // Middle top
            else if (localY >= 0 && localY <= borderPad)
            {
                *result = HTTOP;
            }
            // Middle bottom
            else if (localY >= height() - borderPad && localY <= height())
            {
                *result = HTBOTTOM;
            }
            else
            {
                // Default handling
                return QMainWindow::nativeEvent(eventType, message, result);
            }

            // We handled event
            return true;
        }
        default:
            break;
    }

    // Default handling
    return QMainWindow::nativeEvent(eventType, message, result);
}


void MainWindow::onMinimizeClicked(){
    showMinimized();
}


void MainWindow::onMaximizeClicked(){
    if (isMaximized())
    {
        //maximum->setIcon(QIcon(maximizeIcon));
        showNormal();
    }
    else
    {
        //maximum->setIcon(QIcon(defaultSizeIcon));
        showMaximized();
    }
}


void MainWindow::onCloseClicked(){
    close();
}


void MainWindow::startPortaudio()
{
    // Initialize portaudio while checking for any error
    if (paNoError != Pa_Initialize())
    {
        displayWarning("Can't init portaudio!\nApp will now close.");
        close();
    }
}


void MainWindow::selectDirectory()
{
    // Open file selection dialog
    QFileDialog dialog(this);
    // Directory selection
    dialog.setFileMode(QFileDialog::FileMode::Directory);

    // Ask to select directory and get result
    QStringList dir_or_none;
    if (dialog.exec())
        dir_or_none = dialog.selectedFiles();
    
    // Check if user discarded selection
    if (dir_or_none.count() != 0)
    {
        // Clear previous table
        tracks->clearContents();

        // Get media files in that directory and prepair new table
        QDir directory(dir_or_none[0]);
        QStringList mediafiles = directory.entryList(QStringList() << "*.mp4" << "*.mp3" << "*.wav" << "*.ogg", QDir::Files);
        tracks->setRowCount(mediafiles.count());

        // Add them to list
        for (int i = 0; i < mediafiles.count(); i++)
        {
            // Create sound widget
            AudioTrack *sound_item = new AudioTrack(directory.path() + "/" + mediafiles[i]);
            // Create table item and set its size
            QTableWidgetItem *table_item = new QTableWidgetItem();
            table_item->setSizeHint(sound_item->sizeHint());
            // Add item into table
            tracks->setItem(i, 0, table_item);
            tracks->setCellWidget(i, 0, sound_item);
        }
    }
}


void MainWindow::startPlayers(bool microphone_player, bool mediafiles_player1, bool mediafiles_player2)
{
    // Check if we can launch microphone player
    if (((DeviceTab*)devices->widget(0))->combobox_devices->count() == 0)
    {
        displayWarning("No input devices found!\nTry to refresh devices");
    }
    else if (((DeviceTab*)devices->widget(1))->combobox_devices->count() == 0)
    {
        displayWarning("No virtual output cable found!\nTry to refresh devices");
    }
    else if (((DeviceTab*)devices->widget(2))->combobox_devices->count() == 0)
    {
        displayWarning("No output devices found!\nTry to refresh devices");
    }
    else if (microphone_player)
    {
        microphonePlayerManager->playerRun();
    }

    // Check if we can launch mediafiles player
    if (((DeviceTab*)devices->widget(1))->combobox_devices->count() == 0)
    {
        displayWarning("No virtual output cable found!\nTry to refresh devices");
    }
    else if (((DeviceTab*)devices->widget(2))->combobox_devices->count() == 0)
    {
        displayWarning("No output devices found!\nTry to refresh devices");
    }
    else
    {
        if (mediafiles_player1)
            mediafilesPlayerManager1->playerRun();
        if (mediafiles_player2)
            mediafilesPlayerManager2->playerRun();
    }
}


void MainWindow::stopPlayers(bool *microphone_player, bool *mediafiles_player1, bool *mediafiles_player2)
{
    // Save players state, tell them to stop and wait till they finish
    *microphone_player = microphonePlayerManager->playerState();
    *mediafiles_player1 = mediafilesPlayerManager1->playerState();
    *mediafiles_player2 = mediafilesPlayerManager2->playerState();
    microphonePlayerManager->playerStop();
    mediafilesPlayerManager1->playerStop();
    mediafilesPlayerManager2->playerStop();
    microphonePlayerManager->playerWait();
    mediafilesPlayerManager1->playerWait();
    mediafilesPlayerManager2->playerWait();
}


void MainWindow::restartPlayers(int unused)
{
    // Restart players depending on their previous state
    bool microphone_player, mediafiles_player1, mediafiles_player2;
    stopPlayers(&microphone_player, &mediafiles_player1, &mediafiles_player2);
    startPlayers(microphone_player, mediafiles_player1, mediafiles_player2);
}


void MainWindow::refreshDevices()
{
    // Stop players
    bool microphone_player, mediafiles_player1, mediafiles_player2;
    stopPlayers(&microphone_player, &mediafiles_player1, &mediafiles_player2);

    // Reboot PortAudio
    Pa_Terminate();
    startPortaudio();
    
    // Refresh list of devices in each device tab
    for (int i=0; i<devices->count(); i++)
    {
        ((DeviceTab*)devices->widget(i))->refreshDevices();
    }

    // Start players
    startPlayers(microphone_player, mediafiles_player1, mediafiles_player2);
}