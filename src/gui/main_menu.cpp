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

#include <libakrypt.h>

namespace fs = std::filesystem;

struct termios MainMenu::orig_termios;

/**
 * @brief Отображает главное меню и обрабатывает выбор пользователя.
 *
 * @return MainMenu::OptionsSelected Опция, выбранная пользователем.
 */
MainMenu::OptionsSelected MainMenu::showMenu()
{
    initializeNCR(); //< Инициализация интерфейса ncurses
    clear();

    mvprintw(2, 10, "Select what do you want to process?"); clrtoeol();
    mvprintw(3, 10, "-----------------------------------"); clrtoeol();

    target_choice_label:

    ///< Меню выбора цели операции
    mvprintw(4, 12, "1 - File"); clrtoeol();
    mvprintw(5, 12, "2 - String"); clrtoeol();
    mvprintw(6, 12, "3 - Root folder"); clrtoeol();

    // Очищаем остальные строки
    for (int i = 7; i <= 11; ++i)
    {
        mvprintw(i, 12, " "); clrtoeol();
    }

    const auto user_target_choice = getUserInput({'1', '2', '3', 'q'});

    std::string target_type;

    ///< Обработка выбора цели операции
    switch (user_target_choice)
    {
        case OptionsSelected::EXIT:
            return cleanupNCR(OptionsSelected::EXIT);
        case OptionsSelected::OPT_FILE:
            target_type = "file";
            break;
        case OptionsSelected::OPT_STRING:
            target_type = "string";
            break;
        case OptionsSelected::OPT_ROOT:
            target_type = "root";
            break;
        default:
            mvprintw(11, 12, "Invalid choice provided, exiting..."); clrtoeol();
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return cleanupNCR(OptionsSelected::EXIT);
    }

    mvprintw(2, 10, "Do you want to encrypt or decrypt?"); clrtoeol();
    mvprintw(4, 12, "e - Encrypt %s", target_type.c_str()); clrtoeol();
    mvprintw(5, 12, "d - Decrypt %s", target_type.c_str()); clrtoeol();
    mvprintw(6, 12, "r - Return"); clrtoeol();

    const auto user_operation_choice = getUserInput({'e', 'd', 'r', 'q'});

    ///< Меню выбора типа операции
    switch (user_operation_choice)
    {
        case OptionsSelected::EXIT:
            return cleanupNCR(OptionsSelected::EXIT);
        case OptionsSelected::TYPE_ENCRYPT:
            target_type = "encrypt";
            break;
        case OptionsSelected::TYPE_DECRYPT:
            target_type = "decrypt";
            break;
        case OptionsSelected::RETURN:
            goto target_choice_label;
        default:
            mvprintw(11, 12, "Invalid choice provided, exiting...");
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return cleanupNCR(OptionsSelected::EXIT);
    }

    ///< Обработка строки для шифрования
    if (user_target_choice == OptionsSelected::OPT_STRING)
    {
        mvprintw(6, 12, " "); clrtoeol();
        mvprintw(7, 12, " "); clrtoeol();
        mvprintw(8, 12, " "); clrtoeol();
        mvprintw(2, 10, "Enter string to %s", target_type.c_str()); clrtoeol();
        mvprintw(4, 12, "String to encrypt:"); clrtoeol();
        mvprintw(5, 12, "(max %d characters):", 32); clrtoeol();
        const auto string_to_encrypt = getInputString(5, 32); clrtoeol();

        mvprintw(2, 10, "Enter %sion key", target_type.c_str()); clrtoeol();

        bool generate_key = (user_operation_choice == OptionsSelected::TYPE_ENCRYPT)
                            ? getYesNoInput(6, "Generate key automatically?")
                            : false;

        mvprintw(6, 12, " "); clrtoeol();
        mvprintw(7, 12, " "); clrtoeol();
        mvprintw(11, 12, " "); clrtoeol();

        struct bckey key;
        size_t password_length = 32;
        char password[password_length + 1] = {0}; // Массив для хранения пароля (32 символа + \0)

        if (generate_key)
        {
            mvprintw(6, 12, "Your password:");
            CryptoProvider::generate_random_string(32, password);
            CryptoProvider::generate_key_from_password(password, "", &key);
            mvprintw(7, 12, "(max %d characters): %s", 32, password);
        }
        else
        {
            mvprintw(6, 12, "Your password:");
            mvprintw(7, 12, "(max %d characters):", 32);
            const auto input_string = getInputString(7, 32);
            std::strncpy(password, input_string.c_str(), password_length);
            CryptoProvider::generate_key_from_password(password, "", &key);
            mvprintw(7, 12, "(max %d characters): %s", 32, password);
        }

        std::string result = CryptoProvider::encrypt(string_to_encrypt, &key);
        std::string status = (string_to_encrypt == CryptoProvider::decrypt(result, &key)) ? "Match" : "Non Match";

        mvprintw(8, 12, "Result:"); clrtoeol();
        mvprintw(9, 12, "(%s): %s", status.c_str(), result.c_str());


    }
    else if (user_target_choice == OptionsSelected::OPT_FILE)
    {
        pick_another_label:

        std::string input_file = getInputWithFileValidation("Enter file path: ");

        fs::path path(input_file);

        std::string file_name = path.filename().string();

        clear();

        mvprintw(2, 10, "What do you want to do with file?");
        mvprintw(3, 10, "-----------------------------------");
        mvprintw(4, 12, "c - Continue");
        mvprintw(5, 12, "p - Pick another file");
        mvprintw(6, 12, "r - Return");
        mvprintw(9, 12, "You selected: %s", file_name.c_str());

        const auto user_operation_choice = getUserInput({'c', 'p', 'r', 'q'});

        ///< Меню выбора типа операции
        switch (user_operation_choice)
        {
            case OptionsSelected::EXIT:
                return cleanupNCR(OptionsSelected::EXIT);
            case OptionsSelected::CONTINUE:
                break;
            case OptionsSelected::PICK_ANOTHER:
                goto pick_another_label;
            case OptionsSelected::RETURN:
                goto target_choice_label;
            default:
                mvprintw(11, 12, "Invalid choice provided, exiting...");
                std::this_thread::sleep_for(std::chrono::seconds(2));
                return cleanupNCR(OptionsSelected::EXIT);
        }

        for (int i = 4; i <= 9; ++i)
        {
            mvprintw(i, 12, " "); clrtoeol();
        }

        struct bckey key;
        size_t password_length = 32;
        char password[password_length + 1] = {0};

        mvprintw(4, 12, "You selected:");
        mvprintw(4, 12, "%s", file_name.c_str());
        mvprintw(6, 12, "Your password:");
        mvprintw(7, 12, "(max %d characters):", 32);
        const auto input_string = getInputString(7, 32);
        std::strncpy(password, input_string.c_str(), password_length);
        CryptoProvider::generate_key_from_password(password, "", &key);
        mvprintw(7, 12, "(max %d characters): %s", 32, password);

        size_t size = 0;
        ak_uint8 *buffer = nullptr;

        buffer = ak_ptr_load_from_file(buffer, &size, input_file.c_str());

        if (buffer == nullptr || size == 0)
        {
            mvprintw(9, 12, "Failed to load file or file is empty.");
            return cleanupNCR();
        }

        ak_uint8 *encrypted_buffer = CryptoProvider::encrypt(buffer, size, &key);

        if (encrypted_buffer == nullptr)
        {
            mvprintw(9, 12, "Encryption failed.");
            delete[] buffer;
            return cleanupNCR();
        }

        ak_uint8 *decrypted_buffer = CryptoProvider::decrypt(encrypted_buffer, size, &key);

        if (decrypted_buffer == nullptr)
        {
            mvprintw(9, 12, "Decryption failed.");
            delete[] encrypted_buffer;
            delete[] buffer;
            return cleanupNCR();
        }

        CryptoProvider::ak_save_to_file(encrypted_buffer, size, input_file);

        std::string status = (std::memcmp(buffer, decrypted_buffer, size) == 0) ? "Match" : "Non Match";

        mvprintw(8, 12, "Result:"); clrtoeol();
        mvprintw(9, 12, "(%s)", status.c_str());

        delete[] encrypted_buffer;
        delete[] decrypted_buffer;
        delete[] buffer;
    }
    else
    {
        clear();
        mvprintw(2, 10, "Wrong file system");
        mvprintw(3, 10, "-----------------------------------");
        mvprintw(4, 10, "In order to recursively encrypt your whole system");
        mvprintw(5, 10, "You have to be Ubuntu user");
    }

    if (!getYesNoInput(10, "Exit?"))
    {
        goto target_choice_label;
    }

    return cleanupNCR(user_target_choice);
}

