#include "FileManager.h"

wstring readFileContents(const wstring& filePath) {
    wifstream file(filePath);
    if (!file.is_open()) {
        return L"Failed to open file.";
    }
    file.imbue(locale(locale::empty(), new codecvt_utf8<wchar_t>));
    wstring content((istreambuf_iterator<wchar_t>(file)), istreambuf_iterator<wchar_t>());
    return content;
}

vector<wstring> getDirectoryContents(const wstring& path) {
    vector<wstring> contents;
    wstring search_path = path + L"\\*";
    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(search_path.c_str(), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (wcscmp(fd.cFileName, L".") != 0 && wcscmp(fd.cFileName, L"..") != 0) {
                contents.push_back(fd.cFileName);
            }
        } while (FindNextFileW(hFind, &fd) != 0);
        FindClose(hFind);
    }
    return contents;
}

bool copyItem(const wstring& source, const wstring& destination) {
    if (PathFileExistsW(source.c_str())) {
        if (GetFileAttributesW(source.c_str()) & FILE_ATTRIBUTE_DIRECTORY) {
            return CopyFileW(source.c_str(), destination.c_str(), FALSE);
        }
        else {
            return CopyFileW(source.c_str(), destination.c_str(), FALSE);
        }
    }
    return false;
}

bool moveItem(const wstring& source, const wstring& destination) {
    if (PathFileExistsW(source.c_str())) {
        return MoveFileW(source.c_str(), destination.c_str());
    }
    return false;
}

void setConsoleColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void displayFiles(const wstring& current_directory, const vector<wstring>& files, int cursor_position, const wstring& marked_item, bool is_cut) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int console_height = 20;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        console_height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }

    wcout << L"Current Directory: " << current_directory << endl;
    int start_index = max(0, cursor_position - console_height / 2);
    int end_index = min(static_cast<int>(files.size()), start_index + console_height - 2);

    for (int i = start_index; i < end_index; ++i) {
        if (current_directory + L"\\" + files[i] == marked_item) {
            setConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
            wcout << L"   " << files[i];
            if (is_cut) {
                wcout << L" [CUT]";
            }
            else {
                wcout << L" [COPIED]";
            }
        }
        else if (i == cursor_position) {
            setConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            wcout << L"-> " << files[i];
        }
        else {
            wcout << L"   " << files[i];
        }
        setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        wcout << endl;
    }
}

void handleEnterKey(wstring& current_directory, vector<wstring>& files, int& cursor_position) {
    if (!files.empty()) {
        wstring selected_item = files[cursor_position];
        wstring full_path = current_directory + L"\\" + selected_item;
        DWORD attribs = GetFileAttributesW(full_path.c_str());
        if (attribs != INVALID_FILE_ATTRIBUTES && (attribs & FILE_ATTRIBUTE_DIRECTORY)) {
            current_directory = full_path;
            files = getDirectoryContents(current_directory);
            cursor_position = 0;
        }
    }
}

void handleBackspaceKey(wstring& current_directory, vector<wstring>& files, int& cursor_position) {
    size_t pos = current_directory.find_last_of(L"\\");
    if (pos != wstring::npos) {
        current_directory = current_directory.substr(0, pos);
        if (current_directory.empty()) {
            current_directory = L"C:\\";
        }
        files = getDirectoryContents(current_directory);
        cursor_position = 0;
    }
    else {
        wchar_t buffer[MAX_PATH];
        GetFullPathNameW(current_directory.c_str(), MAX_PATH, buffer, NULL);
        wstring full_path(buffer);
        size_t root_pos = full_path.find_first_of(L"\\");
        if (root_pos != wstring::npos) {
            current_directory = full_path.substr(0, root_pos + 1);
            files = getDirectoryContents(current_directory);
            cursor_position = 0;
        }
    }
}

void handleDeleteKey(vector<wstring>& files, const wstring& current_directory, int& cursor_position) {
    if (!files.empty()) {
        wstring selected_item = files[cursor_position];
        wstring full_path = current_directory + L"\\" + selected_item;
        DWORD attribs = GetFileAttributesW(full_path.c_str());
        if (attribs != INVALID_FILE_ATTRIBUTES) {
            if (attribs & FILE_ATTRIBUTE_DIRECTORY) {
                if (RemoveDirectoryW(full_path.c_str()) == 0) {
                    wcerr << L"Failed to remove directory: " << full_path << L" Error: " << GetLastError() << endl;
                }
                else {
                    files.erase(files.begin() + cursor_position);
                    if (cursor_position >= static_cast<int>(files.size())) {
                        cursor_position = static_cast<int>(files.size()) - 1;
                    }
                }
            }
            else {
                if (DeleteFileW(full_path.c_str()) == 0) {
                    wcerr << L"Failed to delete file: " << full_path << L" Error: " << GetLastError() << endl;
                }
                else {
                    files.erase(files.begin() + cursor_position);
                    if (cursor_position >= static_cast<int>(files.size())) {
                        cursor_position = static_cast<int>(files.size()) - 1;
                    }
                }
            }
        }
    }
}

void handleF1Key(const vector<wstring>& files, const wstring& current_directory, int cursor_position, bool& view_mode, wstring& file_content) {
    if (!files.empty()) {
        wstring selected_item = files[cursor_position];
        wstring full_path = current_directory + L"\\" + selected_item;
        DWORD attribs = GetFileAttributesW(full_path.c_str());
        if (attribs != INVALID_FILE_ATTRIBUTES && !(attribs & FILE_ATTRIBUTE_DIRECTORY)) {
            file_content = readFileContents(full_path);
            view_mode = true;
        }
    }
}

void handleF2Key(vector<wstring>& files, const wstring& current_directory, int cursor_position, wstring& marked_item, bool& is_cut) {
    if (!files.empty()) {
        wstring selected_item = current_directory + L"\\" + files[cursor_position];
        if (marked_item == selected_item && !is_cut) {
            marked_item.clear();
        }
        else {
            marked_item = selected_item;
            is_cut = false;
        }
       
        system("cls");
    }
}

void handleF3Key(vector<wstring>& files, const wstring& current_directory, int cursor_position, wstring& marked_item, bool& is_cut) {
    if (!files.empty()) {
        wstring selected_item = current_directory + L"\\" + files[cursor_position];
        if (marked_item == selected_item && is_cut) {
            marked_item.clear();
        }
        else {
            marked_item = selected_item;
            is_cut = true;
        }
        
        system("cls");
    }
}

void handleF4Key(vector<wstring>& files, const wstring& current_directory, wstring& marked_item, bool is_cut, int& cursor_position) {
    if (!marked_item.empty()) {
        wstring destination = current_directory + L"\\" + marked_item.substr(marked_item.find_last_of(L"\\") + 1);
        if (is_cut) {
            if (!moveItem(marked_item, destination)) {
                wcerr << L"Failed to move item: " << marked_item << L" to " << destination << endl;
            }
        }
        else {
            if (!copyItem(marked_item, destination)) {
                wcerr << L"Failed to copy item: " << marked_item << L" to " << destination << endl;
            }
        }
        files = getDirectoryContents(current_directory);
        cursor_position = 0;
        marked_item.clear();
    }
}