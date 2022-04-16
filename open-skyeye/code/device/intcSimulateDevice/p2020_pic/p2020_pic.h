#ifndef __P2020_PIC_H__
#define __P2020_PIC_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "BBR1",
    "BBR2",
    "IPIDR0",
    "IPIDR1",
    "IPIDR2",
    "IPIDR3",
    "CTPR",
    "WHOAMI",
    "IACK",
    "EOI",
    "FRR",
    "GCR",
    "VIR",
    "PIR",
    "IPIVPR0",
    "IPIVPR1",
    "IPIVPR2",
    "IPIVPR3",
    "SVR",
    "TFRR0",
    "TFRR1",
    "GTCCRA0",
    "GTCCRA1",
    "GTCCRA2",
    "GTCCRA3",
    "GTBCRA0",
    "GTBCRA1",
    "GTBCRA2",
    "GTBCRA3",
    "GTVPRA0",
    "GTVPRA1",
    "GTVPRA2",
    "GTVPRA3",
    "GTDRA0",
    "GTDRA1",
    "GTDRA2",
    "GTDRA3",
    "TCR0",
    "TCR1",
    "ERQSR",
    "IRQSR0",
    "IRQSR1",
    "IRQSR2",
    "CISR0",
    "CISR1",
    "CISR2",
    "PMMR00",
    "PMMR01",
    "PMMR02",
    "PMMR03",
    "PMMR10",
    "PMMR11",
    "PMMR12",
    "PMMR13",
    "PMMR20",
    "PMMR21",
    "PMMR22",
    "PMMR23",
    "MSGR0",
    "MSGR1",
    "MSGR2",
    "MSGR3",
    "MER",
    "MSR",
    "MSIR0",
    "MSIR1",
    "MSIR2",
    "MSIR3",
    "MSIR4",
    "MSIR5",
    "MSIR6",
    "MSIR7",
    "MSISR",
    "MSIIR",
    "GTCCRB0",
    "GTCCRB1",
    "GTCCRB2",
    "GTCCRB3",
    "GTBCRB0",
    "GTBCRB1",
    "GTBCRB2",
    "GTBCRB3",
    "GTVPRB0",
    "GTVPRB1",
    "GTVPRB2",
    "GTVPRB3",
    "GTDRB0",
    "GTDRB1",
    "GTDRB2",
    "GTDRB3",
    "MSGRa0",
    "MSGRa1",
    "MSGRa2",
    "MSGRa3",
    "MERa",
    "MSRa",
    "EIVPR0",
    "EIVPR1",
    "EIVPR2",
    "EIVPR3",
    "EIVPR4",
    "EIVPR5",
    "EIVPR6",
    "EIVPR7",
    "EIVPR8",
    "EIVPR9",
    "EIVPR10",
    "EIVPR11",
    "EIDR0",
    "EIDR1",
    "EIDR2",
    "EIDR3",
    "EIDR4",
    "EIDR5",
    "EIDR6",
    "EIDR7",
    "EIDR8",
    "EIDR9",
    "EIDR10",
    "EIDR11",
    "IIVPR0",
    "IIVPR1",
    "IIVPR2",
    "IIVPR3",
    "IIVPR4",
    "IIVPR5",
    "IIVPR6",
    "IIVPR7",
    "IIVPR8",
    "IIVPR9",
    "IIVPR10",
    "IIVPR11",
    "IIVPR12",
    "IIVPR13",
    "IIVPR14",
    "IIVPR15",
    "IIVPR16",
    "IIVPR17",
    "IIVPR18",
    "IIVPR19",
    "IIVPR20",
    "IIVPR21",
    "IIVPR22",
    "IIVPR23",
    "IIVPR24",
    "IIVPR25",
    "IIVPR26",
    "IIVPR27",
    "IIVPR28",
    "IIVPR29",
    "IIVPR30",
    "IIVPR31",
    "IIVPR32",
    "IIVPR33",
    "IIVPR34",
    "IIVPR35",
    "IIVPR36",
    "IIVPR37",
    "IIVPR38",
    "IIVPR39",
    "IIVPR40",
    "IIVPR41",
    "IIVPR42",
    "IIVPR43",
    "IIVPR44",
    "IIVPR45",
    "IIVPR46",
    "IIVPR47",
    "IIVPR48",
    "IIVPR49",
    "IIVPR50",
    "IIVPR51",
    "IIVPR52",
    "IIVPR53",
    "IIVPR54",
    "IIVPR55",
    "IIVPR56",
    "IIVPR57",
    "IIVPR58",
    "IIVPR59",
    "IIVPR60",
    "IIVPR61",
    "IIVPR62",
    "IIVPR63",
    "IIDR0",
    "IIDR1",
    "IIDR2",
    "IIDR3",
    "IIDR4",
    "IIDR5",
    "IIDR6",
    "IIDR7",
    "IIDR8",
    "IIDR9",
    "IIDR10",
    "IIDR11",
    "IIDR12",
    "IIDR13",
    "IIDR14",
    "IIDR15",
    "IIDR16",
    "IIDR17",
    "IIDR18",
    "IIDR19",
    "IIDR20",
    "IIDR21",
    "IIDR22",
    "IIDR23",
    "IIDR24",
    "IIDR25",
    "IIDR26",
    "IIDR27",
    "IIDR28",
    "IIDR29",
    "IIDR30",
    "IIDR31",
    "IIDR32",
    "IIDR33",
    "IIDR34",
    "IIDR35",
    "IIDR36",
    "IIDR37",
    "IIDR38",
    "IIDR39",
    "IIDR40",
    "IIDR41",
    "IIDR42",
    "IIDR43",
    "IIDR44",
    "IIDR45",
    "IIDR46",
    "IIDR47",
    "IIDR48",
    "IIDR49",
    "IIDR50",
    "IIDR51",
    "IIDR52",
    "IIDR53",
    "IIDR54",
    "IIDR55",
    "IIDR56",
    "IIDR57",
    "IIDR58",
    "IIDR59",
    "IIDR60",
    "IIDR61",
    "IIDR62",
    "IIDR63",
    "MIVPR0",
    "MIVPR1",
    "MIVPR2",
    "MIVPR3",
    "MIVPR4",
    "MIVPR5",
    "MIVPR6",
    "MIVPR7",
    "MIDR0",
    "MIDR1",
    "MIDR2",
    "MIDR3",
    "MIDR4",
    "MIDR5",
    "MIDR6",
    "MIDR7",
    "MSIVPR0",
    "MSIVPR1",
    "MSIVPR2",
    "MSIVPR3",
    "MSIVPR4",
    "MSIVPR5",
    "MSIVPR6",
    "MSIVPR7",
    "MSIDR0",
    "MSIDR1",
    "MSIDR2",
    "MSIDR3",
    "MSIDR4",
    "MSIDR5",
    "MSIDR6",
    "MSIDR7",
    "IPIDR_CPU00",
    "IPIDR_CPU01",
    "IPIDR_CPU02",
    "IPIDR_CPU03",
    "CTPR_CPU0",
    "WHOAMI_CPU0",
    "IACK_CPU0",
    "EOI_CPU0",
    "IPIDR_CPU10",
    "IPIDR_CPU11",
    "IPIDR_CPU12",
    "IPIDR_CPU13",
    "CTPR_CPU1",
    "WHOAMI_CPU1",
    "IACK_CPU1",
    "EOI_CPU1",
    NULL
};

