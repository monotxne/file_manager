#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <iostream>
#include <windows.h>
#include <conio.h>
#include <vector>
#include <string>
#include <fstream>
#include <fcntl.h> 
#include <io.h> 
#include <locale>
#include <codecvt>
#include <shlwapi.h> 

#pragma comment(lib, "Shlwapi.lib")

using namespace std;

wstring readFileContents(const wstring& filePath);
vector<wstring> getDirectoryContents(const wstring& path);
bool copyItem(const wstring& source, const wstring& destination);
bool moveItem(const wstring& source, const wstring& destination);
void setConsoleColor(WORD color);
void displayFiles(const wstring& current_directory, const vector<wstring>& files, int cursor_position, const wstring& marked_item, bool is_cut);
void handleEnterKey(wstring& current_directory, vector<wstring>& files, int& cursor_position);
void handleBackspaceKey(wstring& current_directory, vector<wstring>& files, int& cursor_position);
void handleDeleteKey(vector<wstring>& files, const wstring& current_directory, int& cursor_position);
void handleF1Key(const vector<wstring>& files, const wstring& current_directory, int cursor_position, bool& view_mode, wstring& file_content);
void handleF2Key(vector<wstring>& files, const wstring& current_directory, int cursor_position, wstring& marked_item, bool& is_cut);
void handleF3Key(vector<wstring>& files, const wstring& current_directory, int cursor_position, wstring& marked_item, bool& is_cut);
void handleF4Key(vector<wstring>& files, const wstring& current_directory, wstring& marked_item, bool is_cut, int& cursor_position);

#endif