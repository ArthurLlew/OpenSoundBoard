#include "MainWindow.hpp"


// Constructor
MainWindow::MainWindow(const QApplication *app, QWidget *parent, Qt::WindowFlags flags) : QMainWindow(parent, flags)
{
    try
    {
        // Initialize PortAudio
        start_portaudio();

        // Get screen geometry
        QScreen *screen = app->primaryScreen();
        QRect rect = screen->availableGeometry();
        screean_rect = new QRect(rect);

        // Set title
        setWindowTitle("SoundBoard");
        // Set main window geomenty depending on screen resolution
        setMinimumSize(screean_rect->width()/1.5, screean_rect->height()/1.5);
        setGeometry(screean_rect->width()/10, screean_rect->height()/8, screean_rect->width()/1.5, screean_rect->height()/1.5);

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
        connect(button_add_track, &QAction::triggered, this, &MainWindow::add_track);
        toolbar->addAction(button_add_track);
        /* Button to refresh devices */
        QAction *button_refresh_devices = new QAction("Refresh Devices");
        connect(button_refresh_devices, &QAction::triggered, this, &MainWindow::refresh_devices);
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
        devices->addTab(new DeviceTab(screean_rect, "Listen to input", true, INPUT, combobox_devices), "Input Device:");
        // Connect after device tab creation to ensure MainWindow::restart_players won't be called inside constructor (causing crash)
        connect(combobox_devices, indexChangedSignal, this, &MainWindow::restart_players);
        /* Virtual Output Cable */
        combobox_devices = new QComboBox();
        devices->addTab(new DeviceTab(screean_rect, "Feed to virtual output", true, OUTPUT, combobox_devices), "Virtual Output Cable:");
        connect(combobox_devices, indexChangedSignal, this, &MainWindow::restart_players);
        /* Output Device */
        combobox_devices = new QComboBox();
        devices->addTab(new DeviceTab(screean_rect, "Feed to ouput", false, OUTPUT, combobox_devices), "Output Device:");
        connect(combobox_devices, indexChangedSignal, this, &MainWindow::restart_players);

        // Init player managers
        microphone_player_manager = new MicrophonePlayerManager(devices, "Microphone Rerouter");
        right_vertbox->addWidget(microphone_player_manager);
        mediafiles_player_manager = new MediaFilesPlayerManager(devices, "Media Files Player", screean_rect);
        right_vertbox->addWidget(mediafiles_player_manager);
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
    stop_players();

    // Terminate PortAudio
    Pa_Terminate();

    // Delete objects we own by reference (widgets are deleted automatically)
    delete microphone_player_manager;
    delete mediafiles_player_manager;
}


void MainWindow::start_portaudio()
{
    // Initialize portaudio while checking for any error
    if (paNoError != Pa_Initialize())
    {
        show_warning("Can't init portaudio!\nApp will now close.");
        close();
    }
}


void MainWindow::add_track()
{
    // Ask to select media files
    QStringList filenames = open_file_dialog("Media (*.mp4 *.mp3 *.wav *.ogg)");
    
    // Iterate over files
    for (const auto &filename : std::as_const(filenames))
    {
        // Create sound widget
        AudioTrack *sound_item = new AudioTrack(filename, mediafiles_player_manager);
        // Create list item
        QListWidgetItem *lst_item = new QListWidgetItem(tracks);
        // Set size hint
        lst_item->setSizeHint(sound_item->sizeHint());
        // Add item into list
        tracks->addItem(lst_item);
        tracks->setItemWidget(lst_item, sound_item);
    }
}


QStringList MainWindow::open_file_dialog(QString name_filter)
{
    // Open file selection dialog
    QFileDialog dialog(this);
    dialog.setNameFilter(name_filter);
    // Multiple files selection
    dialog.setFileMode(QFileDialog::FileMode::ExistingFiles);

    // Gather selected files
    QStringList filenames;
    if (dialog.exec())
        filenames = dialog.selectedFiles();

    // Return filenames
    return filenames;
}


void MainWindow::start_players()
{
    // Check if we can launch microphone player
    if (((DeviceTab*)devices->widget(0))->combobox_devices->count() == 0)
    {
        show_warning("No input devices found!\nTry to refresh devices");
    }
    else if (((DeviceTab*)devices->widget(1))->combobox_devices->count() == 0)
    {
        show_warning("No virtual output cable found!\nTry to refresh devices");
    }
    else if (((DeviceTab*)devices->widget(2))->combobox_devices->count() == 0)
    {
        show_warning("No output devices found!\nTry to refresh devices");
    }
    else
    {
        microphone_player_manager->player_run();
    }

    // Check if we can launch mediafiles player
    if (((DeviceTab*)devices->widget(1))->combobox_devices->count() == 0)
    {
        show_warning("No virtual output cable found!\nTry to refresh devices");
    }
    else if (((DeviceTab*)devices->widget(2))->combobox_devices->count() == 0)
    {
        show_warning("No output devices found!\nTry to refresh devices");
    }
    else
    {
        mediafiles_player_manager->player_run();
    }
}


void MainWindow::stop_players()
{
    // Tell players to stop and wait till they finish
    microphone_player_manager->player_stop();
    mediafiles_player_manager->player_stop();
    microphone_player_manager->player_wait();
    mediafiles_player_manager->player_wait();
}


void MainWindow::restart_players(int unused)
{
    // What else to expect from restart :)
    stop_players();
    start_players();
}


void MainWindow::refresh_devices()
{
    // Stop players
    stop_players();

    // Reboot PortAudio
    Pa_Terminate();
    start_portaudio();
    
    // Refresh list of devices in each device tab
    for (int i=0; i<devices->count(); i++)
    {
        ((DeviceTab*)devices->widget(i))->refresh_devices();
    }

    // Start players
    start_players();
}