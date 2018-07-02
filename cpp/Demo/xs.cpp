#include <mbstring.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <unordered_map>

#include <zlib.h>
#include <tpl.h>

static uint8_t s_table1[256] = { 0 };
static uint8_t s_dec_table1[256] = { 0 };
static uint8_t s_table3[256] = { 0 };
static uint8_t s_index2[256] = { 0 };
static uint8_t s_index4[256] = { 0 };
static uint8_t s_index8[256] = { 0 };
static const char* s_confuse_str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,./;\"'<>?";
static const int s_confuse_str_len = strlen(s_confuse_str);
static const char* s_key1 = "93e6dc0011a725a8025207efff74c3d3";
static const char* s_key2 = "c2ba218a99bc034bdff2e4126a974b8c";
static const char* s_v3_key1 = "16194ae0f6ad88da4675e1b130b4d162";
static const char* s_v3_key2 = "c2ba218a99bc034bdff2e4126a974b8c";
static uint8_t s_key5[] = { 0x22, 0x10, 0x19, 0x64, 0x10, 0x19, 0x64, 0x22, 0x19, 0x64, 0x22, 0x10, 0x64, 0x22, 0x10, 0x19, 0x00 };
static uint8_t s_v3_key5[] = { 0x64, 0x22, 0x19, 0x22, 0x10, 0x19, 0x64, 0x22, 0x10, 0x64, 0x10, 0x19, 0x64, 0x22, 0x10, 0x19 };
static std::unordered_map<uint32_t, uint32_t> s_enc_map;

static void init_xs();
static void deinit_xs();
static uint8_t* xun_huan_zuo_yi(uint8_t *a1);
static void from_table1(uint8_t *a1);
static void gen_key6(uint8_t *key4, uint8_t *key6, int param_value8);
static void gen_key7_1(uint8_t *key7, uint8_t *key6, int a3);
static void gen_key7_2(uint8_t *key7);
static void revert_gen_key7_2(uint8_t *key7);
static void gen_key7_3(uint8_t *key7);
static void enc_line(uint8_t *buff, uint8_t *line, uint8_t *key6, int ll);
static void dec_line(uint8_t *buff, uint8_t *line, uint8_t *key6, int ll);
static int enc_xx(uint8_t *buf, int len, char *key4, int key4len, uint8_t *key5);
static int dec_xx(uint8_t *buf, int len, char *key4, int key4len, uint8_t *key5);
static void gen_confuse(char *confuse, int len);
static int enc_file(const char *src_path, const char *xpk_path, const char *null_path);
static int dec_file(const char *xpk);
static void enc_lua_obj(char* compiled_data_buf, int lua_obj_len, const char* key, int key_len);

