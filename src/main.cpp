#include <csignal>

#include <QApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QTranslator>
#include <QMessageBox>
#include <QStandardPaths>
#include <QLocalSocket>
#include <QLocalServer>
#include <QThread>

#include "include/global/Configs.hpp"

#include "include/ui/mainwindow_interface.h"

#ifdef Q_OS_WIN
#include "include/sys/windows/MiniDump.h"
#include "include/sys/windows/eventHandler.h"
#include "include/sys/windows/WinVersion.h"
#include <qfontdatabase.h>
#endif
#ifdef Q_OS_LINUX
#include "include/sys/linux/desktopinfo.h"
#include <qfontdatabase.h>
#endif

void signal_handler(int signum) {
    if (GetMainWindow()) {
        GetMainWindow()->prepare_exit();
        qApp->quit();
    }
}

QTranslator* trans = nullptr;
QTranslator* trans_qt = nullptr;

void loadTranslate(const QString& locale) {
    QT_TRANSLATE_NOOP("QPlatformTheme", "Cancel");
    QT_TRANSLATE_NOOP("QPlatformTheme", "Apply");
    QT_TRANSLATE_NOOP("QPlatformTheme", "Yes");
    QT_TRANSLATE_NOOP("QPlatformTheme", "No");
    QT_TRANSLATE_NOOP("QPlatformTheme", "OK");
    if (trans != nullptr) {
        trans->deleteLater();
    }
    if (trans_qt != nullptr) {
        trans_qt->deleteLater();
    }
    //
    trans = new QTranslator;
    trans_qt = new QTranslator;
    QLocale::setDefault(QLocale(locale));
    //
    if (trans->load(":/translations/" + locale + ".qm")) {
        QCoreApplication::installTranslator(trans);
    }
}

#define LOCAL_SERVER_PREFIX "nekobox-"

#ifdef Q_OS_WIN
#include <tuple>
#include <windows.h>
#include <QRandomGenerator>
#include <ostream>
#include <iostream>
#include <fstream>
#include <istream>
#include <3rdparty/WinCommander.hpp>
#include "3rdparty/WinCommander.hpp"


QString getRandomString(int length)
{
    const QString charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                            "0123456789";

    QString result;
    result.reserve(length);

    for (int i = 0; i < length; ++i) {
        int index = QRandomGenerator::global()->bounded(charset.length());
        result.append(charset.at(index));
    }

    return result;
}

QString getRandomPipeName()
{
    QString randomPart = getRandomString(12);
    return QStringLiteral(R"(\\.\pipe\QtPipe_%1)").arg(randomPart);
}

std::tuple<HANDLE, QString> createRandomNamedPipe(){
	QString pipeName = getRandomPipeName();
	
    HANDLE hPipe = CreateNamedPipe(
		(LPCWSTR)pipeName.utf16(), // Convert QString to LPCWSTR
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		1,
		4096, 4096,
		0,
		NULL
	);

	if (hPipe == INVALID_HANDLE_VALUE) {
		qCritical() << "Failed to create pipe";
		return std::make_tuple(INVALID_HANDLE_VALUE, pipeName);
	}

	BOOL connected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
	if (!connected) {
		qCritical() << "Failed to connect pipe";
		CloseHandle(hPipe);
		return std::make_tuple(INVALID_HANDLE_VALUE, pipeName);
	}
	return std::make_tuple(hPipe, pipeName);
}

// Struct to pass parameters to the thread
struct StreamThreadParams {
    HANDLE handle;
    std::ostream* output;
};

// Thread function: read from handle, write to ostream
DWORD WINAPI streamThreadFunc(LPVOID lpParam) {
    StreamThreadParams* params = static_cast<StreamThreadParams*>(lpParam);
    HANDLE hRead = params->handle;
    std::ostream& out = *(params->output);

    const DWORD bufferSize = 1024;
    char buffer[bufferSize];
    DWORD bytesRead;

    while (true) {
        BOOL success = ReadFile(hRead, buffer, bufferSize, &bytesRead, NULL);
        if (!success || bytesRead == 0) {
            break;  // EOF or error
        }
        out.write(buffer, bytesRead);
        out.flush();  // Optional
    }

    delete params;  // Clean up allocated memory
    return 0;
}


//  Create a thread that reads from handle and writes to ostream
HANDLE startStreamThread(HANDLE readHandle, std::ostream& output) {
    auto* params = new StreamThreadParams{ readHandle, &output };

    HANDLE hThread = CreateThread(
        NULL,              // default security
        0,                 // default stack size
        streamThreadFunc,  // thread function
        params,            // argument
        0,                 // creation flags
        NULL               // thread ID
    );

    if (!hThread) {
        std::cerr << "Failed to create thread. Error: " << GetLastError() << "\n";
        delete params;
        return NULL;
    }

    return hThread; // You can WaitForSingleObject(hThread, ...) if needed
}

