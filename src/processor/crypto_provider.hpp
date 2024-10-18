/**
 * @file       <crypto_provider.hpp>
 * @brief      Хэдер криптографической части ak-file-encryptor.
 *
 *             Содержит в себе обяявления несколько оберточных функций,
 *             предназначенных для более простой работы с libakrypt.
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
#ifndef CRYPO_PROVIDER_HPP
#define CRYPO_PROVIDER_HPP

#include <string>
#include <stddef.h>

#define SALT_SIZE 16
#define BLOCK_SIZE 16
#define ITERATIONS 10000
#define IV { 0x01, 0x02, 0x03, 0x04, 0x11, 0xaa, 0x4e, 0x12 }
#define IV_SIZE 8

typedef unsigned char ak_uint8;

class CryptoProvider
{
public:
    static int generate_key_from_password(const std::string &password, const std::string &salt, struct bckey *key, const std::string &algorithm = "magma");
    static void generate_random_string(size_t length, char *output);
    static std::string encrypt(const std::string& plain_text, struct bckey *key);
    static std::string decrypt(const std::string& cipher_text, struct bckey *key);
    static ak_uint8* encrypt(ak_uint8* plain_text, size_t size, struct bckey *key);
    static ak_uint8* decrypt(ak_uint8* cipher_text, size_t size, struct bckey *key);
    static bool ak_save_to_file(const ak_uint8* data, size_t size, const std::string& original_file);
};

#endif // CRYPO_PROVIDER_HPP