static void zlib_compress_test()
{
    const char* s1 = "1";
    int len = strlen(s1);
    auto zip_len = compressBound(len);
    uint8_t* zip_buff = (uint8_t*)malloc(zip_len);
    compress(zip_buff, &zip_len, (uint8_t*)s1, len);
    printf("%02x%02x%02x%02x%02x%02x%02x%02x, zip_len=%d\n", zip_buff[0], zip_buff[1], zip_buff[2], zip_buff[3], zip_buff[4], zip_buff[5], zip_buff[6], zip_buff[7], zip_len);
    free(zip_buff);

    s1 = "12";
    len = strlen(s1);
    zip_len = compressBound(len);
    zip_buff = (uint8_t*)malloc(zip_len);
    compress(zip_buff, &zip_len, (uint8_t*)s1, len);
    printf("%02x%02x%02x%02x%02x%02x%02x%02x, zip_len=%d\n", zip_buff[0], zip_buff[1], zip_buff[2], zip_buff[3], zip_buff[4], zip_buff[5], zip_buff[6], zip_buff[7], zip_len);
    free(zip_buff);

    s1 = "123";
    len = strlen(s1);
    zip_len = compressBound(len);
    zip_buff = (uint8_t*)malloc(zip_len);
    compress(zip_buff, &zip_len, (uint8_t*)s1, len);
    printf("%02x%02x%02x%02x%02x%02x%02x%02x, zip_len=%d\n", zip_buff[0], zip_buff[1], zip_buff[2], zip_buff[3], zip_buff[4], zip_buff[5], zip_buff[6], zip_buff[7], zip_len);
    free(zip_buff);

    s1 = "1234";
    len = strlen(s1);
    zip_len = compressBound(len);
    zip_buff = (uint8_t*)malloc(zip_len);
    compress(zip_buff, &zip_len, (uint8_t*)s1, len);
    printf("%02x%02x%02x%02x%02x%02x%02x%02x, zip_len=%d\n", zip_buff[0], zip_buff[1], zip_buff[2], zip_buff[3], zip_buff[4], zip_buff[5], zip_buff[6], zip_buff[7], zip_len);
    free(zip_buff);

    s1 = "12345";
    len = strlen(s1);
    zip_len = compressBound(len);
    zip_buff = (uint8_t*)malloc(zip_len);
    compress(zip_buff, &zip_len, (uint8_t*)s1, len);
    printf("%02x%02x%02x%02x%02x%02x%02x%02x, zip_len=%d\n", zip_buff[0], zip_buff[1], zip_buff[2], zip_buff[3], zip_buff[4], zip_buff[5], zip_buff[6], zip_buff[7], zip_len);
    free(zip_buff);

    s1 = "123456";
    len = strlen(s1);
    zip_len = compressBound(len);
    zip_buff = (uint8_t*)malloc(zip_len);
    compress(zip_buff, &zip_len, (uint8_t*)s1, len);
    printf("%02x%02x%02x%02x%02x%02x%02x%02x, zip_len=%d\n", zip_buff[0], zip_buff[1], zip_buff[2], zip_buff[3], zip_buff[4], zip_buff[5], zip_buff[6], zip_buff[7], zip_len);
    free(zip_buff);

    s1 = "1234567";
    len = strlen(s1);
    zip_len = compressBound(len);
    zip_buff = (uint8_t*)malloc(zip_len);
    compress(zip_buff, &zip_len, (uint8_t*)s1, len);
    printf("%02x%02x%02x%02x%02x%02x%02x%02x, zip_len=%d\n", zip_buff[0], zip_buff[1], zip_buff[2], zip_buff[3], zip_buff[4], zip_buff[5], zip_buff[6], zip_buff[7], zip_len);
    free(zip_buff);

    s1 = "12345678";
    len = strlen(s1);
    zip_len = compressBound(len);
    zip_buff = (uint8_t*)malloc(zip_len);
    compress(zip_buff, &zip_len, (uint8_t*)s1, len);
    printf("%02x%02x%02x%02x%02x%02x%02x%02x, zip_len=%d\n", zip_buff[0], zip_buff[1], zip_buff[2], zip_buff[3], zip_buff[4], zip_buff[5], zip_buff[6], zip_buff[7], zip_len);
    free(zip_buff);

    s1 = "11";
    len = strlen(s1);
    zip_len = compressBound(len);
    zip_buff = (uint8_t*)malloc(zip_len);
    compress(zip_buff, &zip_len, (uint8_t*)s1, len);
    printf("%02x%02x%02x%02x%02x%02x%02x%02x, zip_len=%d\n", zip_buff[0], zip_buff[1], zip_buff[2], zip_buff[3], zip_buff[4], zip_buff[5], zip_buff[6], zip_buff[7], zip_len);
    free(zip_buff);

    s1 = "112";
    len = strlen(s1);
    zip_len = compressBound(len);
    zip_buff = (uint8_t*)malloc(zip_len);
    compress(zip_buff, &zip_len, (uint8_t*)s1, len);
    printf("%02x%02x%02x%02x%02x%02x%02x%02x, zip_len=%d\n", zip_buff[0], zip_buff[1], zip_buff[2], zip_buff[3], zip_buff[4], zip_buff[5], zip_buff[6], zip_buff[7], zip_len);
    free(zip_buff);
}

int main(int argc, char* argv[])
{
    zlib_compress_test();

    init_xs();

    enc_file(0, 0, 0);

    dec_file("w22");

    deinit_xs();
    return 0;
}

