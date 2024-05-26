#include "FileManager.h"

int main() {
    _setmode(_fileno(stdout), _O_U16TEXT);

    wstring initial_directory = L"C:\\";
    wstring current_directory = initial_directory;
    vector<wstring> files = getDirectoryContents(current_directory);
    int cursor_position = 0;
    bool view_mode = false;
    wstring file_content;

    wstring marked_item;
    bool is_cut = false;

    while (true) {
        system("cls");

        if (view_mode) {
            wcout << file_content << endl;
            wcout << L"\nPress any key to return..." << endl;
            _getch();
            view_mode = false;
            continue;
        }

        displayFiles(current_directory, files, cursor_position, marked_item, is_cut);

        int key = _getch();
        if (key == 224) {
            key = _getch();
            if (key == 72 && cursor_position > 0) {
                cursor_position--;
            }
            else if (key == 80 && cursor_position < static_cast<int>(files.size()) - 1) {
                cursor_position++;
            }
        }
        else if (key == 13) { // Enter
            handleEnterKey(current_directory, files, cursor_position);
        }
        else if (key == 8) { // Backspace для перемещения выше по директории
            handleBackspaceKey(current_directory, files, cursor_position);
        }
        else if (key == 32) { // Space для удаления
            handleDeleteKey(files, current_directory, cursor_position);
        }
        else if (key == 59) { // F1 для чтения содержимого
            handleF1Key(files, current_directory, cursor_position, view_mode, file_content);
        }
        else if (key == 60) { // F2 для отметки
            handleF2Key(files, current_directory, cursor_position, marked_item, is_cut);
        }
        else if (key == 61) { // F3 для вырезания
            handleF3Key(files, current_directory, cursor_position, marked_item, is_cut);
        }
        else if (key == 62) { // F4 для вставки
            handleF4Key(files, current_directory, marked_item, is_cut, cursor_position);
        }
    }

    return 0;
}