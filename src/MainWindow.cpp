#include "MainWindow.hpp"


// Constructor
MainWindow::MainWindow(const QApplication *app, QWidget *parent, Qt::WindowFlags flags) : QMainWindow(parent, flags)
{
    #ifdef Q_OS_WIN
    // Set darkmode for Windows
    BOOL use_dark_mode = true;
    DwmSetWindowAttribute(HWND(winId()), DWMWINDOWATTRIBUTE::DWMWA_USE_IMMERSIVE_DARK_MODE,
                            &use_dark_mode, sizeof(use_dark_mode));
    #endif

    // Set application title
    setWindowTitle("OpenSoundBoard");

    // Get primary screen geometry
    screeanGeometry = new QRect(app->primaryScreen()->availableGeometry());
    // Set geomenty depending on screen resolution
    setMinimumSize(screeanGeometry->width()/1.5, screeanGeometry->height()/1.5);
    setGeometry(screeanGeometry->width()/10, screeanGeometry->height()/8,
                screeanGeometry->width()/1.5, screeanGeometry->height()/1.5);

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
    tracks->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
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
    connect(combobox_devices, indexChangedSignal, this, &MainWindow::updateDevices);
    /* Virtual Output Cable */
    combobox_devices = new QComboBox();
    devices->addTab(new DeviceTab("Feed to virtual output", true, DeviceTab::OUTPUT, combobox_devices), "Virtual Output Cable");
    connect(combobox_devices, indexChangedSignal, this, &MainWindow::updateDevices);
    /* Output Device */
    combobox_devices = new QComboBox();
    devices->addTab(new DeviceTab("Feed to ouput", false, DeviceTab::OUTPUT, combobox_devices), "Output Device");
    connect(combobox_devices, indexChangedSignal, this, &MainWindow::updateDevices);

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


// Destructor
MainWindow::~MainWindow()
{
    // Delete player managers (players are stopped and deleted automatically inside destructor)
    delete microphonePlayerManager;
    delete mediafilesPlayerManager1;
    delete mediafilesPlayerManager2;
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


bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    #ifdef Q_OS_WIN
    // Parse message
    MSG *msg = static_cast<MSG*>(message);

    // Handle event
    switch (msg->message)
    {
        case WM_NCCALCSIZE:
        {
            // Adjust gap between app client area and title bar
            ((NCCALCSIZE_PARAMS*)msg->lParam)->rgrc->top -= 1;
            break;
        }
        default:
            break;
    }
    #endif

    // Default handling
    return QMainWindow::nativeEvent(eventType, message, result);
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


void MainWindow::updateDevices()
{
    microphonePlayerManager->updateDevices();
    mediafilesPlayerManager1->updateDevices();
    mediafilesPlayerManager2->updateDevices();
}


void MainWindow::refreshDevices()
{
    // Refresh list of devices in each device tab
    for (int i=0; i<devices->count(); i++)
    {
        ((DeviceTab*)devices->widget(i))->refreshDevices();
    }

    updateDevices();
}