static void init_xs()
{
    FILE* fin = fopen("t1", "rb");
    fseek(fin, 0, SEEK_END);
    int len = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    fread(s_table1, 1, len, fin);
    fclose(fin);
    for (int i = 0; i < 256; ++i)
    {
        s_dec_table1[s_table1[(uint8_t)i]] = (uint8_t)i;
    }
    std::cout << "c" << (char)s_dec_table1[s_table1['c']] << " d" << (char)s_dec_table1[s_table1['d']]
        << " e" << (char)s_dec_table1[s_table1['e']] << " f" << (char)s_dec_table1[s_table1['f']] << std::endl;

    fin = fopen("t3", "rb");
    fseek(fin, 0, SEEK_END);
    len = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    fread(s_table3, 1, len, fin);
    fclose(fin);

    fin = fopen("i2", "rb");
    fseek(fin, 0, SEEK_END);
    len = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    fread(s_index2, 1, len, fin);
    fclose(fin);

    fin = fopen("i4", "rb");
    fseek(fin, 0, SEEK_END);
    len = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    fread(s_index4, 1, len, fin);
    fclose(fin);

    fin = fopen("i8", "rb");
    fseek(fin, 0, SEEK_END);
    len = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    fread(s_index8, 1, len, fin);
    fclose(fin);
}
static void deinit_xs()
{
}
static uint8_t * xun_huan_zuo_yi(uint8_t *a1)
{
    uint8_t v1; // ST0F_1

    v1 = *a1;
    *a1 = a1[1];
    a1[1] = a1[2];
    a1[2] = a1[3];
    a1[3] = v1;
    return a1;
}
static void from_table1(uint8_t *a1)
{
    a1[0] = s_table1[(uint8_t)a1[0]];
    a1[1] = s_table1[(uint8_t)a1[1]];
    a1[2] = s_table1[(uint8_t)a1[2]];
    a1[3] = s_table1[(uint8_t)a1[3]];
}
static void gen_key6(uint8_t *key4, uint8_t *key6, int param_value8)
{
    uint8_t v4[4]; // [esp+18h] [ebp-20h]
    int v5; // [esp+1Ch] [ebp-1Ch]
    int v6; // [esp+20h] [ebp-18h]
    int value_60; // [esp+24h] [ebp-14h]
    int v8; // [esp+28h] [ebp-10h]
    int value_8_59; // [esp+2Ch] [ebp-Ch]

    value_60 = 4 * (param_value8 + 7);
    value_8_59 = 0;
    v8 = 0;
    while (value_8_59 < param_value8)
    {
        memcpy(&key6[4 * value_8_59++], &key4[v8], 4u);
        v8 += 4;
    }
    while (value_8_59 < value_60)               // v9 [8, 59]
    {
        *(int *)v4 = *(int *)&key6[4 * (value_8_59 - 1)];
        if (value_8_59 % param_value8)
        {
            if (param_value8 > 6 && value_8_59 % param_value8 == 4)
                from_table1(v4);
        }
        else
        {
            xun_huan_zuo_yi(v4);
            from_table1(v4);
            v4[0] ^= s_table3[value_8_59 / param_value8];
        }
        v8 = value_8_59 - param_value8;
        v6 = 4 * value_8_59;
        v5 = 4 * (value_8_59 - param_value8);
        key6[4 * value_8_59] = key6[4 * (value_8_59 - param_value8)] ^ v4[0];
        key6[v6 + 1] = key6[v5 + 1] ^ v4[1];
        key6[v6 + 2] = key6[v5 + 2] ^ v4[2];
        key6[v6 + 3] = key6[v5 + 3] ^ v4[3];
        ++value_8_59;
    }
}

