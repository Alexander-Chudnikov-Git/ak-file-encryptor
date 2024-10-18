/**
 * @file       <crypto_provider.cpp>
 * @brief      Основной файл криптографической части ak-file-encryptor.
 *
 *             Содержит в себе несколько оберточных функций для более простой работы с libakrypt.
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
#include "crypto_provider.hpp"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <libakrypt.h>
#include <sstream>
#include <iomanip>

namespace fs = std::filesystem;

/**
 * @brief Генерирует ключ на основе пароля и соли.
 *
 * Эта функция инициализирует библиотеку libakrypt и создает ключ,
 * используя указанный алгоритм (kuznechik или magma). Затем она
 * устанавливает ключ на основе предоставленного пароля и соли.
 *
 * @param password Пароль, используемый для генерации ключа.
 * @param salt Соль, используемая для генерации ключа.
 * @param key Указатель на структуру bckey, где будет храниться ключ.
 * @param algorithm Алгоритм для генерации ключа (kuznechik или magma).
 * @return int Код ошибки (EXIT_SUCCESS при успехе, EXIT_FAILURE при ошибке).
 */
int CryptoProvider::generate_key_from_password(const std::string &password,
                                               const std::string &salt,
                                               struct bckey *key,
                                               const std::string &algorithm)
{
    if (ak_libakrypt_create(ak_function_log_syslog) != ak_true)
    {
        std::cerr << "Ошибка инициализации libakrypt" << std::endl;
        return EXIT_FAILURE;
    }

    int error_code = ak_error_ok;

    if (algorithm == "kuznechik")
    {
        error_code = ak_bckey_create_kuznechik(key);
    }
    else if (algorithm == "magma")
    {
        error_code = ak_bckey_create_magma(key);
    }
    else
    {
        std::cerr << "Неподдерживаемый алгоритм: " << algorithm << std::endl;
        return EXIT_FAILURE;
    }

    if (error_code != ak_error_ok)
    {
        std::cerr << "Ошибка во время генерации ключа: " << error_code << std::endl;
        return EXIT_FAILURE;
    }

    error_code = ak_bckey_set_key_from_password(
        key,
        static_cast<void*>(const_cast<char*>(password.data())),
        password.size(),
        static_cast<void*>(const_cast<char*>(salt.data())),
        salt.size()
    );

    if (error_code != ak_error_ok)
    {
        std::cerr << "Ошибка установки ключа из пароля: " << error_code << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/**
 * @brief Генерирует случайную строку заданной длины.
 *
 * Эта функция создает строку длиной length, заполняя ее
 * случайными символами из заданного набора символов.
 *
 * @param length Длина генерируемой строки.
 * @param output Указатель на буфер, куда будет записана строка.
 */
void CryptoProvider::generate_random_string(size_t length, char *output)
{
    struct random generator;
    ak_uint8 buffer[length];
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    size_t charset_size = sizeof(charset) - 1;

    ak_random_create_lcg(&generator);
    memset(buffer, 0, sizeof(buffer));

    ak_random_ptr(&generator, buffer, length);

    for (size_t i = 0; i < length; ++i)
    {
        output[i] = charset[buffer[i] % charset_size];
    }

    output[length] = '\0';
    ak_random_destroy(&generator);
}

/**
 * @brief Шифрует текст с использованием указанного ключа.
 *
 * Эта функция выполняет шифрование открытого текста с помощью
 * алгоритма OFB и заданного ключа, возвращая зашифрованный текст.
 *
 * @param plain_text Открытый текст для шифрования.
 * @param key Указатель на структуру bckey, содержащую ключ.
 * @return std::string Зашифрованный текст.
 * @throws std::runtime_error Если шифрование не удалось.
 */
std::string CryptoProvider::encrypt(const std::string& plain_text, struct bckey *key)
{
    ak_uint8 iv[8] = { 0x01, 0x02, 0x03, 0x04, 0x11, 0xaa, 0x4e, 0x12 };
    std::string cipher_text = plain_text;

    int error = ak_bckey_ofb(key,
                             static_cast<void*>(const_cast<char*>(plain_text.data())),
                             static_cast<void*>(cipher_text.data()),
                             plain_text.size(),
                             iv,
                             sizeof(iv));

    if (error != ak_error_ok)
    {
        throw std::runtime_error("Шифрование не удалось");
    }

    return cipher_text;
}

/**
 * @brief Дешифрует текст с использованием указанного ключа.
 *
 * Эта функция выполняет дешифрование зашифрованного текста,
 * используя тот же ключ, что и для шифрования.
 *
 * @param cipher_text Зашифрованный текст для дешифрования.
 * @param key Указатель на структуру bckey, содержащую ключ.
 * @return std::string Открытый текст.
 */
std::string CryptoProvider::decrypt(const std::string& cipher_text, struct bckey *key)
{
    return encrypt(cipher_text, key);
}

/**
 * @brief Шифрует массив байтов с использованием указанного ключа.
 *
 * Эта функция выполняет шифрование массива байтов, возвращая
 * зашифрованный массив.
 *
 * @param plain_text Указатель на открытый текст для шифрования.
 * @param size Размер массива.
 * @param key Указатель на структуру bckey, содержащую ключ.
 * @return ak_uint8* Указатель на зашифрованный массив байтов.
 * @throws std::runtime_error Если шифрование не удалось.
 */
ak_uint8* CryptoProvider::encrypt(ak_uint8* plain_text, size_t size, struct bckey *key)
{
    ak_uint8 iv[8] = { 0x01, 0x02, 0x03, 0x04, 0x11, 0xaa, 0x4e, 0x12 };
    ak_uint8* cipher_text = new ak_uint8[size];

    int error = ak_bckey_ofb(key, plain_text, cipher_text, size, iv, sizeof(iv));

    if (error != ak_error_ok)
    {
        delete[] cipher_text;
        throw std::runtime_error("Шифрование не удалось");
    }

    return cipher_text;
}

/**
 * @brief Дешифрует массив байтов с использованием указанного ключа.
 *
 * Эта функция выполняет дешифрование зашифрованного массива байтов.
 *
 * @param cipher_text Указатель на зашифрованный текст для дешифрования.
 * @param size Размер массива.
 * @param key Указатель на структуру bckey, содержащую ключ.
 * @return ak_uint8* Указатель на открытый текст.
 */
ak_uint8* CryptoProvider::decrypt(ak_uint8* cipher_text, size_t size, struct bckey *key)
{
    return encrypt(cipher_text, size, key);
}

/**
 * @brief Сохраняет данные в файл.
 *
 * Эта функция сохраняет указанный массив байтов в файл.
 * Если файл имеет расширение .akr, оно будет удалено. Если нет,
 * будет добавлено расширение .akr.
 *
 * @param data Указатель на данные для сохранения.
 * @param size Размер данных.
 * @param original_file Имя исходного файла.
 * @return bool true, если сохранение прошло успешно, иначе false.
 */
bool CryptoProvider::ak_save_to_file(const ak_uint8* data, size_t size, const std::string& original_file)
{
    std::filesystem::path original_path(original_file);

    if (original_path.extension() == ".akr")
    {
        original_path.replace_extension();
    }
    else
    {
        original_path += ".akr";
    }

    std::ofstream ofs(original_path.string(), std::ios::binary);
    if (!ofs)
    {
        std::cerr << "Не удалось открыть файл для записи: " << original_path << std::endl;
        return false;
    }

    ofs.write(reinterpret_cast<const char*>(data), size);
    if (!ofs)
    {
        std::cerr << "Не удалось записать данные в файл: " << original_path << std::endl;
        return false;
    }

    return true;
}

/**
 * @brief Преобразует ключ в строку в шестнадцатеричном формате.
 *
 * Эта функция принимает указатель на структуру ключа и возвращает
 * строку, представляющую ключ в шестнадцатеричном виде.
 *
 * @param key Указатель на структуру ключа, которую необходимо преобразовать.
 *
 * @return std::string Строка, представляющая ключ в шестнадцатеричном формате.
 *
 * @note Формат строки: каждая байт ключа представлен в виде двух
 * шестнадцатеричных символов, разделенных пробелами.
 */
std::string CryptoProvider::bckey_to_string(struct bckey *key)
{
    std::stringstream ss;

    for (size_t i = 0; i < sizeof(key->key); ++i)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(key->key.key[i]);

        if (i != sizeof(key->key) - 1)
        {
            ss << " ";
        }
    }

    return ss.str();
}
