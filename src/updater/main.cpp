#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <signal.h>
#endif

namespace fs = std::filesystem;

/**
 * Updater Utility
 * 
 * Usage: Updater <app_pid> <source_path> <dest_path> <restart_cmd>
 */
int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cerr << "Usage: Updater <app_pid> <source_path> <dest_path> <restart_cmd>" << std::endl;
        return 1;
    }

    int app_pid = std::stoi(argv[1]);
    fs::path source_path = argv[2];
    fs::path dest_path = argv[3];
    std::string restart_cmd = argv[4];

    std::cout << "Waiting for process " << app_pid << " to exit..." << std::endl;

    // 1. メインプロセスが終了するのを待つ
#ifdef _WIN32
    HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE, app_pid);
    if (hProcess) {
        WaitForSingleObject(hProcess, INFINITE);
        CloseHandle(hProcess);
    }
#else
    while (kill(app_pid, 0) == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
#endif

    std::this_thread::sleep_for(std::chrono::seconds(1));

    try {
        // 2. ファイルの置き換え
        std::cout << "Replacing " << dest_path << " with " << source_path << std::endl;
        
        // 元のファイルをリネームしてバックアップ（念のため）
        auto backup_path = dest_path;
        backup_path += ".bak";
        if (fs::exists(dest_path)) {
            if (fs::exists(backup_path)) fs::remove(backup_path);
            fs::rename(dest_path, backup_path);
        }

        fs::copy_file(source_path, dest_path, fs::copy_options::overwrite_existing);
        
#ifndef _WIN32
        // 実行権限の付与 (macOS/Linux)
        fs::permissions(dest_path, fs::perms::owner_all | fs::perms::group_read | fs::perms::others_read, fs::perm_options::replace);
#endif

        // 3. アプリの再起動
        std::cout << "Restarting app: " << restart_cmd << std::endl;
#ifdef _WIN32
        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        if (CreateProcessA(NULL, (char*)restart_cmd.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
#else
        system((restart_cmd + " &").c_str());
#endif

        // バックアップの削除
        if (fs::exists(backup_path)) fs::remove(backup_path);

    } catch (const std::exception& e) {
        std::cerr << "Error during update: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
