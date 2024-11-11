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
        central_widget = new QWidget();
        setCentralWidget(central_widget);
        /* Grid layout for the central widget */
        QGridLayout *grid = new QGridLayout();
        grid->setColumnStretch(0, 2);
        grid->setColumnStretch(1, 1);
        central_widget->setLayout(grid);
        /* Additional layouts inside grid */
        QVBoxLayout *left_vertbox = new QVBoxLayout();
        QVBoxLayout *right_vertbox = new QVBoxLayout();
        grid->addLayout(left_vertbox, 0, 0, 1, 1);
        grid->addLayout(right_vertbox, 0, 1, 1, 1);

        /*
        // Toolbar:
        */
        QToolBar *toolbar = new QToolBar("Toolbar");
        addToolBar(toolbar);
        /* Button to add tracks */
        QAction *button_add_track = new QAction("Add Track");
        connect(button_add_track, &QAction::triggered, this, &MainWindow::addTrack);
        toolbar->addAction(button_add_track);
        /* Button to refresh devices */
        QAction *button_refresh_devices = new QAction("Refresh Devices");
        connect(button_refresh_devices, &QAction::triggered, this, &MainWindow::refreshDevices);
        toolbar->addAction(button_refresh_devices);

        /*
        // Tracks list:
        */
        tracks = new QListWidget();
        tracks->setSelectionMode(QAbstractItemView::NoSelection);
        left_vertbox->addWidget(tracks);

        /*
        // Button placeholder:
        */
        QPushButton *button = new QPushButton("Placeholder button");
        //connect(button, &QPushButton::pressed, this, &MainWindow::stop_all);
        left_vertbox->addWidget(button);

        /*
        // Devices tabs on the right:
        */
        devices = new QTabWidget();
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
        right_vertbox->addWidget(microphonePlayerManager);
        mediafilesPlayerManager = new MediaFilesPlayerManager(devices, "Media Files Player", screeanGeometry);
        right_vertbox->addWidget(mediafilesPlayerManager);
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
    microphonePlayerManager->player_stop();
    mediafilesPlayerManager->player_stop();
    microphonePlayerManager->player_wait();
    mediafilesPlayerManager->player_wait();

    // Terminate PortAudio
    Pa_Terminate();

    // Delete objects we own by reference (widgets are deleted automatically)
    delete microphonePlayerManager;
    delete mediafilesPlayerManager;
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


void MainWindow::addTrack()
{
    // Ask to select media files
    QStringList filenames = openFilesDialog("Media (*.mp4 *.mp3 *.wav *.ogg)");
    
    // Iterate over files
    for (const auto &filename : std::as_const(filenames))
    {
        // Create sound widget
        AudioTrack *sound_item = new AudioTrack(filename, mediafilesPlayerManager);
        // Create list item
        QListWidgetItem *lst_item = new QListWidgetItem(tracks);
        // Set size hint
        lst_item->setSizeHint(sound_item->sizeHint());
        // Add item into list
        tracks->addItem(lst_item);
        tracks->setItemWidget(lst_item, sound_item);
    }
}


QStringList MainWindow::openFilesDialog(QString filter)
{
    // Open file selection dialog
    QFileDialog dialog(this);
    dialog.setNameFilter(filter);
    // Multiple files selection
    dialog.setFileMode(QFileDialog::FileMode::ExistingFiles);

    // Gather selected files
    QStringList filenames;
    if (dialog.exec())
        filenames = dialog.selectedFiles();

    // Return filenames
    return filenames;
}


void MainWindow::startPlayers(bool microphone_player, bool mediafiles_player)
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
        microphonePlayerManager->player_run();
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
    else if (mediafiles_player)
    {
        mediafilesPlayerManager->player_run();
    }
}


void MainWindow::stopPlayers(bool *microphone_player, bool *mediafiles_player)
{
    // Save players state, tell them to stop and wait till they finish
    *microphone_player = microphonePlayerManager->player_state();
    *mediafiles_player = mediafilesPlayerManager->player_state();
    microphonePlayerManager->player_stop();
    mediafilesPlayerManager->player_stop();
    microphonePlayerManager->player_wait();
    mediafilesPlayerManager->player_wait();
}


void MainWindow::restartPlayers(int unused)
{
    // Restart players depending on their previous state
    bool microphone_player, mediafiles_player;
    stopPlayers(&microphone_player, &mediafiles_player);
    startPlayers(microphone_player, mediafiles_player);
}


void MainWindow::refreshDevices()
{
    // Stop players
    bool microphone_player, mediafiles_player;
    stopPlayers(&microphone_player, &mediafiles_player);

    // Reboot PortAudio
    Pa_Terminate();
    startPortaudio();
    
    // Refresh list of devices in each device tab
    for (int i=0; i<devices->count(); i++)
    {
        ((DeviceTab*)devices->widget(i))->refreshDevices();
    }

    // Start players
    startPlayers(microphone_player, mediafiles_player);
}