static void gen_key7_1(uint8_t *key7, uint8_t *key6, int a3)
{
    for (int i = 0; i < 16; ++i)
    {
        key7[i] ^= key6[16 * a3 + i];
    }
}
static void gen_key7_2(uint8_t *key7)
{
    uint8_t v1; // ST0F_1
    uint8_t v2; // ST0F_1
    uint8_t v3; // ST0E_1
    uint8_t v4; // ST0F_1

    key7[0] = s_table1[key7[0]];
    key7[4] = s_table1[key7[4]];
    key7[8] = s_table1[key7[8]];
    key7[12] = s_table1[key7[12]];
    v1 = s_table1[key7[1]];
    key7[1] = s_table1[key7[5]];
    key7[5] = s_table1[key7[9]];
    key7[9] = s_table1[key7[13]];
    key7[13] = v1;
    v2 = s_table1[key7[2]];
    v3 = s_table1[key7[6]];
    key7[2] = s_table1[key7[10]];
    key7[6] = s_table1[key7[14]];
    key7[10] = v2;
    key7[14] = v3;
    v4 = s_table1[key7[15]];
    key7[15] = s_table1[key7[11]];
    key7[11] = s_table1[key7[7]];
    key7[7] = s_table1[key7[3]];
    key7[3] = v4;
}
static void revert_gen_key7_2(uint8_t *key7)
{
    uint8_t v1; // ST0F_1
    uint8_t v2; // ST0F_1
    uint8_t v3; // ST0E_1
    uint8_t v4; // ST0F_1

    key7[0] = s_dec_table1[key7[0]];
    key7[4] = s_dec_table1[key7[4]];
    key7[8] = s_dec_table1[key7[8]];
    key7[12] = s_dec_table1[key7[12]];
    v1 = s_dec_table1[key7[1]];
    key7[1] = s_dec_table1[key7[13]];
    key7[13] = s_dec_table1[key7[9]];
    key7[9] = s_dec_table1[key7[5]];
    key7[5] = v1;
    v2 = s_dec_table1[key7[6]];
    v3 = s_dec_table1[key7[2]];
    key7[2] = s_dec_table1[key7[10]];
    key7[6] = s_dec_table1[key7[14]];
    key7[10] = v3;
    key7[14] = v2;
    v4 = s_dec_table1[key7[7]];
    key7[7] = s_dec_table1[key7[11]];
    key7[11] = s_dec_table1[key7[15]];
    key7[15] = s_dec_table1[key7[3]];
    key7[3] = v4;
}
static void gen_key7_3(uint8_t *key7)
{
    for (int i = 0; i <= 3; ++i)
    {
        uint8_t * v1 = &key7[4 * i];
        uint8_t b0 = v1[0];
        uint8_t b1 = v1[1];
        uint8_t b2 = v1[2];
        uint8_t b3 = v1[3];

        v1[0] = b1 ^ b2 ^ b3 ^ s_index2[b0 ^ b1];
        v1[1] = b0 ^ b2 ^ b3 ^ s_index2[b1 ^ b2];
        v1[2] = b0 ^ b1 ^ b3 ^ s_index2[b2 ^ b3];
        v1[3] = b0 ^ b1 ^ b2 ^ s_index2[b0 ^ b3];
    }
}
static void revert_gen_key7_3(uint8_t *key7)
{
    for (int i = 0; i <= 3; ++i)
    {
        uint8_t * v1 = &key7[4 * i];
        uint8_t b0 = v1[0];
        uint8_t b1 = v1[1];
        uint8_t b2 = v1[2];
        uint8_t b3 = v1[3];
        uint8_t v2 = b0 ^ b3;
        uint8_t v3 = b1 ^ b2;
        uint8_t v4 = s_index8[v3 ^ v2] ^ v3 ^ v2;
        uint8_t v5 = v4 ^ s_index4[b0 ^ b2];
        uint8_t v6 = s_index4[b1 ^ b3] ^ v4;
        v1[0] ^= v5 ^ s_index2[b0 ^ b1];
        v1[1] ^= v6 ^ s_index2[b1 ^ b2];
        v1[2] ^= v5 ^ s_index2[b2 ^ b3];
        v1[3] ^= v6 ^ s_index2[b0 ^ b3];
    }
}