/**
 * @brief Интерфейс получения строки от пользователя.
 *
 * @param line Строка в интерфейсе, где выводится ввод.
 * @param max_length Максимальная длина строки.
 * @return std::string Введенная строка.
 */
std::string MainMenu::getInputString(int line, unsigned int max_length)
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
        mvprintw(line, 12, "(max %d characters): %s", max_length, input.c_str());
        clrtoeol();
        refresh();
    }
    return input;
}

/**
 * @brief Запрашивает у пользователя ввод "Да" или "Нет".
 *
 * @param line Строка для вывода вопроса.
 * @param question Вопрос для пользователя.
 * @return true Если пользователь выбрал "Да".
 * @return false Если пользователь выбрал "Нет".
 */
bool MainMenu::getYesNoInput(int line, const std::string& question)
{
    int user_input;
    bool valid_input = false;

    mvprintw(line, 12, "%s (y/n): ", question.c_str());
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
                mvprintw(11, 12, "Invalid input! Please enter 'y' or 'n': ");
                refresh();
            }
        }
    }
    return (user_input == 'y' || user_input == 'Y');
}

/**
 * @brief Обработчик прерываний SIGINT и SIGTERM.
 *
 * @param signal Номер сигнала.
 */
void MainMenu::handleInterrupt(int signal)
{
    cleanupNCR();
    exit(signal);
}

