#include <cstdlib>
#include <iostream>
#include <fstream>

char* g_table1 = nullptr;
char* g_table2 = nullptr;
char* g_table3 = nullptr;

static void init_xs()
{
    FILE* fin = fopen("t1", "rb");
    fseek(fin, 0, SEEK_END);
    int len = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    g_table1 = (char*)malloc(len);
    fread(g_table1, 1, len, fin);
    fclose(fin);
    
    fin = fopen("t2", "rb");
    fseek(fin, 0, SEEK_END);
    len = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    g_table2 = (char*)malloc(len);
    fread(g_table2, 1, len, fin);
    fclose(fin);
    
    fin = fopen("t3", "rb");
    fseek(fin, 0, SEEK_END);
    len = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    g_table3 = (char*)malloc(len);
    fread(g_table3, 1, len, fin);
    fclose(fin);
}
static void deinit_xs()
{
    free(g_table1);
    g_table1 = nullptr;
    free(g_table2);
    g_table2 = nullptr;
    free(g_table3);
    g_table3 = nullptr;
}
static char * xun_huan_zuo_yi(char *a1)
{
    char v1; // ST0F_1
    
    v1 = *a1;
    *a1 = a1[1];
    a1[1] = a1[2];
    a1[2] = a1[3];
    a1[3] = v1;
    return a1;
}
static void from_table1(char *a1)
{
    a1[0] = g_table1[(unsigned char)a1[0]];
    a1[1] = g_table1[(unsigned char)a1[1]];
    a1[2] = g_table1[(unsigned char)a1[2]];
    a1[3] = g_table1[(unsigned char)a1[3]];
}
static void gen_key6(char *key4, char *key6, int param_value8)
{
    char v4[4]; // [esp+18h] [ebp-20h]
    int v5; // [esp+1Ch] [ebp-1Ch]
    int v6; // [esp+20h] [ebp-18h]
    int value_60; // [esp+24h] [ebp-14h]
    int v8; // [esp+28h] [ebp-10h]
    int value_8_59; // [esp+2Ch] [ebp-Ch]
    
    value_60 = 4 * (param_value8 + 7);
    value_8_59 = 0;
    v8 = 0;
    while ( value_8_59 < param_value8 )
    {
        memcpy(&key6[4 * value_8_59++], &key4[v8], 4u);
        v8 += 4;
    }
    while ( value_8_59 < value_60 )               // v9 [8, 59]
    {
        *(int *)v4 = *(int *)&key6[4 * (value_8_59 - 1)];
        if ( value_8_59 % param_value8 )
        {
            if ( param_value8 > 6 && value_8_59 % param_value8 == 4 )
                from_table1(v4);
        }
        else
        {
            xun_huan_zuo_yi(v4);
            from_table1(v4);
            v4[0] ^= g_table3[value_8_59 / param_value8];
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

int enc_xs()
{
    init_xs();
    
    FILE* fin = fopen("k0", "rb");
    fseek(fin, 0, SEEK_END);
    int len = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    char* k0 = (char*)malloc(len);
    fread(k0, 1, len, fin);
    fclose(fin);
    
    gen_key6(k0, k0+32, 8);
    
    FILE* fout = fopen("k2", "wb");
    fwrite(k0, 1, len, fout);
    fclose(fout);
    
    deinit_xs();
    return 0;
}
