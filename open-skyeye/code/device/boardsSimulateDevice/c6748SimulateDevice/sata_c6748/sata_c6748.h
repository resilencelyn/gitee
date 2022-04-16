#ifndef __SATA_C6748_H__ 
#define __SATA_C6748_H__ 

typedef struct reg_cap{
    uint32_t np             :5;
    uint32_t sxs            :1;
    uint32_t emx            :1;
    uint32_t cccs           :1;
    uint32_t ncs            :5;
    uint32_t psc            :1;
    uint32_t ssc            :1;
    uint32_t pmd            :1;
    uint32_t reserved       :1;
    uint32_t spm            :1;
    uint32_t sam            :1;
    uint32_t snzo           :1;
    uint32_t iss            :4;
    uint32_t sclo           :1;
    uint32_t sal            :1;
    uint32_t salp           :1;
    uint32_t sss            :1;
    uint32_t smps           :1;
    uint32_t ssntf          :1;
    uint32_t sncq           :1;
    uint32_t s64a           :1;
}reg_cap_t;
union cap_reg{
    uint32_t        all;
    reg_cap_t       bit;
};

typedef struct reg_ghc{
    uint32_t hr             :1;
    uint32_t ie             :1;
    uint32_t reserved       :29;
    uint32_t ae             :1;
}reg_ghc_t;
union ghc_reg{
    uint32_t        all;
    reg_ghc_t       bit;
};

typedef struct reg_is{
    uint32_t ips            :2;
    uint32_t reserved       :30;
}reg_is_t;
union is_reg{
    uint32_t       all;
    reg_is_t       bit;
};

typedef struct reg_pi{
    uint32_t pi             :2;
    uint32_t reserved       :30;
}reg_pi_t;
union pi_reg{
    uint32_t       all;
    reg_pi_t       bit;
};

typedef struct reg_vs{
    uint32_t mnr            :16;
    uint32_t mjr            :16;
}reg_vs_t;
union vs_reg{
    uint32_t       all;
    reg_vs_t       bit;
};

typedef struct reg_ccc_ctl{
    uint32_t en             :1;
    uint32_t reserved       :2;
    uint32_t inter          :5;
    uint32_t cc             :8;
    uint32_t tv             :16;
}reg_ccc_ctl_t;
union ccc_ctl_reg{
    uint32_t       all;
    reg_ccc_ctl_t  bit;
};

typedef struct reg_bistafr{
    uint32_t pd             :8;
    uint32_t ncp            :8;
    uint32_t reserved       :16;
}reg_bistafr_t;
union bistafr_reg{
    uint32_t       all;
    reg_bistafr_t  bit;
};

typedef struct reg_bistcr{
    uint32_t pattern        :4;
    uint32_t pv             :1;
    uint32_t flip           :1;
    uint32_t erren          :1;
    uint32_t reserved_1     :1;
    uint32_t llc            :3;
    uint32_t reserved_2     :5;
    uint32_t nealb          :1;
    uint32_t cntclr         :1;
    uint32_t txo            :1;
    uint32_t reserved_3     :13;
}reg_bistcr_t;
union bistcr_reg{
    uint32_t       all;
    reg_bistcr_t  bit;
};

typedef struct reg_bistsr{
    uint32_t framerr        :16;
    uint32_t brsterr        :8;
    uint32_t reserved       :8;
}reg_bistsr_t;
union bistsr_reg{
    uint32_t       all;
    reg_bistsr_t  bit;
};

typedef struct reg_timer1ms{
    uint32_t timv           :20;
    uint32_t reserved       :12;
}reg_timer1ms_t;
union timer1ms_reg{
    uint32_t       all;
    reg_timer1ms_t  bit;
};

typedef struct reg_gparam1r{
    uint32_t m_hdara        :3;
    uint32_t s_hdata        :3;
    uint32_t m_haddr        :1;
    uint32_t s_haddr        :1;
    uint32_t ahb_endian     :2;
    uint32_t return_err     :1;
    uint32_t reserved       :1;
    uint32_t phy_type       :1;
    uint32_t bist_m         :1;
    uint32_t latch_m        :1;
    uint32_t phy_stat       :6;
    uint32_t phy_ctrl       :6;
    uint32_t phy_rst        :1;
    uint32_t phy_data       :2;
    uint32_t rx_buffer      :1;
    uint32_t align_m        :1;
}reg_gparam1r_t;
union gparam1r_reg{
    uint32_t       all;
    reg_gparam1r_t  bit;
};

typedef struct reg_gparam2r{
    uint32_t bxoob_clk      :9;
    uint32_t tx_oob_m       :1;
    uint32_t bxoob_m        :1;
    uint32_t bxoob_clk_m    :1;
    uint32_t encode_m       :1;
    uint32_t dev_mp         :1;
    uint32_t dev_cp         :1;
    uint32_t reserved       :17;
}reg_gparam2r_t;
union gparam2r_reg{
    uint32_t       all;
    reg_gparam2r_t  bit;
};

