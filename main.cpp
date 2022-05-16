#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <vector>

#define SIZE_OF_DISK_NAME 4
// "C:\" - це три символи, а також ще роздільник між назвами дисків, тобто +1 символ, разом 4

using namespace std;

const int GB = 1024 * 1024 * 1024;

struct DiskInfo {
    TCHAR name[SIZE_OF_DISK_NAME]; /*ім'я диску
 * Для багатобайтового набору символів: TCHAR означає char (простий символ 1 байт)
 * Для набору символів Unicode: TCHAR означає wchar (широкий символ 2 байта)*/
    DWORD systemFlag;         //прапор системи
    DWORD serialNumber;        //серійний номер
    TCHAR fileSystem[20];      //тип файлової системи
    string driveType;          //тип диску
    __int64 totalSpace;        /*всього пам'яті
 * Тип __int64, наприклад, завжди має розмір 64 біти як на 32-розрядних, так і на 64-розрядних платформах.
 * Типи size_t і ptrdiff_t є 32-розрядними на 32-розрядній платформі і 64-бітними на 64-розрядній платформі.
 * Саме цей момент викликає неприємності та плутанину під час друку значень цих типів.*/
    __int64 freeSpace;         //вільно пам'яті

    void print() {
        cout << "Disk name :\t" << name << endl;
        cout << "System flag :\t" << systemFlag << endl;
        cout << "Serial number :\t" << serialNumber << endl;
        cout << "File system :\t" << fileSystem << endl;
        cout << "Driver type :\t" << driveType << endl;
        cout << "Total space :\t" << totalSpace / GB << " GB" << endl;
        cout << "Free space :\t" << freeSpace / GB << " GB" << endl;
    }
};

vector<DiskInfo> getDisksInfo();

int main() {
    vector list = getDisksInfo();
    for (auto &disk: list) {
        disk.print();
        cout << endl;
    }
}

string getDriverType(TCHAR *name) {
    UINT driveType = GetDriveTypeA(name);
    //Визначає, чи є дисковід знімним, фіксованим, CD-ROM, диском RAM або мережевим диском.
    string result = "error";
    switch (driveType) {
        case DRIVE_UNKNOWN:
            result = "DRIVE_UNKNOWN";
            break;
        case DRIVE_NO_ROOT_DIR:
            result = "DRIVE_NO_ROOT_DIR";
            break;
        case DRIVE_REMOVABLE:
            result = "DRIVE_REMOVABLE";
            break;
        case DRIVE_FIXED:
            result = "DRIVE_FIXED";
            break;
        case DRIVE_REMOTE:
            result = "DRIVE_REMOTE";
            break;
        case DRIVE_CDROM:
            result = "DRIVE_CDROM";
            break;
        case DRIVE_RAMDISK:
            result = "DRIVE_RAMDISK";
            break;
    }
    return result;
}

vector<DiskInfo> getDisksInfo() {
    auto *namesArr = new TCHAR();
    GetLogicalDriveStrings(MAXINTATOM, namesArr); // отримуємо масив назв дисків
    /*Функція GetLogicalDriveStrings отримує рядок, що закінчується нулем, для кожного наявного диску.
     * Використовувати ці рядки, коли потрібен кореневий каталог.*/
    int numberOfDisks = sizeof(namesArr) / SIZE_OF_DISK_NAME;//кількість дисків
    vector<DiskInfo> disks;
    for (int i = 0; i < numberOfDisks; ++i) {
        DiskInfo disk;
        _tcscpy_s(disk.name, SIZE_OF_DISK_NAME, namesArr); //Записуємо ім'я диску
        GetVolumeInformationA(disk.name, NULL, 0, &disk.serialNumber, 0, &disk.systemFlag,
                              disk.fileSystem,
                              sizeof(disk.fileSystem));/*Отримує інформацію про файлову систему та диск,
         * пов’язану із зазначеним кореневим каталогом.*/
        disk.driveType = getDriverType(disk.name);//записуємо тип диску
        GetDiskFreeSpaceEx(disk.name, (PULARGE_INTEGER) &disk.freeSpace,
                           (PULARGE_INTEGER) &disk.totalSpace,
                           NULL); //отримуємо інформацію про використання пам'яті
        disks.push_back(disk);
        namesArr += sizeof(TCHAR) * SIZE_OF_DISK_NAME;
        //переміщуємо вказівник масиву на розмір одного ім'я диску
    }
    return disks;
}

