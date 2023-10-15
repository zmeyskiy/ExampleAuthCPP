#include <iostream>
#include <Windows.h>
#include <string>

#include "security.h"
#include "sqlite3.h";

using namespace std;

string USERNAME;
string PASSWORD;
string HWID;

void validated()
{
    int action;

    cout << "\nДанные: " << endl << endl;

    cout << "Ваш юзернейм: " << USERNAME << endl;
    cout << "Ваш пароль: " << PASSWORD << endl;
    cout << "Ваш HWID: " << HWID << endl;

    cout << "\nДействия: " << endl << endl;

    cout << "[1] Выйти с аккаунта" << endl << endl;

    cout << USERNAME << "@user: ";
    cin >> action;

    switch (action)
    {
    case 1:
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);

        system("cls");
        system(buffer);
    }
}

int main()
{
    SetConsoleTitleW(L"ExampleAuth");
	setlocale(LC_ALL, "Russian");

	cout << "Подключение к базе данных...\n";

    sqlite3* db;
    char* errMsg = 0;
    int rc;

    rc = sqlite3_open("C:\\Users\\zmeyskiy\\Desktop\\logins.db", &db);

    if (rc) {
        cout << "Ошибка: Не удалось подключится к базе данных" << endl;
        sqlite3_close(db);

        Sleep(750);

        exit(0);
    }

    sqlite3_stmt* stmt;

	Sleep(1000);

	system("cls");
	cout << "Привет, ты в приложении ExampleAuth\nЗарегистрируйся, либо, ввойди в свой аккаунт\n\n[1] Зарегистрироватся\n[2] Ввойти\n\nunknown@user: ";

choose:

	int action;
	cin >> action;

    if (action == 1)
    {
        string username;
        string password;

    enterName:

        cout << "\nВведите никнейм: ";
        cin >> username;

        const char* query = "SELECT COUNT(*) FROM data WHERE username = ?;";
        rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

        if (rc == SQLITE_OK) 
        {
            sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

            if (sqlite3_step(stmt) == SQLITE_ROW) 
            {
                int count = sqlite3_column_int(stmt, 0);
                if (count > 0) {
                    cout << "Ошибка: Такой юзернейм уже существует" << endl;
                    goto enterName;
                }
            }
            else 
            {
                cout << "Ошибка: Не удалось проверить существование юзернейма" << endl;
                goto choose;
            }

            sqlite3_finalize(stmt);
        }
        else 
        {
            cout << "Ошибка: Не удалось подготовить запрос" << endl;
        }

        cout << "Введите пароль: ";
        cin >> password;

        string passwordEncrypted = encrypt(password, "ajhciASYdfhaifyIAxhjfciaUYHCiASUFJIhujfoihaLIFYHocAFLDIYwidqefUHAESIFHGkhjfiAjflolFHJOajfsohFIashjfihFIKL");

        string sql = "INSERT INTO data (username, password, hwid, hwidEnabled) VALUES ('" + username + "', '" + passwordEncrypted + "', '" + getHWID() + "', '1'); ";
        rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);

        if (rc != SQLITE_OK) 
        {
            cerr << "Ошибка: Не удалось зарегистрировать аккаунт" << endl;
        }
        else 
        {
            USERNAME = username;
            PASSWORD = password;
            HWID = getHWID();

            validated();
        }
    }
    else if (action == 2)
    {
        bool userValid = false;

        string usernameToCheck;
        string passwordNotToCheck;
        string hwidToCheck = getHWID();

        cout << "Введите ваш никнейм: ";
        cin >> usernameToCheck;

        cout << "Введите ваш пароль: ";
        cin >> passwordNotToCheck;

        string passwordToCheck = encrypt(passwordNotToCheck, "ajhciASYdfhaifyIAxhjfciaUYHCiASUFJIhujfoihaLIFYHocAFLDIYwidqefUHAESIFHGkhjfiAjflolFHJOajfsohFIashjfihFIKL");

        const char* sql = "SELECT username, password, hwid FROM data";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

        if (rc != SQLITE_OK) 
        {
            cout << "Ошибка: Не удалось подготовить запрос" << endl;
            return(0);
        }

        while (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            const char* username = (const char*) sqlite3_column_text(stmt, 0);
            const char* password = (const char*) sqlite3_column_text(stmt, 1);
            const char* hwid = (const char*) sqlite3_column_text(stmt, 2);

            if (strcmp(username, usernameToCheck.c_str()) == 0 && strcmp(password, passwordToCheck.c_str()) == 0 && strcmp(hwid, hwidToCheck.c_str()) == 0) 
            {
                userValid = true;
                break;
            }
        }

        if (userValid)
        {
            USERNAME = usernameToCheck;
            PASSWORD = passwordNotToCheck;
            HWID = getHWID();

            validated();
        }
        else
        {
            cout << "Ошибка: Не удалось войти. \nВозможные причины: неправильный юзернейм/пароль, ваш HWID не совпадает с тем, что в базе данных" << endl;
        }
    }
}