// 寄存器偏移地址
#define _BBR1 0x0
#define _BBR2 0x10
#define _IPIDR0 0x40
#define _IPIDR1 0x50
#define _IPIDR2 0x60
#define _IPIDR3 0x70
#define _CTPR 0x80
#define _WHOAMI 0x90
#define _IACK 0xA0
#define _EOI 0xB0
#define _FRR 0x1000
#define _GCR 0x1020
#define _VIR 0x1080
#define _PIR 0x1090
#define _IPIVPR0 0x10A0
#define _IPIVPR1 0x10B0
#define _IPIVPR2 0x10C0
#define _IPIVPR3 0x10D0
#define _SVR 0x10E0
#define _TFRR0 0x20F0
#define _TFRR1 0x30F0
#define _GTCCRA0 0x1100
#define _GTCCRA1 0x1140
#define _GTCCRA2 0x1180
#define _GTCCRA3 0x11C0
#define _GTBCRA0 0x1110 //
#define _GTBCRA1 0x1150
#define _GTBCRA2 0x1190
#define _GTBCRA3 0x11D0
#define _GTVPRA0 0x1120 //
#define _GTVPRA1 0x1160
#define _GTVPRA2 0x11A0 //
#define _GTVPRA3 0x11E0
#define _GTDRA0 0x1130
#define _GTDRA1 0x1170
#define _GTDRA2 0x11B0 //
#define _GTDRA3 0x11F0
#define _TCR0 0x1300
#define _TCR1 0x2300
#define _ERQSR 0x1308
#define _IRQSR0 0x1310
#define _IRQSR1 0x1320
#define _IRQSR2 0x1324
#define _CISR0 0x1330
#define _CISR1 0x1340
#define _CISR2 0x1344
#define _PMMR00 0x1350
#define _PMMR01 0x1370
#define _PMMR02 0x1390
#define _PMMR03 0x13B0
#define _PMMR10 0x1360
#define _PMMR11 0x1380
#define _PMMR12 0x13A0
#define _PMMR13 0x13C0
#define _PMMR20 0x1364
#define _PMMR21 0x1384
#define _PMMR22 0x13A4
#define _PMMR23 0x13C4
#define _MSGR0 0x1400
#define _MSGR1 0x1410
#define _MSGR2 0x1420
#define _MSGR3 0x1430
#define _MER 0x1500
#define _MSR 0x1510
#define _MSIR0 0x1600
#define _MSIR1 0x1610
#define _MSIR2 0x1620
#define _MSIR3 0x1630
#define _MSIR4 0x1640
#define _MSIR5 0x1650
#define _MSIR6 0x1660
#define _MSIR7 0x1670
#define _MSISR 0x1720
#define _MSIIR 0x1740
#define _GTCCRB0 0x2100
#define _GTCCRB1 0x2140
#define _GTCCRB2 0x2180
#define _GTCCRB3 0x21C0
#define _GTBCRB0 0x2110
#define _GTBCRB1 0x2150
#define _GTBCRB2 0x2190
#define _GTBCRB3 0x21D0
#define _GTVPRB0 0x2120
#define _GTVPRB1 0x2160
#define _GTVPRB2 0x21A0
#define _GTVPRB3 0x21E0
#define _GTDRB0 0x2130
#define _GTDRB1 0x2170
#define _GTDRB2 0x21B0
#define _GTDRB3 0x21F0
#define _MSGRA0 0x2400
#define _MSGRA1 0x2410
#define _MSGRA2 0x2420
#define _MSGRA3 0x2430
#define _MERA 0x2500
#define _MSRA 0x2510
#define _EIVPR0 0x10000
#define _EIVPR1 0x10020
#define _EIVPR2 0x10040
#define _EIVPR3 0x10060
#define _EIVPR4 0x10080
#define _EIVPR5 0x100A0
#define _EIVPR6 0x100C0
#define _EIVPR7 0x100E0
#define _EIVPR8 0x10100
#define _EIVPR9 0x10120
#define _EIVPR10 0x10140
#define _EIVPR11 0x10160
#define _EIDR0 0x10010
#define _EIDR1 0x10030
#define _EIDR2 0x10050
#define _EIDR3 0x10070
#define _EIDR4 0x10090
#define _EIDR5 0x100B0
#define _EIDR6 0x100D0
#define _EIDR7 0x100F0
#define _EIDR8 0x10110
#define _EIDR9 0x10130
#define _EIDR10 0x10150
#define _EIDR11 0x10170
#define _IIVPR0 0x10200
#define _IIVPR1 0x10220
#define _IIVPR2 0x10240
#define _IIVPR3 0x10260
#define _IIVPR4 0x10280
#define _IIVPR5 0x102A0
#define _IIVPR6 0x102C0
#define _IIVPR7 0x102E0
#define _IIVPR8 0x10300
#define _IIVPR9 0x10320
#define _IIVPR10 0x10340
#define _IIVPR11 0x10360
#define _IIVPR12 0x10380
#define _IIVPR13 0x103A0
#define _IIVPR14 0x103C0
#define _IIVPR15 0x103E0
#define _IIVPR16 0x10400
#define _IIVPR17 0x10420
#define _IIVPR18 0x10440
#define _IIVPR19 0x10460
#define _IIVPR20 0x10480
#define _IIVPR21 0x104A0
#define _IIVPR22 0x104C0
#define _IIVPR23 0x104E0
#define _IIVPR24 0x10500
#define _IIVPR25 0x10520
#define _IIVPR26 0x10540
#define _IIVPR27 0x10560
#define _IIVPR28 0x10580
#define _IIVPR29 0x105A0
#define _IIVPR30 0x105C0
#define _IIVPR31 0x105E0
#define _IIVPR32 0x10600
#define _IIVPR33 0x10620
#define _IIVPR34 0x10640
#define _IIVPR35 0x10660
#define _IIVPR36 0x10680
#define _IIVPR37 0x106A0
#define _IIVPR38 0x106C0
#define _IIVPR39 0x106E0
#define _IIVPR40 0x10700
#define _IIVPR41 0x10720
#define _IIVPR42 0x10740
#define _IIVPR43 0x10760
#define _IIVPR44 0x10780
#define _IIVPR45 0x107A0
#define _IIVPR46 0x107C0
#define _IIVPR47 0x107E0
#define _IIVPR48 0x10800
#define _IIVPR49 0x10820
#define _IIVPR50 0x10840
#define _IIVPR51 0x10860
#define _IIVPR52 0x10880
#define _IIVPR53 0x108A0
#define _IIVPR54 0x108C0
#define _IIVPR55 0x108E0
#define _IIVPR56 0x10900
#define _IIVPR57 0x10920
#define _IIVPR58 0x10940
#define _IIVPR59 0x10960
#define _IIVPR60 0x10980
#define _IIVPR61 0x109A0
#define _IIVPR62 0x109C0
#define _IIVPR63 0x109E0
#define _IIDR0 0x10210
#define _IIDR1 0x10230
#define _IIDR2 0x10250
#define _IIDR3 0x10270
#define _IIDR4 0x10290
#define _IIDR5 0x102B0
#define _IIDR6 0x102D0
#define _IIDR7 0x102F0
#define _IIDR8 0x10310
#define _IIDR9 0x10330
#define _IIDR10 0x10350
#define _IIDR11 0x10370
#define _IIDR12 0x10390
#define _IIDR13 0x103B0
#define _IIDR14 0x103D0
#define _IIDR15 0x103F0
#define _IIDR16 0x10410
#define _IIDR17 0x10430
#define _IIDR18 0x10450
#define _IIDR19 0x10470
#define _IIDR20 0x10490
#define _IIDR21 0x104B0
#define _IIDR22 0x104D0
#define _IIDR23 0x104F0
#define _IIDR24 0x10510
#define _IIDR25 0x10530
#define _IIDR26 0x10550
#define _IIDR27 0x10570
#define _IIDR28 0x10590
#define _IIDR29 0x105B0
#define _IIDR30 0x105D0
#define _IIDR31 0x105F0
#define _IIDR32 0x10610
#define _IIDR33 0x10630
#define _IIDR34 0x10650
#define _IIDR35 0x10670
#define _IIDR36 0x10690
#define _IIDR37 0x106B0
#define _IIDR38 0x106D0
#define _IIDR39 0x106F0
#define _IIDR40 0x10710
#define _IIDR41 0x10730
#define _IIDR42 0x10750
#define _IIDR43 0x10770
#define _IIDR44 0x10790
#define _IIDR45 0x107B0
#define _IIDR46 0x107D0
#define _IIDR47 0x107F0
#define _IIDR48 0x10810
#define _IIDR49 0x10830
#define _IIDR50 0x10850
#define _IIDR51 0x10870
#define _IIDR52 0x10890
#define _IIDR53 0x108B0
#define _IIDR54 0x108D0
#define _IIDR55 0x108F0
#define _IIDR56 0x10910
#define _IIDR57 0x10930
#define _IIDR58 0x10950
#define _IIDR59 0x10970
#define _IIDR60 0x10990
#define _IIDR61 0x109B0
#define _IIDR62 0x109D0
#define _IIDR63 0x109F0
#define _MIVPR0 0x11600
#define _MIVPR1 0x11620
#define _MIVPR2 0x11640
#define _MIVPR3 0x11660
#define _MIVPR4 0x11680
#define _MIVPR5 0x116A0
#define _MIVPR6 0x116C0
#define _MIVPR7 0x116E0
#define _MIDR0 0x11610
#define _MIDR1 0x11630
#define _MIDR2 0x11650
#define _MIDR3 0x11670
#define _MIDR4 0x11690
#define _MIDR5 0x116B0
#define _MIDR6 0x116D0
#define _MIDR7 0x116F0
#define _MSIVPR0 0x11C00
#define _MSIVPR1 0x11C20
#define _MSIVPR2 0x11C40
#define _MSIVPR3 0x11C60
#define _MSIVPR4 0x11C80
#define _MSIVPR5 0x11CA0
#define _MSIVPR6 0x11CC0
#define _MSIVPR7 0x11CE0
#define _MSIDR0 0x11C10
#define _MSIDR1 0x11C30
#define _MSIDR2 0x11C50
#define _MSIDR3 0x11C70
#define _MSIDR4 0x11C90
#define _MSIDR5 0x11CB0
#define _MSIDR6 0x11CD0
#define _MSIDR7 0x11CF0
#define _IPIDR_CPU00 0x20040
#define _IPIDR_CPU01 0x20050
#define _IPIDR_CPU02 0x20060
#define _IPIDR_CPU03 0x20070
#define _CTPR_CPU0 0x20080
#define _WHOAMI_CPU0 0x20090
#define _IACK_CPU0 0x200A0
#define _EOI_CPU0 0x200B0
#define _IPIDR_CPU10 0x21040
#define _IPIDR_CPU11 0x21050
#define _IPIDR_CPU12 0x21060
#define _IPIDR_CPU13 0x21070
#define _CTPR_CPU1 0x21080
#define _WHOAMI_CPU1 0x21090
#define _IACK_CPU1 0x210A0
#define _EOI_CPU1 0x210B0

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _BBR1,
    _BBR2,
    _IPIDR0,
    _IPIDR1,
    _IPIDR2,
    _IPIDR3,
    _CTPR,
    _WHOAMI,
    _IACK,
    _EOI,
    _FRR,
    _GCR,
    _VIR,
    _PIR,
    _IPIVPR0,
    _IPIVPR1,
    _IPIVPR2,
    _IPIVPR3,
    _SVR,
    _TFRR0,
    _TFRR1,
    _GTCCRA0,
    _GTCCRA1,
    _GTCCRA2,
    _GTCCRA3,
    _GTBCRA0,
    _GTBCRA1,
    _GTBCRA2,
    _GTBCRA3,
    _GTVPRA0,
    _GTVPRA1,
    _GTVPRA2,
    _GTVPRA3,
    _GTDRA0,
    _GTDRA1,
    _GTDRA2,
    _GTDRA3,
    _TCR0,
    _TCR1,
    _ERQSR,
    _IRQSR0,
    _IRQSR1,
    _IRQSR2,
    _CISR0,
    _CISR1,
    _CISR2,
    _PMMR00,
    _PMMR01,
    _PMMR02,
    _PMMR03,
    _PMMR10,
    _PMMR11,
    _PMMR12,
    _PMMR13,
    _PMMR20,
    _PMMR21,
    _PMMR22,
    _PMMR23,
    _MSGR0,
    _MSGR1,
    _MSGR2,
    _MSGR3,
    _MER,
    _MSR,
    _MSIR0,
    _MSIR1,
    _MSIR2,
    _MSIR3,
    _MSIR4,
    _MSIR5,
    _MSIR6,
    _MSIR7,
    _MSISR,
    _MSIIR,
    _GTCCRB0,
    _GTCCRB1,
    _GTCCRB2,
    _GTCCRB3,
    _GTBCRB0,
    _GTBCRB1,
    _GTBCRB2,
    _GTBCRB3,
    _GTVPRB0,
    _GTVPRB1,
    _GTVPRB2,
    _GTVPRB3,
    _GTDRB0,
    _GTDRB1,
    _GTDRB2,
    _GTDRB3,
    _MSGRA0,
    _MSGRA1,
    _MSGRA2,
    _MSGRA3,
    _MERA,
    _MSRA,
    _EIVPR0,
    _EIVPR1,
    _EIVPR2,
    _EIVPR3,
    _EIVPR4,
    _EIVPR5,
    _EIVPR6,
    _EIVPR7,
    _EIVPR8,
    _EIVPR9,
    _EIVPR10,
    _EIVPR11,
    _EIDR0,
    _EIDR1,
    _EIDR2,
    _EIDR3,
    _EIDR4,
    _EIDR5,
    _EIDR6,
    _EIDR7,
    _EIDR8,
    _EIDR9,
    _EIDR10,
    _EIDR11,
    _IIVPR0,
    _IIVPR1,
    _IIVPR2,
    _IIVPR3,
    _IIVPR4,
    _IIVPR5,
    _IIVPR6,
    _IIVPR7,
    _IIVPR8,
    _IIVPR9,
    _IIVPR10,
    _IIVPR11,
    _IIVPR12,
    _IIVPR13,
    _IIVPR14,
    _IIVPR15,
    _IIVPR16,
    _IIVPR17,
    _IIVPR18,
    _IIVPR19,
    _IIVPR20,
    _IIVPR21,
    _IIVPR22,
    _IIVPR23,
    _IIVPR24,
    _IIVPR25,
    _IIVPR26,
    _IIVPR27,
    _IIVPR28,
    _IIVPR29,
    _IIVPR30,
    _IIVPR31,
    _IIVPR32,
    _IIVPR33,
    _IIVPR34,
    _IIVPR35,
    _IIVPR36,
    _IIVPR37,
    _IIVPR38,
    _IIVPR39,
    _IIVPR40,
    _IIVPR41,
    _IIVPR42,
    _IIVPR43,
    _IIVPR44,
    _IIVPR45,
    _IIVPR46,
    _IIVPR47,
    _IIVPR48,
    _IIVPR49,
    _IIVPR50,
    _IIVPR51,
    _IIVPR52,
    _IIVPR53,
    _IIVPR54,
    _IIVPR55,
    _IIVPR56,
    _IIVPR57,
    _IIVPR58,
    _IIVPR59,
    _IIVPR60,
    _IIVPR61,
    _IIVPR62,
    _IIVPR63,
    _IIDR0,
    _IIDR1,
    _IIDR2,
    _IIDR3,
    _IIDR4,
    _IIDR5,
    _IIDR6,
    _IIDR7,
    _IIDR8,
    _IIDR9,
    _IIDR10,
    _IIDR11,
    _IIDR12,
    _IIDR13,
    _IIDR14,
    _IIDR15,
    _IIDR16,
    _IIDR17,
    _IIDR18,
    _IIDR19,
    _IIDR20,
    _IIDR21,
    _IIDR22,
    _IIDR23,
    _IIDR24,
    _IIDR25,
    _IIDR26,
    _IIDR27,
    _IIDR28,
    _IIDR29,
    _IIDR30,
    _IIDR31,
    _IIDR32,
    _IIDR33,
    _IIDR34,
    _IIDR35,
    _IIDR36,
    _IIDR37,
    _IIDR38,
    _IIDR39,
    _IIDR40,
    _IIDR41,
    _IIDR42,
    _IIDR43,
    _IIDR44,
    _IIDR45,
    _IIDR46,
    _IIDR47,
    _IIDR48,
    _IIDR49,
    _IIDR50,
    _IIDR51,
    _IIDR52,
    _IIDR53,
    _IIDR54,
    _IIDR55,
    _IIDR56,
    _IIDR57,
    _IIDR58,
    _IIDR59,
    _IIDR60,
    _IIDR61,
    _IIDR62,
    _IIDR63,
    _MIVPR0,
    _MIVPR1,
    _MIVPR2,
    _MIVPR3,
    _MIVPR4,
    _MIVPR5,
    _MIVPR6,
    _MIVPR7,
    _MIDR0,
    _MIDR1,
    _MIDR2,
    _MIDR3,
    _MIDR4,
    _MIDR5,
    _MIDR6,
    _MIDR7,
    _MSIVPR0,
    _MSIVPR1,
    _MSIVPR2,
    _MSIVPR3,
    _MSIVPR4,
    _MSIVPR5,
    _MSIVPR6,
    _MSIVPR7,
    _MSIDR0,
    _MSIDR1,
    _MSIDR2,
    _MSIDR3,
    _MSIDR4,
    _MSIDR5,
    _MSIDR6,
    _MSIDR7,
    _IPIDR_CPU00,
    _IPIDR_CPU01,
    _IPIDR_CPU02,
    _IPIDR_CPU03,
    _CTPR_CPU0,
    _WHOAMI_CPU0,
    _IACK_CPU0,
    _EOI_CPU0,
    _IPIDR_CPU10,
    _IPIDR_CPU11,
    _IPIDR_CPU12,
    _IPIDR_CPU13,
    _CTPR_CPU1,
    _WHOAMI_CPU1,
    _IACK_CPU1,
    _EOI_CPU1,

};

