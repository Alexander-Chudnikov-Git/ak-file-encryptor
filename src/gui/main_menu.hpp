/**
 * @file       <main_menu.hpp>
 * @brief      Хэдер графической части ak-file-encryptor.
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
#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include <set>
#include <string>

class MainMenu
{
public:
    enum OptionsSelected
    {
        NONE = 0,
        RETURN,
        EXIT,
        CONTINUE,
        PICK_ANOTHER,

        OPT_FILE,
        OPT_STRING,
        OPT_ROOT,

        TYPE_ENCRYPT,
        TYPE_DECRYPT
    };

public:
    static MainMenu::OptionsSelected showMenu();

private:
    static void handleInterrupt(int signal = 0);

    static void initializeNCR();
    static MainMenu::OptionsSelected cleanupNCR(MainMenu::OptionsSelected option = NONE);
    static MainMenu::OptionsSelected getUserInput(const std::set<char>& validInputs = {});

    static std::string getInputString(int line, unsigned int max_length = 64);
    static bool getYesNoInput(int line, const std::string& question);
    static std::string getInputWithFileValidation(const std::string& prompt);

private:
    static struct termios orig_termios;
};

#endif // MAIN_MENU_HPP