typedef struct reg_pparamr{
    uint32_t rx_fifo_depth  :3;
    uint32_t x_fifo_depth   :3;
    uint32_t rx_mem_s       :1;
    uint32_t rx_mem_m       :1;
    uint32_t tx_mem_s       :1;
    uint32_t tx_mem_m       :1;
    uint32_t reserved       :22;
}reg_pparamr_t;
union pparamr_reg{
    uint32_t       all;
    reg_pparamr_t  bit;
};

typedef struct reg_testr{
    uint32_t test_if        :1;
    uint32_t reserved_1     :15;
    uint32_t psel           :3;
    uint32_t reserved_2     :13;
}reg_testr_t;
union testr_reg{
    uint32_t       all;
    reg_testr_t  bit;
};

typedef struct reg_p0clb{
    uint32_t reserved       :10;
    uint32_t clb            :22;
}reg_p0clb_t;
union p0clb_reg{
    uint32_t       all;
    reg_p0clb_t  bit;
};

typedef struct reg_p0fb{
    uint32_t reserved       :8;
    uint32_t fb             :24;
}reg_p0fb_t;
union p0fb_reg{
    uint32_t       all;
    reg_p0fb_t  bit;
};

typedef struct reg_p0is{
    uint32_t dhrs           :1;
    uint32_t pss            :1;
    uint32_t dss            :1;
    uint32_t sdbs           :1;
    uint32_t ufs            :1;
    uint32_t dps            :1;
    uint32_t pcs            :1;
    uint32_t dmps           :1;
    uint32_t reserved_1     :14;
    uint32_t prcs           :1;
    uint32_t ipms           :1;
    uint32_t ofs            :1;
    uint32_t reserved_2     :1;
    uint32_t infs           :1;
    uint32_t ifs            :1;
    uint32_t hbds           :1;
    uint32_t hbfs           :1;
    uint32_t tfes           :1;
    uint32_t cpds           :1;
}reg_p0is_t;
union p0is_reg{
    uint32_t       all;
    reg_p0is_t  bit;
};

typedef struct reg_p0ie{
    uint32_t dhre           :1;
    uint32_t pse            :1;
    uint32_t dse            :1;
    uint32_t sdbe           :1;
    uint32_t ufe            :1;
    uint32_t dpe            :1;
    uint32_t pce            :1;
    uint32_t dmpe           :1;
    uint32_t reserved_1     :14;
    uint32_t prce           :1;
    uint32_t ipme           :1;
    uint32_t ofe            :1;
    uint32_t reserved_2     :1;
    uint32_t infe           :1;
    uint32_t ife            :1;
    uint32_t hbde           :1;
    uint32_t hbfe           :1;
    uint32_t tfee           :1;
    uint32_t cpde           :1;
}reg_p0ie_t;
union p0ie_reg{
    uint32_t       all;
    reg_p0ie_t  bit;
};

typedef struct reg_p0cmd{
    uint32_t st             :1;
    uint32_t sud            :1;
    uint32_t pod            :1;
    uint32_t clo            :1;
    uint32_t fre            :1;
    uint32_t reserved_1     :3;
    uint32_t ccs            :5;
    uint32_t mpss           :1;
    uint32_t fr             :1;
    uint32_t cr             :1;
    uint32_t cps            :1;
    uint32_t pma            :1;
    uint32_t hpcp           :1;
    uint32_t mpsp           :1;
    uint32_t cpd            :1;
    uint32_t esp            :1;
    uint32_t reserved_2     :2;
    uint32_t atapi          :1;
    uint32_t dlae           :1;
    uint32_t alpe           :1;
    uint32_t asp            :1;
    uint32_t icc            :4;
}reg_p0cmd_t;
union p0cmd_reg{
    uint32_t       all;
    reg_p0cmd_t  bit;
};

typedef struct reg_p0tfd{
    uint32_t sts            :8;
    uint32_t err            :8;
    uint32_t reserved       :16;
}reg_p0tfd_t;
union p0tfd_reg{
    uint32_t       all;
    reg_p0tfd_t  bit;
};

typedef struct reg_p0ssts{
    uint32_t det            :4;
    uint32_t spd            :4;
    uint32_t ipm            :4;
    uint32_t reserved       :20;
}reg_p0ssts_t;
union p0ssts_reg{
    uint32_t       all;
    reg_p0ssts_t  bit;
};

typedef struct reg_p0serr{
    uint32_t err_i          :1;
    uint32_t err_m          :1;
    uint32_t reserved_1     :6;
    uint32_t err_t          :1;
    uint32_t err_c          :1;
    uint32_t err_p          :1;
    uint32_t err_e          :1;
    uint32_t reserved_2     :4;
    uint32_t diag_n         :1;
    uint32_t diag_i         :1;
    uint32_t diag_w         :1;
    uint32_t diag_b         :1;
    uint32_t diag_d         :1;
    uint32_t diag_c         :1;
    uint32_t diag_h         :1;
    uint32_t diag_s         :1;
    uint32_t diag_t         :1;
    uint32_t diag_f         :1;
    uint32_t diag_x         :1;
    uint32_t reserved_3     :5;
}reg_p0serr_t;
union p0serr_reg{
    uint32_t       all;
    reg_p0serr_t  bit;
};

