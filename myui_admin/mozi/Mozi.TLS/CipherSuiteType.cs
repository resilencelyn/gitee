﻿namespace Mozi.TLS
{
    /// <summary>
    /// 密码套件类型
    /// </summary>
    internal enum CipherSuiteType : ushort
    {
        TLS_AES_128_CCM_8_SHA256 = 4869,        //表示 TLS_AES_128_CCM_8_SHA256 密码套件
        TLS_AES_128_CCM_SHA256 = 4868,        //表示 TLS_AES_128_CCM_SHA256 密码套件
        TLS_AES_128_GCM_SHA256 = 4865,        //表示 TLS_AES_128_GCM_SHA256 密码套件
        TLS_AES_256_GCM_SHA384 = 4866,        //表示 TLS_AES_256_GCM_SHA384 密码套件
        TLS_CHACHA20_POLY1305_SHA256 = 4867,        //表示 TLS_CHACHA20_POLY1305_SHA256 密码套件
        TLS_DH_anon_EXPORT_WITH_DES40_CBC_SHA = 25,        //表示 TLS_DH_anon_EXPORT_WITH_DES40_CBC_SHA 密码套件
        TLS_DH_anon_EXPORT_WITH_RC4_40_MD5 = 23,        //表示 TLS_DH_anon_EXPORT_WITH_RC4_40_MD5 密码套件
        TLS_DH_anon_WITH_3DES_EDE_CBC_SHA = 27,        //表示 TLS_DH_anon_WITH_3DES_EDE_CBC_SHA 密码套件
        TLS_DH_anon_WITH_AES_128_CBC_SHA = 52,        //表示 TLS_DH_anon_WITH_AES_128_CBC_SHA 密码套件
        TLS_DH_anon_WITH_AES_128_CBC_SHA256 = 108,        //表示 TLS_DH_anon_WITH_AES_128_CBC_SHA256 密码套件
        TLS_DH_anon_WITH_AES_128_GCM_SHA256 = 166,        //表示 TLS_DH_anon_WITH_AES_128_GCM_SHA256 密码套件
        TLS_DH_anon_WITH_AES_256_CBC_SHA = 58,        //表示 TLS_DH_anon_WITH_AES_256_CBC_SHA 密码套件
        TLS_DH_anon_WITH_AES_256_CBC_SHA256 = 109,        //表示 TLS_DH_anon_WITH_AES_256_CBC_SHA256 密码套件
        TLS_DH_anon_WITH_AES_256_GCM_SHA384 = 167,        //表示 TLS_DH_anon_WITH_AES_256_GCM_SHA384 密码套件
        TLS_DH_anon_WITH_ARIA_128_CBC_SHA256 = 49222,        //表示 TLS_DH_anon_WITH_ARIA_128_CBC_SHA256 密码套件
        TLS_DH_anon_WITH_ARIA_128_GCM_SHA256 = 49242,        //表示 TLS_DH_anon_WITH_ARIA_128_GCM_SHA256 密码套件
        TLS_DH_anon_WITH_ARIA_256_CBC_SHA384 = 49223,        //表示 TLS_DH_anon_WITH_ARIA_256_CBC_SHA384 密码套件
        TLS_DH_anon_WITH_ARIA_256_GCM_SHA384 = 49243,        //表示 TLS_DH_anon_WITH_ARIA_256_GCM_SHA384 密码套件
        TLS_DH_anon_WITH_CAMELLIA_128_CBC_SHA = 70,        //表示 TLS_DH_anon_WITH_CAMELLIA_128_CBC_SHA 密码套件
        TLS_DH_anon_WITH_CAMELLIA_128_CBC_SHA256 = 191,        //表示 TLS_DH_anon_WITH_CAMELLIA_128_CBC_SHA256 密码套件
        TLS_DH_anon_WITH_CAMELLIA_128_GCM_SHA256 = 49284,        //表示 TLS_DH_anon_WITH_CAMELLIA_128_GCM_SHA256 密码套件
        TLS_DH_anon_WITH_CAMELLIA_256_CBC_SHA = 137,        //表示 TLS_DH_anon_WITH_CAMELLIA_256_CBC_SHA 密码套件
        TLS_DH_anon_WITH_CAMELLIA_256_CBC_SHA256 = 197,        //表示 TLS_DH_anon_WITH_CAMELLIA_256_CBC_SHA256 密码套件
        TLS_DH_anon_WITH_CAMELLIA_256_GCM_SHA384 = 49285,        //表示 TLS_DH_anon_WITH_CAMELLIA_256_GCM_SHA384 密码套件
        TLS_DH_anon_WITH_DES_CBC_SHA = 26,        //表示 TLS_DH_anon_WITH_DES_CBC_SHA 密码套件
        TLS_DH_anon_WITH_RC4_128_MD5 = 24,        //表示 TLS_DH_anon_WITH_RC4_128_MD5 密码套件
        TLS_DH_anon_WITH_SEED_CBC_SHA = 155,        //表示 TLS_DH_anon_WITH_SEED_CBC_SHA 密码套件
        TLS_DH_DSS_EXPORT_WITH_DES40_CBC_SHA = 11,        //表示 TLS_DH_DSS_EXPORT_WITH_DES40_CBC_SHA 密码套件
        TLS_DH_DSS_WITH_3DES_EDE_CBC_SHA = 13,        //表示 TLS_DH_DSS_WITH_3DES_EDE_CBC_SHA 密码套件
        TLS_DH_DSS_WITH_AES_128_CBC_SHA = 48,        //表示 TLS_DH_DSS_WITH_AES_128_CBC_SHA 密码套件
        TLS_DH_DSS_WITH_AES_128_CBC_SHA256 = 62,        //表示 TLS_DH_DSS_WITH_AES_128_CBC_SHA256 密码套件
        TLS_DH_DSS_WITH_AES_128_GCM_SHA256 = 164,        //表示 TLS_DH_DSS_WITH_AES_128_GCM_SHA256 密码套件
        TLS_DH_DSS_WITH_AES_256_CBC_SHA = 54,        //表示 TLS_DH_DSS_WITH_AES_256_CBC_SHA 密码套件
        TLS_DH_DSS_WITH_AES_256_CBC_SHA256 = 104,        //表示 TLS_DH_DSS_WITH_AES_256_CBC_SHA256 密码套件
        TLS_DH_DSS_WITH_AES_256_GCM_SHA384 = 165,        //表示 TLS_DH_DSS_WITH_AES_256_GCM_SHA384 密码套件
        TLS_DH_DSS_WITH_ARIA_128_CBC_SHA256 = 49214,        //表示 TLS_DH_DSS_WITH_ARIA_128_CBC_SHA256 密码套件
        TLS_DH_DSS_WITH_ARIA_128_GCM_SHA256 = 49240,        //表示 TLS_DH_DSS_WITH_ARIA_128_GCM_SHA256 密码套件
        TLS_DH_DSS_WITH_ARIA_256_CBC_SHA384 = 49215,        //表示 TLS_DH_DSS_WITH_ARIA_256_CBC_SHA384 密码套件
        TLS_DH_DSS_WITH_ARIA_256_GCM_SHA384 = 49241,        //表示 TLS_DH_DSS_WITH_ARIA_256_GCM_SHA384 密码套件
        TLS_DH_DSS_WITH_CAMELLIA_128_CBC_SHA = 66,        //表示 TLS_DH_DSS_WITH_CAMELLIA_128_CBC_SHA 密码套件
        TLS_DH_DSS_WITH_CAMELLIA_128_CBC_SHA256 = 187,        //表示 TLS_DH_DSS_WITH_CAMELLIA_128_CBC_SHA256 密码套件
        TLS_DH_DSS_WITH_CAMELLIA_128_GCM_SHA256 = 49282,        //表示 TLS_DH_DSS_WITH_CAMELLIA_128_GCM_SHA256 密码套件
        TLS_DH_DSS_WITH_CAMELLIA_256_CBC_SHA = 133,        //表示 TLS_DH_DSS_WITH_CAMELLIA_256_CBC_SHA 密码套件
        TLS_DH_DSS_WITH_CAMELLIA_256_CBC_SHA256 = 193,        //表示 TLS_DH_DSS_WITH_CAMELLIA_256_CBC_SHA256 密码套件
        TLS_DH_DSS_WITH_CAMELLIA_256_GCM_SHA384 = 49283,        //表示 TLS_DH_DSS_WITH_CAMELLIA_256_GCM_SHA384 密码套件
        TLS_DH_DSS_WITH_DES_CBC_SHA = 12,        //表示 TLS_DH_DSS_WITH_DES_CBC_SHA 密码套件
        TLS_DH_DSS_WITH_SEED_CBC_SHA = 151,        //表示 TLS_DH_DSS_WITH_SEED_CBC_SHA 密码套件
        TLS_DH_RSA_EXPORT_WITH_DES40_CBC_SHA = 14,        //表示 TLS_DH_RSA_EXPORT_WITH_DES40_CBC_SHA 密码套件
        TLS_DH_RSA_WITH_3DES_EDE_CBC_SHA = 16,        //表示 TLS_DH_RSA_WITH_3DES_EDE_CBC_SHA 密码套件
        TLS_DH_RSA_WITH_AES_128_CBC_SHA = 49,        //表示 TLS_DH_RSA_WITH_AES_128_CBC_SHA 密码套件
        TLS_DH_RSA_WITH_AES_128_CBC_SHA256 = 63,        //表示 TLS_DH_RSA_WITH_AES_128_CBC_SHA256 密码套件
        TLS_DH_RSA_WITH_AES_128_GCM_SHA256 = 160,        //表示 TLS_DH_RSA_WITH_AES_128_GCM_SHA256 密码套件
        TLS_DH_RSA_WITH_AES_256_CBC_SHA = 55,        //表示 TLS_DH_RSA_WITH_AES_256_CBC_SHA 密码套件
        TLS_DH_RSA_WITH_AES_256_CBC_SHA256 = 105,        //表示 TLS_DH_RSA_WITH_AES_256_CBC_SHA256 密码套件
        TLS_DH_RSA_WITH_AES_256_GCM_SHA384 = 161,        //表示 TLS_DH_RSA_WITH_AES_256_GCM_SHA384 密码套件
        TLS_DH_RSA_WITH_ARIA_128_CBC_SHA256 = 49216,        //表示 TLS_DH_RSA_WITH_ARIA_128_CBC_SHA256 密码套件
        TLS_DH_RSA_WITH_ARIA_128_GCM_SHA256 = 49236,        //表示 TLS_DH_RSA_WITH_ARIA_128_GCM_SHA256 密码套件
        TLS_DH_RSA_WITH_ARIA_256_CBC_SHA384 = 49217,        //表示 TLS_DH_RSA_WITH_ARIA_256_CBC_SHA384 密码套件
        TLS_DH_RSA_WITH_ARIA_256_GCM_SHA384 = 49237,        //表示 TLS_DH_RSA_WITH_ARIA_256_GCM_SHA384 密码套件
        TLS_DH_RSA_WITH_CAMELLIA_128_CBC_SHA = 67,        //表示 TLS_DH_RSA_WITH_CAMELLIA_128_CBC_SHA 密码套件
        TLS_DH_RSA_WITH_CAMELLIA_128_CBC_SHA256 = 188,        //表示 TLS_DH_RSA_WITH_CAMELLIA_128_CBC_SHA256 密码套件
        TLS_DH_RSA_WITH_CAMELLIA_128_GCM_SHA256 = 49278,        //表示 TLS_DH_RSA_WITH_CAMELLIA_128_GCM_SHA256 密码套件
        TLS_DH_RSA_WITH_CAMELLIA_256_CBC_SHA = 134,        //表示 TLS_DH_RSA_WITH_CAMELLIA_256_CBC_SHA 密码套件
        TLS_DH_RSA_WITH_CAMELLIA_256_CBC_SHA256 = 194,        //表示 TLS_DH_RSA_WITH_CAMELLIA_256_CBC_SHA256 密码套件
        TLS_DH_RSA_WITH_CAMELLIA_256_GCM_SHA384 = 49279,        //表示 TLS_DH_RSA_WITH_CAMELLIA_256_GCM_SHA384 密码套件
        TLS_DH_RSA_WITH_DES_CBC_SHA = 15,        //表示 TLS_DH_RSA_WITH_DES_CBC_SHA 密码套件
        TLS_DH_RSA_WITH_SEED_CBC_SHA = 152,        //表示 TLS_DH_RSA_WITH_SEED_CBC_SHA 密码套件
        TLS_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA = 17,        //表示 TLS_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA 密码套件
        TLS_DHE_DSS_WITH_3DES_EDE_CBC_SHA = 19,        //表示 TLS_DHE_DSS_WITH_3DES_EDE_CBC_SHA 密码套件
        TLS_DHE_DSS_WITH_AES_128_CBC_SHA = 50,        //表示 TLS_DHE_DSS_WITH_AES_128_CBC_SHA 密码套件
        TLS_DHE_DSS_WITH_AES_128_CBC_SHA256 = 64,        //表示 TLS_DHE_DSS_WITH_AES_128_CBC_SHA256 密码套件
        TLS_DHE_DSS_WITH_AES_128_GCM_SHA256 = 162,        //表示 TLS_DHE_DSS_WITH_AES_128_GCM_SHA256 密码套件
        TLS_DHE_DSS_WITH_AES_256_CBC_SHA = 56,        //表示 TLS_DHE_DSS_WITH_AES_256_CBC_SHA 密码套件
        TLS_DHE_DSS_WITH_AES_256_CBC_SHA256 = 106,        //表示 TLS_DHE_DSS_WITH_AES_256_CBC_SHA256 密码套件
        TLS_DHE_DSS_WITH_AES_256_GCM_SHA384 = 163,        //表示 TLS_DHE_DSS_WITH_AES_256_GCM_SHA384 密码套件
        TLS_DHE_DSS_WITH_ARIA_128_CBC_SHA256 = 49218,        //表示 TLS_DHE_DSS_WITH_ARIA_128_CBC_SHA256 密码套件
        TLS_DHE_DSS_WITH_ARIA_128_GCM_SHA256 = 49238,        //表示 TLS_DHE_DSS_WITH_ARIA_128_GCM_SHA256 密码套件
        TLS_DHE_DSS_WITH_ARIA_256_CBC_SHA384 = 49219,        //表示 TLS_DHE_DSS_WITH_ARIA_256_CBC_SHA384 密码套件
        TLS_DHE_DSS_WITH_ARIA_256_GCM_SHA384 = 49239,        //表示 TLS_DHE_DSS_WITH_ARIA_256_GCM_SHA384 密码套件
        TLS_DHE_DSS_WITH_CAMELLIA_128_CBC_SHA = 68,        //表示 TLS_DHE_DSS_WITH_CAMELLIA_128_CBC_SHA 密码套件
        TLS_DHE_DSS_WITH_CAMELLIA_128_CBC_SHA256 = 189,        //表示 TLS_DHE_DSS_WITH_CAMELLIA_128_CBC_SHA256 密码套件
        TLS_DHE_DSS_WITH_CAMELLIA_128_GCM_SHA256 = 49280,        //表示 TLS_DHE_DSS_WITH_CAMELLIA_128_GCM_SHA256 密码套件
        TLS_DHE_DSS_WITH_CAMELLIA_256_CBC_SHA = 135,        //表示 TLS_DHE_DSS_WITH_CAMELLIA_256_CBC_SHA 密码套件
        TLS_DHE_DSS_WITH_CAMELLIA_256_CBC_SHA256 = 195,        //表示 TLS_DHE_DSS_WITH_CAMELLIA_256_CBC_SHA256 密码套件
        TLS_DHE_DSS_WITH_CAMELLIA_256_GCM_SHA384 = 49281,        //表示 TLS_DHE_DSS_WITH_CAMELLIA_256_GCM_SHA384 密码套件
        TLS_DHE_DSS_WITH_DES_CBC_SHA = 18,        //表示 TLS_DHE_DSS_WITH_DES_CBC_SHA 密码套件
        TLS_DHE_DSS_WITH_SEED_CBC_SHA = 153,        //表示 TLS_DHE_DSS_WITH_SEED_CBC_SHA 密码套件
        TLS_DHE_PSK_WITH_3DES_EDE_CBC_SHA = 143,        //表示 TLS_DHE_PSK_WITH_3DES_EDE_CBC_SHA 密码套件
        TLS_DHE_PSK_WITH_AES_128_CBC_SHA = 144,        //表示 TLS_DHE_PSK_WITH_AES_128_CBC_SHA 密码套件
        TLS_DHE_PSK_WITH_AES_128_CBC_SHA256 = 178,        //表示 TLS_DHE_PSK_WITH_AES_128_CBC_SHA256 密码套件
        TLS_DHE_PSK_WITH_AES_128_CCM = 49318,        //表示 TLS_DHE_PSK_WITH_AES_128_CCM 密码套件
        TLS_DHE_PSK_WITH_AES_128_GCM_SHA256 = 170,        //表示 TLS_DHE_PSK_WITH_AES_128_GCM_SHA256 密码套件
        TLS_DHE_PSK_WITH_AES_256_CBC_SHA = 145,        //表示 TLS_DHE_PSK_WITH_AES_256_CBC_SHA 密码套件
        TLS_DHE_PSK_WITH_AES_256_CBC_SHA384 = 179,        //表示 TLS_DHE_PSK_WITH_AES_256_CBC_SHA384 密码套件
        TLS_DHE_PSK_WITH_AES_256_CCM = 49319,        //表示 TLS_DHE_PSK_WITH_AES_256_CCM 密码套件
        TLS_DHE_PSK_WITH_AES_256_GCM_SHA384 = 171,        //表示 TLS_DHE_PSK_WITH_AES_256_GCM_SHA384 密码套件
        TLS_DHE_PSK_WITH_ARIA_128_CBC_SHA256 = 49254,        //表示 TLS_DHE_PSK_WITH_ARIA_128_CBC_SHA256 密码套件
        TLS_DHE_PSK_WITH_ARIA_128_GCM_SHA256 = 49260,        //表示 TLS_DHE_PSK_WITH_ARIA_128_GCM_SHA256 密码套件
        TLS_DHE_PSK_WITH_ARIA_256_CBC_SHA384 = 49255,        //表示 TLS_DHE_PSK_WITH_ARIA_256_CBC_SHA384 密码套件
        TLS_DHE_PSK_WITH_ARIA_256_GCM_SHA384 = 49261,        //表示 TLS_DHE_PSK_WITH_ARIA_256_GCM_SHA384 密码套件
        TLS_DHE_PSK_WITH_CAMELLIA_128_CBC_SHA256 = 49302,        //表示 TLS_DHE_PSK_WITH_CAMELLIA_128_CBC_SHA256 密码套件
        TLS_DHE_PSK_WITH_CAMELLIA_128_GCM_SHA256 = 49296,        //表示 TLS_DHE_PSK_WITH_CAMELLIA_128_GCM_SHA256 密码套件
        TLS_DHE_PSK_WITH_CAMELLIA_256_CBC_SHA384 = 49303,        //表示 TLS_DHE_PSK_WITH_CAMELLIA_256_CBC_SHA384 密码套件
        TLS_DHE_PSK_WITH_CAMELLIA_256_GCM_SHA384 = 49297,        //表示 TLS_DHE_PSK_WITH_CAMELLIA_256_GCM_SHA384 密码套件
        TLS_DHE_PSK_WITH_CHACHA20_POLY1305_SHA256 = 52397,        //表示 TLS_DHE_PSK_WITH_CHACHA20_POLY1305_SHA256 密码套件
        TLS_DHE_PSK_WITH_NULL_SHA = 45,        //表示 TLS_DHE_PSK_WITH_NULL_SHA 密码套件
        TLS_DHE_PSK_WITH_NULL_SHA256 = 180,        //表示 TLS_DHE_PSK_WITH_NULL_SHA256 密码套件
        TLS_DHE_PSK_WITH_NULL_SHA384 = 181,        //表示 TLS_DHE_PSK_WITH_NULL_SHA384 密码套件
        TLS_DHE_PSK_WITH_RC4_128_SHA = 142,        //表示 TLS_DHE_PSK_WITH_RC4_128_SHA 密码套件
        TLS_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA = 20,        //表示 TLS_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA 密码套件
        TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA = 22,        //表示 TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA 密码套件
        TLS_DHE_RSA_WITH_AES_128_CBC_SHA = 51,        //表示 TLS_DHE_RSA_WITH_AES_128_CBC_SHA 密码套件
        TLS_DHE_RSA_WITH_AES_128_CBC_SHA256 = 103,        //表示 TLS_DHE_RSA_WITH_AES_128_CBC_SHA256 密码套件
        TLS_DHE_RSA_WITH_AES_128_CCM = 49310,        //表示 TLS_DHE_RSA_WITH_AES_128_CCM 密码套件
        TLS_DHE_RSA_WITH_AES_128_CCM_8 = 49314,        //表示 TLS_DHE_RSA_WITH_AES_128_CCM_8 密码套件
        TLS_DHE_RSA_WITH_AES_128_GCM_SHA256 = 158,        //表示 TLS_DHE_RSA_WITH_AES_128_GCM_SHA256 密码套件
        TLS_DHE_RSA_WITH_AES_256_CBC_SHA = 57,        //表示 TLS_DHE_RSA_WITH_AES_256_CBC_SHA 密码套件
        TLS_DHE_RSA_WITH_AES_256_CBC_SHA256 = 107,        //表示 TLS_DHE_RSA_WITH_AES_256_CBC_SHA256 密码套件
        TLS_DHE_RSA_WITH_AES_256_CCM = 49311,        //表示 TLS_DHE_RSA_WITH_AES_256_CCM 密码套件
        TLS_DHE_RSA_WITH_AES_256_CCM_8 = 49315,        //表示 TLS_DHE_RSA_WITH_AES_256_CCM_8 密码套件
        TLS_DHE_RSA_WITH_AES_256_GCM_SHA384 = 159,        //表示 TLS_DHE_RSA_WITH_AES_256_GCM_SHA384 密码套件
        TLS_DHE_RSA_WITH_ARIA_128_CBC_SHA256 = 49220,        //表示 TLS_DHE_RSA_WITH_ARIA_128_CBC_SHA256 密码套件
        TLS_DHE_RSA_WITH_ARIA_128_GCM_SHA256 = 49234,        //表示 TLS_DHE_RSA_WITH_ARIA_128_GCM_SHA256 密码套件
        TLS_DHE_RSA_WITH_ARIA_256_CBC_SHA384 = 49221,        //表示 TLS_DHE_RSA_WITH_ARIA_256_CBC_SHA384 密码套件
        TLS_DHE_RSA_WITH_ARIA_256_GCM_SHA384 = 49235,        //表示 TLS_DHE_RSA_WITH_ARIA_256_GCM_SHA384 密码套件
        TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA = 69,        //表示 TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA 密码套件
        TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256 = 190,        //表示 TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256 密码套件
        TLS_DHE_RSA_WITH_CAMELLIA_128_GCM_SHA256 = 49276,        //表示 TLS_DHE_RSA_WITH_CAMELLIA_128_GCM_SHA256 密码套件
        TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA = 136,        //表示 TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA 密码套件
        TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256 = 196,        //表示 TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256 密码套件
        TLS_DHE_RSA_WITH_CAMELLIA_256_GCM_SHA384 = 49277,        //表示 TLS_DHE_RSA_WITH_CAMELLIA_256_GCM_SHA384 密码套件
        TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256 = 52394,        //表示 TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256 密码套件
        TLS_DHE_RSA_WITH_DES_CBC_SHA = 21,        //表示 TLS_DHE_RSA_WITH_DES_CBC_SHA 密码套件
        TLS_DHE_RSA_WITH_SEED_CBC_SHA = 154,        //表示 TLS_DHE_RSA_WITH_SEED_CBC_SHA 密码套件
        TLS_ECCPWD_WITH_AES_128_CCM_SHA256 = 49330,        //表示 TLS_ECCPWD_WITH_AES_128_CCM_SHA256 密码套件
        TLS_ECCPWD_WITH_AES_128_GCM_SHA256 = 49328,        //表示 TLS_ECCPWD_WITH_AES_128_GCM_SHA256 密码套件
        TLS_ECCPWD_WITH_AES_256_CCM_SHA384 = 49331,        //表示 TLS_ECCPWD_WITH_AES_256_CCM_SHA384 密码套件
        TLS_ECCPWD_WITH_AES_256_GCM_SHA384 = 49329,        //表示 TLS_ECCPWD_WITH_AES_256_GCM_SHA384 密码套件
        TLS_ECDH_anon_WITH_3DES_EDE_CBC_SHA = 49175,        //表示 TLS_ECDH_anon_WITH_3DES_EDE_CBC_SHA 密码套件
        TLS_ECDH_anon_WITH_AES_128_CBC_SHA = 49176,        //表示 TLS_ECDH_anon_WITH_AES_128_CBC_SHA 密码套件
        TLS_ECDH_anon_WITH_AES_256_CBC_SHA = 49177,        //表示 TLS_ECDH_anon_WITH_AES_256_CBC_SHA 密码套件
        TLS_ECDH_anon_WITH_NULL_SHA = 49173,        //表示 TLS_ECDH_anon_WITH_NULL_SHA 密码套件
        TLS_ECDH_anon_WITH_RC4_128_SHA = 49174,        //表示 TLS_ECDH_anon_WITH_RC4_128_SHA 密码套件
        TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA = 49155,        //表示 TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA 密码套件
        TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA = 49156,        //表示 TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA 密码套件
        TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256 = 49189,        //表示 TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256 密码套件
        TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256 = 49197,        //表示 TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256 密码套件
        TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA = 49157,        //表示 TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA 密码套件
        TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384 = 49190,        //表示TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384 密码套件
        TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384 = 49198,        //表示 TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384 密码套件
        TLS_ECDH_ECDSA_WITH_ARIA_128_CBC_SHA256 = 49226,        //表示 TLS_ECDH_ECDSA_WITH_ARIA_128_CBC_SHA256 密码套件
        TLS_ECDH_ECDSA_WITH_ARIA_128_GCM_SHA256 = 49246,        //表示 TLS_ECDH_ECDSA_WITH_ARIA_128_GCM_SHA256 密码套件
        TLS_ECDH_ECDSA_WITH_ARIA_256_CBC_SHA384 = 49227,        //表示 TLS_ECDH_ECDSA_WITH_ARIA_256_CBC_SHA384 密码套件
        TLS_ECDH_ECDSA_WITH_ARIA_256_GCM_SHA384 = 49247,        //表示 TLS_ECDH_ECDSA_WITH_ARIA_256_GCM_SHA384 密码套件
        TLS_ECDH_ECDSA_WITH_CAMELLIA_128_CBC_SHA256 = 49268,        //表示 TLS_ECDH_ECDSA_WITH_CAMELLIA_128_CBC_SHA256 密码套件
        TLS_ECDH_ECDSA_WITH_CAMELLIA_128_GCM_SHA256 = 49288,        //表示 TLS_ECDH_ECDSA_WITH_CAMELLIA_128_GCM_SHA256 密码套件
        TLS_ECDH_ECDSA_WITH_CAMELLIA_256_CBC_SHA384 = 49269,        //表示 TLS_ECDH_ECDSA_WITH_CAMELLIA_256_CBC_SHA384 密码套件
        TLS_ECDH_ECDSA_WITH_CAMELLIA_256_GCM_SHA384 = 49289,        //表示 TLS_ECDH_ECDSA_WITH_CAMELLIA_256_GCM_SHA384 密码套件
        TLS_ECDH_ECDSA_WITH_NULL_SHA = 49153,        //表示 TLS_ECDH_ECDSA_WITH_NULL_SHA 密码套件
        TLS_ECDH_ECDSA_WITH_RC4_128_SHA = 49154,        //表示 TLS_ECDH_ECDSA_WITH_RC4_128_SHA 密码套件
        TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA = 49165,        //表示 TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA 密码套件
        TLS_ECDH_RSA_WITH_AES_128_CBC_SHA = 49166,        //表示 TLS_ECDH_RSA_WITH_AES_128_CBC_SHA 密码套件
        TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256 = 49193,        //表示 TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256 密码套件
        TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256 = 49201,        //表示 TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256 密码套件
        TLS_ECDH_RSA_WITH_AES_256_CBC_SHA = 49167,        //表示 TLS_ECDH_RSA_WITH_AES_256_CBC_SHA 密码套件
        TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384 = 49194,        //表示 TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384 密码套件
        TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384 = 49202,        //表示 TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384 密码套件
        TLS_ECDH_RSA_WITH_ARIA_128_CBC_SHA256 = 49230,        //表示 TLS_ECDH_RSA_WITH_ARIA_128_CBC_SHA256 密码套件
        TLS_ECDH_RSA_WITH_ARIA_128_GCM_SHA256 = 49250,        //表示 TLS_ECDH_RSA_WITH_ARIA_128_GCM_SHA256 密码套件
        TLS_ECDH_RSA_WITH_ARIA_256_CBC_SHA384 = 49231,        //表示 TLS_ECDH_RSA_WITH_ARIA_256_CBC_SHA384 密码套件
        TLS_ECDH_RSA_WITH_ARIA_256_GCM_SHA384 = 49251,        //表示 TLS_ECDH_RSA_WITH_ARIA_256_GCM_SHA384 密码套件
        TLS_ECDH_RSA_WITH_CAMELLIA_128_CBC_SHA256 = 49272,        //表示 TLS_ECDH_RSA_WITH_CAMELLIA_128_CBC_SHA256 密码套件
        TLS_ECDH_RSA_WITH_CAMELLIA_128_GCM_SHA256 = 49292,        //表示 TLS_ECDH_RSA_WITH_CAMELLIA_128_GCM_SHA256 密码套件
        TLS_ECDH_RSA_WITH_CAMELLIA_256_CBC_SHA384 = 49273,        //表示 TLS_ECDH_RSA_WITH_CAMELLIA_256_CBC_SHA384 密码套件
        TLS_ECDH_RSA_WITH_CAMELLIA_256_GCM_SHA384 = 49293,        //表示 TLS_ECDH_RSA_WITH_CAMELLIA_256_GCM_SHA384 密码套件
        TLS_ECDH_RSA_WITH_NULL_SHA = 49163,        //表示 TLS_ECDH_RSA_WITH_NULL_SHA 密码套件
        TLS_ECDH_RSA_WITH_RC4_128_SHA = 49164,        //表示 TLS_ECDH_RSA_WITH_RC4_128_SHA 密码套件
        TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA = 49160,        //表示 TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA 密码套件
        TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA = 49161,        //表示 TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA 密码套件
        TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256 = 49187,        //表示 TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256 密码套件
        TLS_ECDHE_ECDSA_WITH_AES_128_CCM = 49324,        //表示 TLS_ECDHE_ECDSA_WITH_AES_128_CCM 密码套件
        TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8 = 49326,        //表示 TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8 密码套件
        TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256 = 49195,        //表示 TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256 密码套件
        TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA = 49162,        //表示 TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA 密码套件
        TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384 = 49188,        //表示 TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384 密码套件
        TLS_ECDHE_ECDSA_WITH_AES_256_CCM = 49325,        //表示 TLS_ECDHE_ECDSA_WITH_AES_256_CCM 密码套件
        TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8 = 49327,        //表示 TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8 密码套件
        TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384 = 49196,        //表示 TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384 密码套件
        TLS_ECDHE_ECDSA_WITH_ARIA_128_CBC_SHA256 = 49224,        //表示TLS_ECDHE_ECDSA_WITH_ARIA_128_CBC_SHA256 密码套件
        TLS_ECDHE_ECDSA_WITH_ARIA_128_GCM_SHA256 = 49244,        //表示 TLS_ECDHE_ECDSA_WITH_ARIA_128_GCM_SHA256 密码套件
        TLS_ECDHE_ECDSA_WITH_ARIA_256_CBC_SHA384 = 49225,        //表示 TLS_ECDHE_ECDSA_WITH_ARIA_256_CBC_SHA384 密码套件
        TLS_ECDHE_ECDSA_WITH_ARIA_256_GCM_SHA384 = 49245,        //表示 TLS_ECDHE_ECDSA_WITH_ARIA_256_GCM_SHA384 密码套件
        TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_CBC_SHA256 = 49266,        //表示 TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_CBC_SHA256 密码套件
        TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_GCM_SHA256 = 49286,        //表示 TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_GCM_SHA256 密码套件
        TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_CBC_SHA384 = 49267,        //表示 TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_CBC_SHA384 密码套件
        TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_GCM_SHA384 = 49287,        //表示 TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_GCM_SHA384 密码套件
        TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256 = 52393,        //表示 TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256 密码套件
        TLS_ECDHE_ECDSA_WITH_NULL_SHA = 49158,        //表示 TLS_ECDHE_ECDSA_WITH_NULL_SHA 密码套件
        TLS_ECDHE_ECDSA_WITH_RC4_128_SHA = 49159,        //表示 TLS_ECDHE_ECDSA_WITH_RC4_128_SHA 密码套件
        TLS_ECDHE_PSK_WITH_3DES_EDE_CBC_SHA = 49204,        //表示 TLS_ECDHE_PSK_WITH_3DES_EDE_CBC_SHA 密码套件
        TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA = 49205,        //表示 TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA 密码套件
        TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256 = 49207,        //表示 TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256 密码套件
        TLS_ECDHE_PSK_WITH_AES_128_CCM_8_SHA256 = 53251,        //表示 TLS_ECDHE_PSK_WITH_AES_128_CCM_8_SHA256 密码套件
        TLS_ECDHE_PSK_WITH_AES_128_CCM_SHA256 = 53253,        //表示 TLS_ECDHE_PSK_WITH_AES_128_CCM_SHA256 密码套件
        TLS_ECDHE_PSK_WITH_AES_128_GCM_SHA256 = 53249,        //表示 TLS_ECDHE_PSK_WITH_AES_128_GCM_SHA256 密码套件
        TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA = 49206,        //表示 TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA 密码套件
        TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA384 = 49208,        //表示 TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA384 密码套件
        TLS_ECDHE_PSK_WITH_AES_256_GCM_SHA384 = 53250,        //表示 TLS_ECDHE_PSK_WITH_AES_256_GCM_SHA384 密码套件
        TLS_ECDHE_PSK_WITH_ARIA_128_CBC_SHA256 = 49264,        //表示 TLS_ECDHE_PSK_WITH_ARIA_128_CBC_SHA256 密码套件
        TLS_ECDHE_PSK_WITH_ARIA_256_CBC_SHA384 = 49265,        //表示TLS_ECDHE_PSK_WITH_ARIA_256_CBC_SHA384 密码套件
        TLS_ECDHE_PSK_WITH_CAMELLIA_128_CBC_SHA256 = 49306,        //表示 TLS_ECDHE_PSK_WITH_CAMELLIA_128_CBC_SHA256 密码套件
        TLS_ECDHE_PSK_WITH_CAMELLIA_256_CBC_SHA384 = 49307,        //表示 TLS_ECDHE_PSK_WITH_CAMELLIA_256_CBC_SHA384 密码套件
        TLS_ECDHE_PSK_WITH_CHACHA20_POLY1305_SHA256 = 52396,        //表示 TLS_ECDHE_PSK_WITH_CHACHA20_POLY1305_SHA256 密码套件
        TLS_ECDHE_PSK_WITH_NULL_SHA = 49209,        //表示 TLS_ECDHE_PSK_WITH_NULL_SHA 密码套件
        TLS_ECDHE_PSK_WITH_NULL_SHA256 = 49210,        //表示 TLS_ECDHE_PSK_WITH_NULL_SHA256 密码套件
        TLS_ECDHE_PSK_WITH_NULL_SHA384 = 49211,        //表示 TLS_ECDHE_PSK_WITH_NULL_SHA384 密码套件
        TLS_ECDHE_PSK_WITH_RC4_128_SHA = 49203,        //表示 TLS_ECDHE_PSK_WITH_RC4_128_SHA 密码套件
        TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA = 49170,        //表示 TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA 密码套件
        TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA = 49171,        //表示 TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA 密码套件
        TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256 = 49191,        //表示 TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256 密码套件
        TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256 = 49199,        //表示 TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256 密码套件
        TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA = 49172,        //表示 TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA 密码套件
        TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384 = 49192,        //表示 TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384 密码套件
        TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384 = 49200,        //表示 TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384 密码套件
        TLS_ECDHE_RSA_WITH_ARIA_128_CBC_SHA256 = 49228,        //表示 TLS_ECDHE_RSA_WITH_ARIA_128_CBC_SHA256 密码套件
        TLS_ECDHE_RSA_WITH_ARIA_128_GCM_SHA256 = 49248,        //表示 TLS_ECDHE_RSA_WITH_ARIA_128_GCM_SHA256 密码套件
        TLS_ECDHE_RSA_WITH_ARIA_256_CBC_SHA384 = 49229,        //表示 TLS_ECDHE_RSA_WITH_ARIA_256_CBC_SHA384 密码套件
        TLS_ECDHE_RSA_WITH_ARIA_256_GCM_SHA384 = 49249,        //表示 TLS_ECDHE_RSA_WITH_ARIA_256_GCM_SHA384 密码套件
        TLS_ECDHE_RSA_WITH_CAMELLIA_128_CBC_SHA256 = 49270,        //表示 TLS_ECDHE_RSA_WITH_CAMELLIA_128_CBC_SHA256 密码套件
        TLS_ECDHE_RSA_WITH_CAMELLIA_128_GCM_SHA256 = 49290,        //表示 TLS_ECDHE_RSA_WITH_CAMELLIA_128_GCM_SHA256 密码套件
        TLS_ECDHE_RSA_WITH_CAMELLIA_256_CBC_SHA384 = 49271,        //表示 TLS_ECDHE_RSA_WITH_CAMELLIA_256_CBC_SHA384 密码套件
        TLS_ECDHE_RSA_WITH_CAMELLIA_256_GCM_SHA384 = 49291,        //表示TLS_ECDHE_RSA_WITH_CAMELLIA_256_GCM_SHA384 密码套件
        TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256 = 52392,        //表示 TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256 密码套件
        TLS_ECDHE_RSA_WITH_NULL_SHA = 49168,        //表示 TLS_ECDHE_RSA_WITH_NULL_SHA 密码套件
        TLS_ECDHE_RSA_WITH_RC4_128_SHA = 49169,        //表示 TLS_ECDHE_RSA_WITH_RC4_128_SHA 密码套件
        TLS_KRB5_EXPORT_WITH_DES_CBC_40_MD5 = 41,        //表示 TLS_KRB5_EXPORT_WITH_DES_CBC_40_MD5 密码套件
        TLS_KRB5_EXPORT_WITH_DES_CBC_40_SHA = 38,        //表示 TLS_KRB5_EXPORT_WITH_DES_CBC_40_SHA 密码套件
        TLS_KRB5_EXPORT_WITH_RC2_CBC_40_MD5 = 42,        //表示 TLS_KRB5_EXPORT_WITH_RC2_CBC_40_MD5 密码套件
        TLS_KRB5_EXPORT_WITH_RC2_CBC_40_SHA = 39,        //表示 TLS_KRB5_EXPORT_WITH_RC2_CBC_40_SHA 密码套件
        TLS_KRB5_EXPORT_WITH_RC4_40_MD5 = 43,        //表示 TLS_KRB5_EXPORT_WITH_RC4_40_MD5 密码套件
        TLS_KRB5_EXPORT_WITH_RC4_40_SHA = 40,        //表示 TLS_KRB5_EXPORT_WITH_RC4_40_SHA 密码套件
        TLS_KRB5_WITH_3DES_EDE_CBC_MD5 = 35,        //表示 TLS_KRB5_WITH_3DES_EDE_CBC_MD5 密码套件
        TLS_KRB5_WITH_3DES_EDE_CBC_SHA = 31,        //表示 TLS_KRB5_WITH_3DES_EDE_CBC_SHA 密码套件
        TLS_KRB5_WITH_DES_CBC_MD5 = 34,        //表示 TLS_KRB5_WITH_DES_CBC_MD5 密码套件
        TLS_KRB5_WITH_DES_CBC_SHA = 30,        //表示 TLS_KRB5_WITH_DES_CBC_SHA 密码套件
        TLS_KRB5_WITH_IDEA_CBC_MD5 = 37,        //表示 TLS_KRB5_WITH_IDEA_CBC_MD5 密码套件
        TLS_KRB5_WITH_IDEA_CBC_SHA = 33,        //表示 TLS_KRB5_WITH_IDEA_CBC_SHA 密码套件
        TLS_KRB5_WITH_RC4_128_MD5 = 36,        //表示 TLS_KRB5_WITH_RC4_128_MD5 密码套件
        TLS_KRB5_WITH_RC4_128_SHA = 32,        //表示 TLS_KRB5_WITH_RC4_128_SHA 密码套件
        TLS_NULL_WITH_NULL_NULL = 0,        //表示 TLS_NULL_WITH_NULL_NULL 密码套件
        TLS_PSK_DHE_WITH_AES_128_CCM_8 = 49322,        //表示 TLS_PSK_DHE_WITH_AES_128_CCM_8 密码套件
        TLS_PSK_DHE_WITH_AES_256_CCM_8 = 49323,        //表示 TLS_PSK_DHE_WITH_AES_256_CCM_8 密码套件
        TLS_PSK_WITH_3DES_EDE_CBC_SHA = 139,        //表示 TLS_PSK_WITH_3DES_EDE_CBC_SHA 密码套件
        TLS_PSK_WITH_AES_128_CBC_SHA = 140,        //表示 TLS_PSK_WITH_AES_128_CBC_SHA 密码套件
        TLS_PSK_WITH_AES_128_CBC_SHA256 = 174,        //表示 TLS_PSK_WITH_AES_128_CBC_SHA256 密码套件
        TLS_PSK_WITH_AES_128_CCM = 49316,        //表示 TLS_PSK_WITH_AES_128_CCM 密码套件
        TLS_PSK_WITH_AES_128_CCM_8 = 49320,        //表示 TLS_PSK_WITH_AES_128_CCM_8 密码套件
        TLS_PSK_WITH_AES_128_GCM_SHA256 = 168,        //表示 TLS_PSK_WITH_AES_128_GCM_SHA256 密码套件
        TLS_PSK_WITH_AES_256_CBC_SHA = 141,        //表示 TLS_PSK_WITH_AES_256_CBC_SHA 密码套件
        TLS_PSK_WITH_AES_256_CBC_SHA384 = 175,        //表示 TLS_PSK_WITH_AES_256_CBC_SHA384 密码套件
        TLS_PSK_WITH_AES_256_CCM = 49317,        //表示 TLS_PSK_WITH_AES_256_CCM 密码套件
        TLS_PSK_WITH_AES_256_CCM_8 = 49321,        //表示 TLS_PSK_WITH_AES_256_CCM_8 密码套件
        TLS_PSK_WITH_AES_256_GCM_SHA384 = 169,        //表示 TLS_PSK_WITH_AES_256_GCM_SHA384 密码套件
        TLS_PSK_WITH_ARIA_128_CBC_SHA256 = 49252,        //表示 TLS_PSK_WITH_ARIA_128_CBC_SHA256 密码套件
        TLS_PSK_WITH_ARIA_128_GCM_SHA256 = 49258,        //表示 TLS_PSK_WITH_ARIA_128_GCM_SHA256 密码套件
        TLS_PSK_WITH_ARIA_256_CBC_SHA384 = 49253,        //表示 TLS_PSK_WITH_ARIA_256_CBC_SHA384 密码套件
        TLS_PSK_WITH_ARIA_256_GCM_SHA384 = 49259,        //表示 TLS_PSK_WITH_ARIA_256_GCM_SHA384 密码套件
        TLS_PSK_WITH_CAMELLIA_128_CBC_SHA256 = 49300,        //表示 TLS_PSK_WITH_CAMELLIA_128_CBC_SHA256 密码套件
        TLS_PSK_WITH_CAMELLIA_128_GCM_SHA256 = 49294,        //表示 TLS_PSK_WITH_CAMELLIA_128_GCM_SHA256 密码套件
        TLS_PSK_WITH_CAMELLIA_256_CBC_SHA384 = 49301,        //表示 TLS_PSK_WITH_CAMELLIA_256_CBC_SHA384 密码套件
        TLS_PSK_WITH_CAMELLIA_256_GCM_SHA384 = 49295,        //表示 TLS_PSK_WITH_CAMELLIA_256_GCM_SHA384 密码套件
        TLS_PSK_WITH_CHACHA20_POLY1305_SHA256 = 52395,        //表示 TLS_PSK_WITH_CHACHA20_POLY1305_SHA256 密码套件
        TLS_PSK_WITH_NULL_SHA = 44,        //表示 TLS_PSK_WITH_NULL_SHA 密码套件
        TLS_PSK_WITH_NULL_SHA256 = 176,        //表示 TLS_PSK_WITH_NULL_SHA256 密码套件
        TLS_PSK_WITH_NULL_SHA384 = 177,        //表示 TLS_PSK_WITH_NULL_SHA384 密码套件
        TLS_PSK_WITH_RC4_128_SHA = 138,        //表示 TLS_PSK_WITH_RC4_128_SHA 密码套件
        TLS_RSA_EXPORT_WITH_DES40_CBC_SHA = 8,        //表示 TLS_RSA_EXPORT_WITH_DES40_CBC_SHA 密码套件
        TLS_RSA_EXPORT_WITH_RC2_CBC_40_MD5 = 6,        //表示 TLS_RSA_EXPORT_WITH_RC2_CBC_40_MD5 密码套件
        TLS_RSA_EXPORT_WITH_RC4_40_MD5 = 3,        //表示 TLS_RSA_EXPORT_WITH_RC4_40_MD5 密码套件
        TLS_RSA_PSK_WITH_3DES_EDE_CBC_SHA = 147,        //表示 TLS_RSA_PSK_WITH_3DES_EDE_CBC_SHA 密码套件
        TLS_RSA_PSK_WITH_AES_128_CBC_SHA = 148,        //表示 TLS_RSA_PSK_WITH_AES_128_CBC_SHA 密码套件
        TLS_RSA_PSK_WITH_AES_128_CBC_SHA256 = 182,        //表示 TLS_RSA_PSK_WITH_AES_128_CBC_SHA256 密码套件
        TLS_RSA_PSK_WITH_AES_128_GCM_SHA256 = 172,        //表示 TLS_RSA_PSK_WITH_AES_128_GCM_SHA256 密码套件
        TLS_RSA_PSK_WITH_AES_256_CBC_SHA = 149,        //表示 TLS_RSA_PSK_WITH_AES_256_CBC_SHA 密码套件
        TLS_RSA_PSK_WITH_AES_256_CBC_SHA384 = 183,        //表示 TLS_RSA_PSK_WITH_AES_256_CBC_SHA384 密码套件
        TLS_RSA_PSK_WITH_AES_256_GCM_SHA384 = 173,        //表示 TLS_RSA_PSK_WITH_AES_256_GCM_SHA384 密码套件
        TLS_RSA_PSK_WITH_ARIA_128_CBC_SHA256 = 49256,        //表示 TLS_RSA_PSK_WITH_ARIA_128_CBC_SHA256 密码套件
        TLS_RSA_PSK_WITH_ARIA_128_GCM_SHA256 = 49262,        //表示 TLS_RSA_PSK_WITH_ARIA_128_GCM_SHA256 密码套件
        TLS_RSA_PSK_WITH_ARIA_256_CBC_SHA384 = 49257,        //表示 TLS_RSA_PSK_WITH_ARIA_256_CBC_SHA384 密码套件
        TLS_RSA_PSK_WITH_ARIA_256_GCM_SHA384 = 49263,        //表示 TLS_RSA_PSK_WITH_ARIA_256_GCM_SHA384 密码套件
        TLS_RSA_PSK_WITH_CAMELLIA_128_CBC_SHA256 = 49304,        //表示 TLS_RSA_PSK_WITH_CAMELLIA_128_CBC_SHA256 密码套件
        TLS_RSA_PSK_WITH_CAMELLIA_128_GCM_SHA256 = 49298,        //表示 TLS_RSA_PSK_WITH_CAMELLIA_128_GCM_SHA256 密码套件
        TLS_RSA_PSK_WITH_CAMELLIA_256_CBC_SHA384 = 49305,        //表示 TLS_RSA_PSK_WITH_CAMELLIA_256_CBC_SHA384 密码套件
        TLS_RSA_PSK_WITH_CAMELLIA_256_GCM_SHA384 = 49299,        //表示 TLS_RSA_PSK_WITH_CAMELLIA_256_GCM_SHA384 密码套件
        TLS_RSA_PSK_WITH_CHACHA20_POLY1305_SHA256 = 52398,        //表示 TLS_RSA_PSK_WITH_CHACHA20_POLY1305_SHA256 密码套件
        TLS_RSA_PSK_WITH_NULL_SHA = 46,        //表示 TLS_RSA_PSK_WITH_NULL_SHA 密码套件
        TLS_RSA_PSK_WITH_NULL_SHA256 = 184,        //表示 TLS_RSA_PSK_WITH_NULL_SHA256 密码套件
        TLS_RSA_PSK_WITH_NULL_SHA384 = 185,        //表示 TLS_RSA_PSK_WITH_NULL_SHA384 密码套件
        TLS_RSA_PSK_WITH_RC4_128_SHA = 146,        //表示 TLS_RSA_PSK_WITH_RC4_128_SHA 密码套件
        TLS_RSA_WITH_3DES_EDE_CBC_SHA = 10,        //表示 TLS_RSA_WITH_3DES_EDE_CBC_SHA 密码套件
        TLS_RSA_WITH_AES_128_CBC_SHA = 47,        //表示 TLS_RSA_WITH_AES_128_CBC_SHA 密码套件
        TLS_RSA_WITH_AES_128_CBC_SHA256 = 60,        //表示 TLS_RSA_WITH_AES_128_CBC_SHA256 密码套件
        TLS_RSA_WITH_AES_128_CCM = 49308,        //表示 TLS_RSA_WITH_AES_128_CCM 密码套件
        TLS_RSA_WITH_AES_128_CCM_8 = 49312,        //表示 TLS_RSA_WITH_AES_128_CCM_8 密码套件
        TLS_RSA_WITH_AES_128_GCM_SHA256 = 156,        //表示 TLS_RSA_WITH_AES_128_GCM_SHA256 密码套件
        TLS_RSA_WITH_AES_256_CBC_SHA = 53,        //表示 TLS_RSA_WITH_AES_256_CBC_SHA 密码套件
        TLS_RSA_WITH_AES_256_CBC_SHA256 = 61,        //表示 TLS_RSA_WITH_AES_256_CBC_SHA256 密码套件
        TLS_RSA_WITH_AES_256_CCM = 49309,        //表示 TLS_RSA_WITH_AES_256_CCM 密码套件
        TLS_RSA_WITH_AES_256_CCM_8 = 49313,        //表示 TLS_RSA_WITH_AES_256_CCM_8 密码套件
        TLS_RSA_WITH_AES_256_GCM_SHA384 = 157,        //表示 TLS_RSA_WITH_AES_256_GCM_SHA384 密码套件
        TLS_RSA_WITH_ARIA_128_CBC_SHA256 = 49212,        //表示 TLS_RSA_WITH_ARIA_128_CBC_SHA256 密码套件
        TLS_RSA_WITH_ARIA_128_GCM_SHA256 = 49232,        //表示 TLS_RSA_WITH_ARIA_128_GCM_SHA256 密码套件
        TLS_RSA_WITH_ARIA_256_CBC_SHA384 = 49213,        //表示 TLS_RSA_WITH_ARIA_256_CBC_SHA384 密码套件
        TLS_RSA_WITH_ARIA_256_GCM_SHA384 = 49233,        //表示 TLS_RSA_WITH_ARIA_256_GCM_SHA384 密码套件
        TLS_RSA_WITH_CAMELLIA_128_CBC_SHA = 65,        //表示 TLS_RSA_WITH_CAMELLIA_128_CBC_SHA 密码套件
        TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256 = 186,        //表示 TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256 密码套件
        TLS_RSA_WITH_CAMELLIA_128_GCM_SHA256 = 49274,        //表示 TLS_RSA_WITH_CAMELLIA_128_GCM_SHA256 密码套件
        TLS_RSA_WITH_CAMELLIA_256_CBC_SHA = 132,        //表示 TLS_RSA_WITH_CAMELLIA_256_CBC_SHA 密码套件
        TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256 = 192,        //表示 TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256 密码套件
        TLS_RSA_WITH_CAMELLIA_256_GCM_SHA384 = 49275,        //表示 TLS_RSA_WITH_CAMELLIA_256_GCM_SHA384 密码套件
        TLS_RSA_WITH_DES_CBC_SHA = 9,        //表示 TLS_RSA_WITH_DES_CBC_SHA 密码套件
        TLS_RSA_WITH_IDEA_CBC_SHA = 7,        //表示 TLS_RSA_WITH_IDEA_CBC_SHA 密码套件
        TLS_RSA_WITH_NULL_MD5 = 1,        //表示 TLS_RSA_WITH_NULL_MD5 密码套件
        TLS_RSA_WITH_NULL_SHA = 2,        //表示 TLS_RSA_WITH_NULL_SHA 密码套件
        TLS_RSA_WITH_NULL_SHA256 = 59,        //表示 TLS_RSA_WITH_NULL_SHA256 密码套件
        TLS_RSA_WITH_RC4_128_MD5 = 4,        //表示 TLS_RSA_WITH_RC4_128_MD5 密码套件
        TLS_RSA_WITH_RC4_128_SHA = 5,        //表示 TLS_RSA_WITH_RC4_128_SHA 密码套件
        TLS_RSA_WITH_SEED_CBC_SHA = 150,        //表示 TLS_RSA_WITH_SEED_CBC_SHA 密码套件
        TLS_SRP_SHA_DSS_WITH_3DES_EDE_CBC_SHA = 49180,        //表示 TLS_SRP_SHA_DSS_WITH_3DES_EDE_CBC_SHA 密码套件
        TLS_SRP_SHA_DSS_WITH_AES_128_CBC_SHA = 49183,        //表示 TLS_SRP_SHA_DSS_WITH_AES_128_CBC_SHA 密码套件
        TLS_SRP_SHA_DSS_WITH_AES_256_CBC_SHA = 49186,        //表示 TLS_SRP_SHA_DSS_WITH_AES_256_CBC_SHA 密码套件
        TLS_SRP_SHA_RSA_WITH_3DES_EDE_CBC_SHA = 49179,        //表示 TLS_SRP_SHA_RSA_WITH_3DES_EDE_CBC_SHA 密码套件
        TLS_SRP_SHA_RSA_WITH_AES_128_CBC_SHA = 49182,        //表示 TLS_SRP_SHA_RSA_WITH_AES_128_CBC_SHA 密码套件
        TLS_SRP_SHA_RSA_WITH_AES_256_CBC_SHA = 49185,        //表示 TLS_SRP_SHA_RSA_WITH_AES_256_CBC_SHA 密码套件
        TLS_SRP_SHA_WITH_3DES_EDE_CBC_SHA = 49178,        //表示 TLS_SRP_SHA_WITH_3DES_EDE_CBC_SHA 密码套件
        TLS_SRP_SHA_WITH_AES_128_CBC_SHA = 49181,        //表示 TLS_SRP_SHA_WITH_AES_128_CBC_SHA 密码套件
        TLS_SRP_SHA_WITH_AES_256_CBC_SHA = 49184,        //表示 TLS_SRP_SHA_WITH_AES_256_CBC_SHA 密码套件
    }
}
