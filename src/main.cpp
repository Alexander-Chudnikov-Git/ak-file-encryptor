/**
 * @file       <main.cpp>
 * @brief      Основной файл проекта ak-file-encryptor.
 *
 *             По своей сути просто запускает графический интерфейс.
 *
 * @author     THE_CHOODICK
 * @date       18-10-2024
 * @version    0.0.1
 *
 * @warning    Этот проект предназначен только для ознокомительных целей, сам проект содежит огромное количество говнокода и багов.
 * @bug        Их просто много
 *
 * @copyright  Copyright 2024 chooisfox. All rights reserved.
 *
 *             (Not really)
 *
 * @license    This project is released under the GNUv3 Public License.
 */
#include "gui/main_menu.hpp"

/**
 * @brief Главная функция программы.
 *
 * Эта функция служит точкой входа в приложение. Она инициирует
 * отображение основного меню, позволяя пользователю взаимодействовать
 * с программой. После завершения работы меню функция завершает
 * выполнение и возвращает 0.
 *
 * @return int Код возврата программы. 0 указывает на успешное
 * завершение.
 */
int main()
{
    MainMenu::showMenu();

    return 0;
}