typedef struct reg_p0sntf{
    uint32_t pmn            :16;
    uint32_t reserved       :16;
}reg_p0sntf_t;
union p0sntf_reg{
    uint32_t       all;
    reg_p0sntf_t  bit;
};

typedef struct reg_p0dmacr{
    uint32_t txts           :4;
    uint32_t rxts           :4;
    uint32_t txabl          :4;
    uint32_t rxabl          :4;
    uint32_t reserved       :16;
}reg_p0dmacr_t;
union p0dmacr_reg{
    uint32_t       all;
    reg_p0dmacr_t  bit;
};

typedef struct reg_p0phycr{
    uint32_t mpy            :4;
    uint32_t lb             :2;
    uint32_t los            :1;
    uint32_t rxinvpair      :1;
    uint32_t rxterm         :2;
    uint32_t rxcdr          :3;
    uint32_t rxeq           :4;
    uint32_t txinvpair      :1;
    uint32_t txcm           :1;
    uint32_t txswing        :3;
    uint32_t txde           :4;
    uint32_t reserved       :4;
    uint32_t overrid        :1;
    uint32_t enpll          :1;
}reg_p0phycr_t;
union p0phycr_reg{
    uint32_t       all;
    reg_p0phycr_t  bit;
};

typedef struct reg_p0physr{
    uint32_t lock           :1;
    uint32_t sigdet         :1;
    uint32_t reserved       :30;
}reg_p0physr_t;
union p0physr_reg{
    uint32_t       all;
    reg_p0physr_t  bit;
};

struct registers 
{
	union cap_reg       cap;
	union ghc_reg       ghc;
	union is_reg        is;
	union pi_reg        pi;
	union vs_reg        vs;
	union ccc_ctl_reg   ccc_ctl;
	uint32_t            ccc_ports;
	union bistafr_reg   bistafr;
	union bistcr_reg    bistcr;
	uint32_t            bistfctr;
	union bistsr_reg    bistsr;
	uint32_t            bistdecr;
	union timer1ms_reg  timer1ms;
	union gparam1r_reg  gparam1r;
	union gparam2r_reg  gparam2r;
	union pparamr_reg   pparamr;
	union testr_reg     testr;
	uint32_t            versionr;
	uint32_t            idr;
	union p0clb_reg     p0clb;
	union p0fb_reg      p0fb;
	union p0is_reg      p0is;
	union p0ie_reg      p0ie;
	union p0cmd_reg     p0cmd;
	union p0tfd_reg     p0tfd;
	uint32_t            p0sig;
	union p0ssts_reg    p0ssts;
	union p0ssts_reg    p0sctl;
	union p0serr_reg    p0serr;
	uint32_t            p0sact;
	uint32_t            p0ci;
	union p0sntf_reg    p0sntf;
	union p0dmacr_reg   p0dmacr;
	union p0phycr_reg   p0phycr;
	union p0physr_reg   p0physr;
};


struct c6748_device 
{
	conf_object_t* obj;
	core_signal_intf* signal_iface;
	conf_object_t* signal;
	struct registers regs;
};
typedef struct c6748_device sata_c6748_device;
static char* regs_name[] = 
{
	"CAP",
	"GHC",
	"IS",
	"PI",
	"VS",
	"CCC_CTL",
	"CCC_PORTS",
	"BISTAFR",
	"BISTCR",
	"BISTFCTR",
	"BISTSR",
	"BISTDECR",
	"TIMER1MS",
	"GPARAM1R",
	"GPARAM2R",
	"PPARAMR",
	"TESTR",
	"VERSIONR",
	"IDR",
	"P0CLB",
	"P0FB",
	"P0IS",
	"P0IE",
	"P0CMD",
	"P0TFD",
	"P0SIG",
	"P0SSTS",
	"P0SCTL",
	"P0SERR",
	"P0SACT",
	"P0CI",
	"P0SNTF",
	"P0DMACR",
	"P0PHYCR",
	"P0PHYSR",
	NULL
};
#define CAP 0x000
#define GHC 0x004
#define IS 0x008
#define PI 0x00c
#define VS 0x010
#define CCC_CTL 0x014
#define CCC_PORTS 0x018
#define BISTAFR 0x0A0
#define BISTCR 0x0A4
#define BISTFCTR 0xA8
#define BISTSR 0xAC
#define BISTDECR 0xB0
#define TIMER1MS 0xE0
#define GPARAM1R 0xE8
#define GPARAM2R 0xEC
#define PPARAMR 0xF0
#define TESTR 0xF4
#define VERSIONR 0xF8
#define IDR 0xFC
#define P0CLB 0x100
#define P0FB 0x108
#define P0IS 0x110
#define P0IE 0x114
#define P0CMD 0x118
#define P0TFD 0x120
#define P0SIG 0x124
#define P0SSTS 0x128
#define P0SCTL 0x12C
#define P0SERR 0x130
#define P0SACT 0x134
#define P0CI 0x138
#define P0SNTF 0x13C
#define P0DMACR 0x170
#define P0PHYCR 0x178
#define P0PHYSR 0x17C
#endif  