int runCoreUACMode(int argc, char *argv[]){
	if (argc < 3){
		return -1000;
	}
	QString program = argv[1];
	if (program != "sudo"){
		return -1000;
	}
	program = argv[2];
	QStringList arguments;
	for (int i = 2; i < argc; i ++){
		arguments << argv[i];
	}
	HANDLE perr, pout;
	QString perr_path, pout_path;
	std::tie(perr, perr_path) = createRandomNamedPipe();
	std::tie(pout, pout_path) = createRandomNamedPipe();
	arguments << "-redirect-error";
	arguments << perr_path;
	arguments << "-redirect-output";
	arguments << pout_path;
	int ret = (int) WinCommander::runProcessElevated(program, arguments, "", WinCommander::SW_HIDE, true);
	auto th1 = startStreamThread(perr, std::cerr);
	auto th2 = startStreamThread(pout, std::cout);
	CloseHandle(perr);
	CloseHandle(pout);
	WaitForSingleObject(th1, INFINITE);
	WaitForSingleObject(th2, INFINITE);
	return ret;
}

#endif

int main(int argc, char* argv[]) {
    // Core dump
#ifdef Q_OS_WIN
    Windows_SetCrashHandler();
	int ret_code = runCoreUACMode(argc, argv);
	if (ret_code != -1000){
		return ret_code;
	}
#endif

    QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
    QApplication::setQuitOnLastWindowClosed(false);
    QApplication a(argc, argv);

#if !defined(Q_OS_MACOS) && (QT_VERSION >= QT_VERSION_CHECK(6,9,0))
    // Load the emoji fonts
#ifdef Q_OS_WIN
    int fontId = QFontDatabase::addApplicationFont(WinVersion::IsBuildNumGreaterOrEqual(BuildNumber::Windows_11_22H2) ? ":/font/notoEmoji" : ":/font/Twemoji");
#else
    int fontId = QFontDatabase::addApplicationFont(":/font/notoEmoji");
#endif
    if (fontId >= 0)
    {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        QFontDatabase::setApplicationEmojiFontFamilies(fontFamilies);
    } else
    {
        qDebug() << "could not load noto font!";
    }
#endif

    // Clean
    QDir::setCurrent(QApplication::applicationDirPath());
    if (QFile::exists("updater.old")) {
        QFile::remove("updater.old");
    }

    // Flags
    Configs::dataStore->argv = QApplication::arguments();
    if (Configs::dataStore->argv.contains("-many")) Configs::dataStore->flag_many = true;
    if (Configs::dataStore->argv.contains("-appdata")) {
        Configs::dataStore->flag_use_appdata = true;
        int appdataIndex = Configs::dataStore->argv.indexOf("-appdata");
        if (Configs::dataStore->argv.size() > appdataIndex + 1 && !Configs::dataStore->argv.at(appdataIndex + 1).startsWith("-")) {
            Configs::dataStore->appdataDir = Configs::dataStore->argv.at(appdataIndex + 1);
        }
    }
    if (Configs::dataStore->argv.contains("-tray")) Configs::dataStore->flag_tray = true;
    if (Configs::dataStore->argv.contains("-debug")) Configs::dataStore->flag_debug = true;
    if (Configs::dataStore->argv.contains("-flag_restart_tun_on")) Configs::dataStore->flag_restart_tun_on = true;
    if (Configs::dataStore->argv.contains("-flag_restart_dns_set")) Configs::dataStore->flag_dns_set = true;
#ifdef NKR_CPP_USE_APPDATA
    Configs::dataStore->flag_use_appdata = true; // Example: Package & MacOS
#endif
#ifdef NKR_CPP_DEBUG
    Configs::dataStore->flag_debug = true;
#endif

    // dirs & clean
    auto wd = QDir(QApplication::applicationDirPath());
    if (Configs::dataStore->flag_use_appdata) {
        QApplication::setApplicationName("nekobox");
        if (!Configs::dataStore->appdataDir.isEmpty()) {
            wd.setPath(Configs::dataStore->appdataDir);
        } else {
            wd.setPath(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
        }
    }
    if (!wd.exists()) wd.mkpath(wd.absolutePath());
    if (!wd.exists("config")) wd.mkdir("config");
    QDir::setCurrent(wd.absoluteFilePath("config"));
    QDir("temp").removeRecursively();

#ifdef Q_OS_LINUX
    QApplication::addLibraryPath(QApplication::applicationDirPath() + "/usr/plugins");
#endif

    // dispatchers
    DS_cores = new QThread;
    DS_cores->start();

// icons
    QIcon::setFallbackSearchPaths(QStringList{
        ":/icon",
    });

    // icon for no theme
    if (QIcon::themeName().isEmpty()) {
        QIcon::setThemeName("breeze");
    }

    // Dir
    QDir dir;
    bool dir_success = true;
    if (!dir.exists("profiles")) {
        dir_success &= dir.mkdir("profiles");
    }
    if (!dir.exists("groups")) {
        dir_success &= dir.mkdir("groups");
    }
    if (!dir.exists(ROUTES_PREFIX_NAME)) {
        dir_success &= dir.mkdir(ROUTES_PREFIX_NAME);
    }
    if (!dir_success) {
        QMessageBox::critical(nullptr, "Error", "No permission to write " + dir.absolutePath());
        return 1;
    }

    // migrate the old config file
    if (QFile::exists("groups/nekobox.json"))
    {
        QFile::rename("groups/nekobox.json", "configs.json");
    }

    // Load dataStore
    Configs::dataStore->fn = "configs.json";
    auto isLoaded = Configs::dataStore->Load();
    if (!isLoaded) {
        Configs::dataStore->Save();
    }

#ifdef Q_OS_WIN
    if (Configs::dataStore->windows_set_admin && !Configs::IsAdmin() && !Configs::dataStore->disable_run_admin)
    {
        Configs::dataStore->windows_set_admin = false; // so that if permission denied, we will run as user on the next run
        Configs::dataStore->Save();
        WinCommander::runProcessElevated(QApplication::applicationFilePath(), {}, "", WinCommander::SW_NORMAL, false);
        QApplication::quit();
        return 0;
    }
#endif

    // Datastore & Flags
    if (Configs::dataStore->start_minimal) Configs::dataStore->flag_tray = true;

    // load routing and shortcuts
    Configs::dataStore->routing = std::make_unique<Configs::Routing>();
    Configs::dataStore->routing->fn = ROUTES_PREFIX + "Default";
    isLoaded = Configs::dataStore->routing->Load();
    if (!isLoaded) {
        Configs::dataStore->routing->Save();
    }

    Configs::dataStore->shortcuts = std::make_unique<Configs::Shortcuts>();
    Configs::dataStore->shortcuts->fn = "shortcuts.json";
    isLoaded = Configs::dataStore->shortcuts->Load();
    if (!isLoaded) {
        Configs::dataStore->shortcuts->Save();
    }

    // Translate
    QString locale;
    switch (Configs::dataStore->language) {
        case 1: // English
            break;
        case 2:
            locale = "zh_CN";
            break;
        case 3:
            locale = "fa_IR"; // farsi(iran)
            break;
        case 4:
            locale = "ru_RU"; // Russian
            break;
        default:
            locale = QLocale().name();
    }
    QGuiApplication::tr("QT_LAYOUT_DIRECTION");
    loadTranslate(locale);

    // Check if another instance is running
    QByteArray hashBytes = QCryptographicHash::hash(wd.absolutePath().toUtf8(), QCryptographicHash::Md5).toBase64(QByteArray::OmitTrailingEquals);
    hashBytes.replace('+', '0').replace('/', '1');
    auto serverName = LOCAL_SERVER_PREFIX + QString::fromUtf8(hashBytes);
    qDebug() << "server name: " << serverName;
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if (socket.waitForConnected(250))
    {
        qDebug() << "Another instance is running, let's wake it up and quit";
        socket.disconnectFromServer();
        return 0;
    }

    // QLocalServer
    QLocalServer server(qApp);
    server.setSocketOptions(QLocalServer::WorldAccessOption);
    if (!server.listen(serverName)) {
        qWarning() << "Failed to start QLocalServer! Error:" << server.errorString();
        return 1;
    }
    QObject::connect(&server, &QLocalServer::newConnection, qApp, [&] {
        auto s = server.nextPendingConnection();
        qDebug() << "Another instance tried to wake us up on " << serverName << s;
        s->close();
        // raise main window
        MW_dialog_message("", "Raise");
    });
    QObject::connect(qApp, &QApplication::aboutToQuit, [&]
    {
        server.close();
        QLocalServer::removeServer(serverName);
    });

#ifdef Q_OS_LINUX
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
#endif

#ifdef Q_OS_WIN
    auto eventFilter = new PowerOffTaskkillFilter(signal_handler);
    a.installNativeEventFilter(eventFilter);
#endif

#ifdef Q_OS_MACOS
    QObject::connect(qApp, &QGuiApplication::commitDataRequest, [&](QSessionManager &manager)
    {
        Q_UNUSED(manager);
        signal_handler(0);
    });
#endif

    UI_InitMainWindow();
    return QApplication::exec();
}
