#include "MainWindow.hpp"


// Constructor
MainWindow::MainWindow(const QApplication *app, QWidget *parent, Qt::WindowFlags flags) : QMainWindow(parent, flags)
{
    try
    {
        // Initialize PortAudio
        startPortaudio();

        // Get screen geometry
        screeanGeometry = new QRect(app->primaryScreen()->availableGeometry());

        // Set title
        setWindowTitle("SoundBoard");
        // Set main window geomenty depending on screen resolution
        setMinimumSize(screeanGeometry->width()/1.5, screeanGeometry->height()/1.5);
        setGeometry(screeanGeometry->width()/10, screeanGeometry->height()/8, screeanGeometry->width()/1.5, screeanGeometry->height()/1.5);

        /*
        // Central widget:
        */
        QWidget *central_widget = new QWidget();
        setCentralWidget(central_widget);
        /* Grid layout for the central widget */
        QGridLayout *grid = new QGridLayout();
        grid->setColumnStretch(0, 2);
        grid->setColumnStretch(1, 1);
        central_widget->setLayout(grid);
        /* Additional layouts inside grid */
        QVBoxLayout *left_vertbox = new QVBoxLayout();
        QVBoxLayout *right_vertbox = new QVBoxLayout();
        right_vertbox->setAlignment(Qt::AlignTop);
        grid->addLayout(left_vertbox, 0, 0, 1, 1);
        grid->addLayout(right_vertbox, 0, 1, 1, 1);

        /*
        // Toolbar:
        */
        QToolBar *toolbar = new QToolBar("Toolbar");
        addToolBar(toolbar);
        /* Button to add tracks */
        QAction *button_select_dir = new QAction("Select Directory");
        connect(button_select_dir, &QAction::triggered, this, &MainWindow::selectDirectory);
        toolbar->addAction(button_select_dir);
        /* Button to refresh devices */
        QAction *button_refresh_devices = new QAction("Refresh Devices");
        connect(button_refresh_devices, &QAction::triggered, this, &MainWindow::refreshDevices);
        toolbar->addAction(button_refresh_devices);

        /*
        // Tracks table:
        */
        tracks = new QTableWidget();
        // Table header should streach to the and of table
        tracks->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        // Disable horisontal scroll bar
        tracks->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        // Table header
        tracks->setColumnCount(1);
        tracks->setHorizontalHeaderLabels(QStringList() << "Track Name");
        // Add to layout
        left_vertbox->addWidget(tracks);

        /*
        // Devices tabs on the right:
        */
        devices = new QTabWidget();
        devices->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
        right_vertbox->addWidget(devices);
        /* Input device */
        QComboBox *combobox_devices = new QComboBox();
        void (QComboBox:: *indexChangedSignal)(int) = &QComboBox::currentIndexChanged;
        devices->addTab(new DeviceTab(screeanGeometry, "Listen to input", true, INPUT, combobox_devices), "Input Device:");
        // Connect after device tab creation to ensure MainWindow::restartPlayers won't be called inside constructor (causing crash)
        connect(combobox_devices, indexChangedSignal, this, &MainWindow::restartPlayers);
        /* Virtual Output Cable */
        combobox_devices = new QComboBox();
        devices->addTab(new DeviceTab(screeanGeometry, "Feed to virtual output", true, OUTPUT, combobox_devices), "Virtual Output Cable:");
        connect(combobox_devices, indexChangedSignal, this, &MainWindow::restartPlayers);
        /* Output Device */
        combobox_devices = new QComboBox();
        devices->addTab(new DeviceTab(screeanGeometry, "Feed to ouput", false, OUTPUT, combobox_devices), "Output Device:");
        connect(combobox_devices, indexChangedSignal, this, &MainWindow::restartPlayers);

        // Init player managers
        microphonePlayerManager = new MicrophonePlayerManager(devices, "Microphone Rerouter");
        microphonePlayerManager->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
        right_vertbox->addWidget(microphonePlayerManager);
        mediafilesPlayerManager1 = new MediaFilesPlayerManager(devices, "Media Files Player", screeanGeometry);
        mediafilesPlayerManager1->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
        right_vertbox->addWidget(mediafilesPlayerManager1);
        mediafilesPlayerManager2 = new MediaFilesPlayerManager(devices, "Media Files Player", screeanGeometry);
        mediafilesPlayerManager2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        right_vertbox->addWidget(mediafilesPlayerManager2);
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