#include "PlatformUtils.hpp"

#include <GLFW/glfw3.h>

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <windows.h>
#include <commdlg.h>

#endif

#ifdef __linux__
#define GLFW_EXPOSE_NATIVE_X11
#include <gtk/gtk.h>
#endif

#include <GLFW/glfw3native.h>

// ==============Time===================

Time Time::now() {
    auto now = high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return {duration_cast<milliseconds>(duration)};
}

Time & Time::addSeconds(double seconds) {
    m_time += duration_cast<milliseconds>(duration<double>(seconds));
    return *this;
}

Time & Time::addMilliseconds(double milliseconds) {
    m_time += std::chrono::milliseconds(static_cast<long long>(milliseconds));
    return *this;
}

Time & Time::addNanoseconds(double nanoseconds) {
    m_time += duration_cast<milliseconds>(duration<long long, std::nano>(static_cast<long long>(nanoseconds)));
    return *this;
}

double Time::toSeconds() const {
    return duration<double>(m_time).count();
}

double Time::toMilliseconds() const {
    return duration<double, std::milli>(m_time).count();
}

long long Time::toNanoseconds() const {
    return duration_cast<nanoseconds>(m_time).count();
}

bool Time::operator<(const Time &rhs) const {
    return m_time < rhs.m_time;
}

bool Time::operator>(const Time &rhs) const {
    return rhs < *this;
}

bool Time::operator<=(const Time &rhs) const {
    return !(rhs < *this);
}

bool Time::operator>=(const Time &rhs) const {
    return !(*this < rhs);
}

Time & Time::operator=(const Time &rhs) {
    if (this != &rhs) {
        m_time = rhs.m_time;
    }
    return *this;
}

Time Time::operator+(const Time &rhs) const {
    return Time(m_time + rhs.m_time);
}

std::chrono::milliseconds Time::operator-(const Time &rhs) const {
    return m_time - rhs.m_time;
}

Time::operator duration<long long, std::ratio<1, 1000>>() const {
    return m_time;
}

//==============FileSystem===========
void FileSystem::setWorkingDirectory(const std::string &programWorkingDirectory) {
    try {
        std::filesystem::current_path(programWorkingDirectory);
    } catch (const std::filesystem::filesystem_error &e) {
        AT_FATAL("Error changing directory: {0}", e.what());
    }
}

std::string FileSystem::fileToString(const std::string &filePath) {
    std::ifstream fileStream(filePath);
    if (!fileStream.is_open()) {
        AT_ERROR("Could not open file: {0}", filePath);
    }

    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    return buffer.str();
}


//=============FileDialogs=============

std::string FileDialogs::openFile(const char *filter) {
    auto glfwWindow = glfwGetCurrentContext();
    if (glfwWindow == nullptr) {
        throw std::runtime_error("No glfw window in context, create a window before running this.");
    }

#ifdef _WIN32
    // Windows code
    OPENFILENAMEA ofn;
    CHAR szFile[260] = {0};
    CHAR currentDir[256] = {0};
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = glfwGetWin32Window(glfwWindow);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    if (GetCurrentDirectoryA(256, currentDir)) {
        ofn.lpstrInitialDir = currentDir;
    }
    ofn.lpstrFilter = filter; // https://learn.microsoft.com/en-us/dotnet/api/microsoft.win32.filedialog.filter?view=windowsdesktop-8.0
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn) == TRUE)
        return ofn.lpstrFile;

#endif // _WIN32

#ifdef __linux__
    // Linux code (GTK)
    if (!gtk_init_check(nullptr, nullptr)) {
        throw std::runtime_error("Failed to initialize GTK.");
    }

    GtkWidget* dialog = gtk_file_chooser_dialog_new(
        "Open File",
        GTK_WINDOW(glfwGetWindowUserPointer(glfwWindow)),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        nullptr);

    // Set the file filter (if provided)
    if (filter != nullptr) {
        GtkFileFilter* fileFilter = gtk_file_filter_new();
        gtk_file_filter_set_name(fileFilter, "File Filter");
        gtk_file_filter_add_pattern(fileFilter, filter);
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), fileFilter);
    }

    int result = gtk_dialog_run(GTK_DIALOG(dialog));
    std::string filePath;

    if (result == GTK_RESPONSE_ACCEPT) {
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        filePath = filename;
        g_free(filename);
    }

    gtk_widget_destroy(dialog);

    return filePath;
#endif // __linux__

    return {};
}

std::string FileDialogs::saveFile(const char *filter) {
    auto glfwWindow = glfwGetCurrentContext();
    if (glfwWindow == nullptr) {
        throw std::runtime_error("No glfw window in context, create a window before running this.");
    }

#ifdef _WIN32
    OPENFILENAMEA ofn;
    CHAR szFile[260] = {0};
    CHAR currentDir[256] = {0};
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = glfwGetWin32Window(glfwWindow);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    if (GetCurrentDirectoryA(256, currentDir)) {
        ofn.lpstrInitialDir = currentDir;
    }
    ofn.lpstrFilter = filter; // https://learn.microsoft.com/en-us/dotnet/api/microsoft.win32.filedialog.filter?view=windowsdesktop-8.0
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

    ofn.lpstrDefExt = strchr(filter, '\0') + 1;

    if (GetSaveFileNameA(&ofn) == TRUE)
        return ofn.lpstrFile;

#endif // _WIN32

#ifdef __linux__
    // Linux code (GTK)
        if (!gtk_init_check(nullptr, nullptr)) {
            throw std::runtime_error("Failed to initialize GTK.");
        }

        GtkWidget* dialog = gtk_file_chooser_dialog_new(
            "Save File",
            GTK_WINDOW(glfwGetWindowUserPointer(glfwWindow)),
            GTK_FILE_CHOOSER_ACTION_SAVE,
            "_Cancel", GTK_RESPONSE_CANCEL,
            "_Save", GTK_RESPONSE_ACCEPT,
            nullptr);

        // Set the file filter (if provided)
        if (filter != nullptr) {
            GtkFileFilter* fileFilter = gtk_file_filter_new();
            gtk_file_filter_set_name(fileFilter, "File Filter");
            gtk_file_filter_add_pattern(fileFilter, filter);
            gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), fileFilter);
        }

        int result = gtk_dialog_run(GTK_DIALOG(dialog));
        std::string filePath;

        if (result == GTK_RESPONSE_ACCEPT) {
            char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
            filePath = filename;
            g_free(filename);
        }

        gtk_widget_destroy(dialog);

        return filePath;
#endif // __linux__

    return {};
}