static void enc_line(uint8_t *buff, uint8_t *line, uint8_t *key6, int ll)
{
    std::cout << "buff=0x" << (void*)buff << ", line=0x" << (void*)line << std::endl;
    uint8_t key7[16] = { 0 }; // [esp+Ch] [ebp-20h]
    int i; // [esp+24h] [ebp-8h]
    int k; // [esp+28h] [ebp-4h]

    for (i = 0; i < 16; ++i)
    {
        key7[i] = buff[i];
    }
    gen_key7_1(key7, key6, 0);

    for (k = 1; k < ll; ++k)                    // [1, 13]
    {
        gen_key7_2(key7);
        gen_key7_3(key7);
        gen_key7_1(key7, key6, k);
    }

    gen_key7_2(key7);
    gen_key7_1(key7, key6, ll);

    for (i = 0; i < 16; ++i)
    {
        line[i] = key7[i];
    }
}
static void dec_line(uint8_t *buff, uint8_t *line, uint8_t *key6, int ll)
{
    uint8_t key7[16] = { 0 }; // [esp+Ch] [ebp-20h]
    int i; // [esp+24h] [ebp-8h]
    int k; // [esp+28h] [ebp-4h]

    for (i = 0; i < 16; ++i)
    {
        key7[i] = buff[i];
    }

    gen_key7_1(key7, key6, ll);
    for (k = ll - 1; k > 0; --k)
    {
        revert_gen_key7_2(key7);
        gen_key7_1(key7, key6, k);
        revert_gen_key7_3(key7);
    }
    revert_gen_key7_2(key7);
    gen_key7_1(key7, key6, 0);

    for (i = 0; i < 16; ++i)
    {
        line[i] = key7[i];
    }
}
static int enc_xx(uint8_t *buf, int len, char *key4, int key4len, uint8_t *key5)
{
    std::cout << "buf: 0x" << (void*)buf << std::endl;
    unsigned int v5; // eax
    int v7; // eax
    int _key5[4]; // [esp+18h] [ebp-220h]
    uint8_t copy_key4[512] = { 0 }; // [esp+28h] [ebp-210h]
    int j; // [esp+228h] [ebp-10h]
    int i; // [esp+22Ch] [ebp-Ch]

    v5 = 0;
    do
    {
        *(int *)&copy_key4[v5] = 0;
        v5 += 4;
    } while (v5 < 0x20); //[0, 31]
    if (!buf || !key4 || !key5)
        return 0;
    _key5[0] = *(int *)key5;
    _key5[1] = *((int *)key5 + 1);
    _key5[2] = *((int *)key5 + 2);
    _key5[3] = *((int *)key5 + 3);
    v7 = 32;
    if (key4len <= 32)
        v7 = key4len;
    memcpy(copy_key4, key4, v7);
    gen_key6(copy_key4, copy_key4 + 32, 8);
    std::cout << "buf: 0x" << (void*)buf << std::endl;
    for (i = 0; i < len; i += 16)
    {
        for (j = 0; j <= 15; ++j)
            buf[i + j] ^= *((uint8_t *)_key5 + j);
        enc_line(&buf[i], &buf[i], &copy_key4[32], 14);
        _key5[0] = *(int *)&buf[i];
        _key5[1] = *(int *)&buf[i + 4];
        _key5[2] = *(int *)&buf[i + 8];
        _key5[3] = *(int *)&buf[i + 12];
    }
    return 1;
}
static int dec_xx(uint8_t *buf, int len, char *key4, int key4len, uint8_t *key5)
{
    int n; // eax
    int v8; // [esp+18h] [ebp-230h]
    int v9; // [esp+1Ch] [ebp-22Ch]
    int v10; // [esp+20h] [ebp-228h]
    int v11; // [esp+24h] [ebp-224h]
    int _key5[4]; // [esp+18h] [ebp-220h]
    uint8_t key4_20h[512] = { 0 }; // [esp+38h] [ebp-210h]
    uint8_t key6[512] = { 0 }; // [esp+58h] [ebp-1F0h]
    int j; // [esp+238h] [ebp-10h]
    int i; // [esp+23Ch] [ebp-Ch]

    if (!buf || !key4 || !key5)
        return 0;
    _key5[0] = *(int *)key5;
    _key5[1] = *((int *)key5 + 1);
    _key5[2] = *((int *)key5 + 2);
    _key5[3] = *((int *)key5 + 3);
    n = 32;
    if (key4len <= 32)
        n = key4len;
    memcpy(key4_20h, key4, n);
    gen_key6(key4_20h, key6, 8);
    for (i = 0; i < len; i += 16)
    {
        v8 = *(int *)&buf[i];
        v9 = *(int *)&buf[i + 4];
        v10 = *(int *)&buf[i + 8];
        v11 = *(int *)&buf[i + 12];
        dec_line(&buf[i], &buf[i], (uint8_t *)&key6, 14);
        for (j = 0; j <= 15; ++j)
            buf[i + j] ^= *((uint8_t *)_key5 + j);
        _key5[0] = v8;
        _key5[1] = v9;
        _key5[2] = v10;
        _key5[3] = v11;
    }
    return 1;
}
static void gen_confuse(char *confuse, int len)
{
    char strSource[2] = { 0 }; // [esp+12h] [ebp-16h]

    //srand(time(0));
    srand(0);
    for (int i = 1; i <= len; ++i)
    {
        sprintf(strSource, "%c", s_confuse_str[rand() % s_confuse_str_len]);
        _mbscat((unsigned char *)confuse, (unsigned char *)strSource);
    }
}
static int enc_file(const char *src_path, const char *xpk_path, const char *prop_path)
{
    int new_len; // [esp+1E4h] [ebp-54h]
    unsigned int num_bytes; // [esp+1FCh] [ebp-3Ch]
    char *ziped_new_buf; // [esp+210h] [ebp-28h]
    int i; // [esp+224h] [ebp-14h]
    int k; // [esp+228h] [ebp-10h]
    int j; // [esp+22Ch] [ebp-Ch]


    FILE* fin = fopen("z0", "rb");
    fseek(fin, 0, SEEK_END);
    new_len = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    ziped_new_buf = (char*)malloc(new_len);
    fread(ziped_new_buf, 1, new_len, fin);
    fclose(fin);

    //srand(0)
    char confuse[257] = { 0 };
    gen_confuse(confuse, 256);
    std::cout << "confuse: " << confuse << std::endl;
    //j = 0;
    //v4 = strcspn(s_key2, "9");
    //v5 = strcspn(s_key2, "f") + v4;
    //v6 = strcspn(s_key2, "2") + v5;
    //v7 = v6 - strcspn(s_key2, "6");
    //j = v7 + strcspn(s_key2, "3") - 6;    // j=9
    j = 9;
    char _key3[8] = { 0 }; // [esp+178h] [ebp-C0h]
    for (i = 0; i <= 7; ++i)
        _key3[i] = ziped_new_buf[i] ^ s_key2[i + 8 + j];
    num_bytes = 16 * (signed int)((double)new_len / 16.0 + 1.0);
    auto final_buf = (uint8_t *)malloc(num_bytes);
    if (!final_buf)
        return -7;
    memset(final_buf, 0, num_bytes);
    for (j = 8; j < new_len; j += 8)
    {
        for (k = 0; k <= 7; ++k)
            final_buf[j + k] = _key3[k] ^ ziped_new_buf[j + k];
    }
    new_len = num_bytes;
    char _key4[264] = { 0 };
    for (j = 0; j <= 255; j += 8)
    {
        for (k = 0; k <= 7 && j + k <= 255; ++k)
            _key4[j + k] = _key3[k] ^ confuse[j + k];
    }

    std::cout << "final_buf: 0x" << (void*)final_buf << std::endl;
    enc_xx(final_buf, new_len, _key4, 256, s_key5);
    //dec_xx(final_buf, new_len, _key4, 256, s_key5);

    FILE* fout = fopen("z1", "wb");
    fwrite(final_buf, 1, new_len, fout);
    fclose(fout);

    free(ziped_new_buf);
    free(final_buf);
    return 0;
}
static void enc_lua_obj(char* compiled_data_buf, int lua_obj_len, const char* key, int key_len)
{
    int lua_obj_i = 0;
    int index = 0;
    int v9;
    while (lua_obj_len != lua_obj_i)
    {
        if (index < key_len)
        {
            v9 = index++;
        }
        else
        {
            index = 0;
            v9 = 0;
        }
        compiled_data_buf[lua_obj_i++] ^= key[v9];
    }
}