/**
 * @brief Инициализирует интерфейс ncurses и настраивает терминал.
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
 * @brief Восстанавливает терминал в исходное состояние и завершает режим ncurses.
 *
 * @param option Опция для возврата.
 * @return MainMenu::OptionsSelected Возвращаемая опция.
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
                case 'q': return OptionsSelected::EXIT;
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
 * @brief Получает строковый ввод от пользователя с проверкой на существующий файл.
 *
 * Функция позволяет пользователю вводить строку, при нажатии на Tab проверяется текущий введенный путь.
 * Если это каталог, отображается его содержимое. Пользователь не может нажать Enter до тех пор,
 * пока не введет путь до существующего файла.
 *
 * @param prompt Сообщение, отображаемое пользователю перед вводом.
 * @return std::string Строка, введенная пользователем.
 */
std::string MainMenu::getInputWithFileValidation(const std::string& prompt)
{
    std::string user_input = fs::current_path();
    int ch;

    clear();
    mvprintw(2, 10, "Please select file");
    mvprintw(3, 10, "-----------------------------------");
    mvprintw(4, 12, "%s%s", prompt.c_str(), user_input.c_str());

    while (true)
    {
        ch = getch();

        if (ch == 127 || ch == KEY_BACKSPACE)
        {
            if (!user_input.empty())
            {
                user_input.pop_back();
                goto draw_file_manager;
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

            draw_file_manager:
            clear();
            mvprintw(2, 10, "Please select file");
            mvprintw(3, 10, "-----------------------------------");

            if (fs::exists(user_input) && fs::is_directory(user_input))
            {
                mvprintw(5, 12, "Content of directory: %s", user_input.c_str());

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
                    last_dir = user_input.substr(0, last_slash_pos + 1); //< Получаем путь до последнего '/'

                    if (fs::exists(last_dir) && fs::is_directory(last_dir))
                    {
                        //< Если каталог существует, получаем часть после последнего '/'
                        std::string filter = user_input.substr(last_slash_pos + 1);
                        mvprintw(5, 12, "Content of directory: %s", last_dir.c_str());

                        int line = 6;
                        for (const auto& entry : fs::directory_iterator(last_dir))
                        {
                            std::string filename = entry.path().filename().string();
                            //< Фильтруем по части после последнего '/'
                            if (filename.find(filter) != std::string::npos)
                            {
                                mvprintw(line++, 12, "%s", filename.c_str());
                            }
                        }
                    }
                }
            }
            mvprintw(4, 12, "%s%s", prompt.c_str(), user_input.c_str()); clrtoeol();
        }
    }
}