// 设备寄存器结构体
struct registers
{
    uint32_t BBR1;
    uint32_t BBR2;
    uint32_t IPIDR[4];
    uint32_t CTPR;
    uint32_t WHOAMI;
    uint32_t IACK;
    uint32_t EOI;
    uint32_t FRR;
    uint32_t GCR;
    uint32_t VIR;
    uint32_t PIR;
    uint32_t IPIVPR[4];
    uint32_t SVR;
    uint32_t TFRR0;
    uint32_t TFRR1;
    uint32_t GTCCRA[4];
    uint32_t GTBCRA[4];
    uint32_t GTVPRA[4];
    uint32_t GTDRA[4];
    uint32_t TCR0;
    uint32_t TCR1;
    uint32_t ERQSR;
    uint32_t IRQSR0;
    uint32_t IRQSR1;
    uint32_t IRQSR2;
    uint32_t CISR0;
    uint32_t CISR1;
    uint32_t CISR2;
    uint32_t PMMR0[4];
    uint32_t PMMR1[4];
    uint32_t PMMR2[4];
    uint32_t MSGR[4];
    uint32_t MER;
    uint32_t MSR;
    uint32_t MSIR[8];
    uint32_t MSISR;
    uint32_t MSIIR;
    uint32_t GTCCRB[4];
    uint32_t GTBCRB[4];
    uint32_t GTVPRB[4];
    uint32_t GTDRB[4];
    uint32_t MSGRa[4];
    uint32_t MERa;
    uint32_t MSRa;
    uint32_t EIVPR[12];
    uint32_t EIDR[12];
    uint32_t IIVPR[64];
    uint32_t IIDR[64];
    uint32_t MIVPR[8];
    uint32_t MIDR[8];
    uint32_t MSIVPR[8];
    uint32_t MSIDR[8];
    struct
    {
        uint32_t IPIDR[4];
        uint32_t CTPR;
        uint32_t WHOAMI;
        uint32_t IACK;
        uint32_t EOI;
    } CPU[2];
};

// 处理机中断信号接口
struct core_signal_s
{
    conf_object_t* obj;
    core_signal_intf* iface;
};

// 内存地址总线接口
struct memory_space_s
{
    conf_object_t* obj;
    memory_space_iface_t* iface;
};

// 设备对象结构
struct p2020_pic_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;
    struct core_signal_s core_signal[2];
    struct memory_space_s memory_space;
    struct registers regs;
};

typedef struct p2020_pic_device_t p2020_pic_device;

#define P0 0x1
#define P1 0x2

#define Register_Address_Interval (0x10)

#define PIC_RESET_BIT (1 << 31)

#define   VPR_ACTIVE_BIT_MASK               0x40000000
#define   VECTOR_PIT_GROUPA_GPTIMER_0       76 /* 0x4c */
#define   TICKCLOCK_PERIOD                  1000
#endif