static int dec_file(const char *xpk)
{
    FILE* fin = fopen(xpk, "rb");
    fseek(fin, 6, SEEK_SET);//sig
    int len = 0;
    fread(&len, 4, 1, fin);
    len -= 0x0C;
    std::cout << "content len=" << len << std::endl;
    if (len <= 0)
    {
        fclose(fin);
        return 1;
    }
    uint8_t zip_head[8] = { 0 };
    fread(zip_head, 8, 1, fin);
    char confuse[257] = { 0 };
    uint8_t* enc_buf = (uint8_t*)malloc(len);
    for (int i = 0; i < len; ++i)
    {
        if (i < 256)
        {
            fread(confuse + i, 1, 1, fin);
        }
        fread(enc_buf + i, 1, 1, fin);
    }
    int src_len = 0;
    fread(&src_len, 4, 1, fin);
    std::cout << "src_len=" << src_len << std::endl;
    std::cout << "confuse: " << confuse << std::endl;
    fclose(fin);
    if (src_len <= 0)
    {
        free(enc_buf);
        return 1;
    }

    //int offset = strcspn(s_v3_key2, "f") + strcspn(s_key2, "9") + strcspn(s_key2, "3") + strcspn(s_key2, "2") + 2 - strcspn(s_key2, "6");
    int offset = 0x11;

    char _key3[8] = { 0 };
    for (int i = 0; i <= 7; ++i)
        _key3[i] = zip_head[i] ^ s_key2[i + offset];
        //_key3[i] = zip_head[i] ^ s_v3_key2[i + offset];
    char _key4[264] = { 0 };
    for (int j = 0; j <= 255; j += 8)
    {
        for (int k = 0; k <= 7 && j + k <= 255; ++k)
            _key4[j + k] = _key3[k] ^ confuse[j + k];
    }
    //dec_xx(enc_buf, len, _key4, 256, s_v3_key5);
    dec_xx(enc_buf, len, _key4, 256, s_key5);


    for (int j = 8; j < len; j += 8)
    {
        for (int k = 0; k <= 7; ++k)
            enc_buf[j + k] ^= _key3[k];
    }
    for (int k = 0; k <= 7; ++k)
        enc_buf[k] = zip_head[k];


    unsigned long uzip_len = src_len;
    uint8_t* uzip_buf = (uint8_t*)malloc(src_len);
    int uzip_ret = uncompress(uzip_buf, &uzip_len, enc_buf, len);
    free(enc_buf);
    if (uzip_ret != Z_OK)
    {
        std::cerr << "unzip error " << uzip_ret << std::endl;
        return 1;
    }

    std::cout << "uzip_len=" << uzip_len << std::endl;
    char* fmt = tpl_peek(TPL_MEM, uzip_buf, uzip_len);
    std::cout << "fmt=" << fmt << std::endl;

    tpl_node *tn;
    int b_n = 0;
    tpl_bin tb;
    int s_n = 0;
    char* s;
    tn = tpl_map("A(iBis)", &b_n, &tb, &s_n, &s);
    tpl_load(tn, TPL_MEM, uzip_buf, uzip_len);
    tpl_unpack(tn, 1);
    std::cout << "b_n=" << b_n << ", s_n=" << s_n << ", s=" << s << std::endl;

    //local s=string.dump(loadfile(\"%s\"), false); return s
    std::string enc_str((char*)tb.addr, tb.sz);
    std::string key(s, s_n);
    tpl_free(tn);

    char* enc = (char*)enc_str.c_str();
    enc_lua_obj(enc, (int)enc_str.size(), key.c_str(), key.size());

    return 0;
}
