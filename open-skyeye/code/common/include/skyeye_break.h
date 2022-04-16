#ifndef __SKYEYE_BREAK_H__
#define __SKYEYE_BREAK_H__

/* @brief The breakpoint struct
*/
typedef struct breakpoint_s{
	breakpoint_id_t id;
	access_t access_type;
	breakpoint_kind_t address_type;
	generic_address_t address;
	uint32 hits;
	uint64 last_bp_step;
}breakpoint_t;

typedef struct breakpoint_mgt{
	/* currently max breakpoint number supported by us is MAX_BP_NUMBER */
	breakpoint_t breakpoint[MAX_BP_NUMBER];
	/* The number point to the last element */
	int bp_number;
}breakpoint_mgt_t;


#endif



