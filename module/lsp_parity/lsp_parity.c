#include <a1000/sysreg_base.h>
#include <general_inc.h>
#include <utility_lite.h>
#include <test_common.h>
#include <int_num.h>
#include <module/lsp_parity.h>
#include <module/gpio.h>

volatile bool_enum int_flag_lsp0 = HVTE_FALSE;
volatile bool_enum int_flag_lsp1 = HVTE_FALSE;

// this test must force singal A_I2S0_CK
int lsp_parity_test_irq(zebu_test_mod_t *mod, void *para)
{
	u32 temp_data;
	u32 i = 10;
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 gpio0_b_ddr_addr = GPIO0_BASE_ADDR + GPIO_SWPORTB_DDR;
	u32 gpio1_b_ddr_addr = GPIO1_BASE_ADDR + GPIO_SWPORTB_DDR;

	int_flag_lsp0 = HVTE_FALSE;
	int_flag_lsp1 = HVTE_FALSE;

	hprintf(DEBUG, "%s \r\n", __FUNCTION__);

	SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP0_PARITY_IRQ);
	SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP1_PARITY_IRQ);

	//    REG32_WRITE(0xc0038004, REG32_READ(0xc0038004) | (0x21 << 13));  // force pinmux
	//    REG32_WRITE(0xc0038000, REG32_READ(0xc0038000) | (0x1 << 22));  // force pinmux

	REG32_WRITE(LSP0_CRM_ADDR + LSP0_SAFETY_FUN_CTRL_REG, 0x30);
	REG32_WRITE(LSP1_CRM_ADDR + LSP1_SAFETY_FUN_CTRL_REG, 0x30);

	temp_data = REG32_READ(gpio0_b_ddr_addr);
	temp_data = 0x7831894a;
	REG32_WRITE(gpio0_b_ddr_addr, temp_data);

	temp_data = REG32_READ(gpio1_b_ddr_addr);
	temp_data = 0x2c6f0592;
	REG32_WRITE(gpio1_b_ddr_addr, temp_data);

	temp_data = REG32_READ(gpio0_b_ddr_addr);
	temp_data = 0x95672134;
	REG32_WRITE(gpio0_b_ddr_addr, temp_data);

	temp_data = REG32_READ(gpio1_b_ddr_addr);
	temp_data = 0x49456a7d;
	REG32_WRITE(gpio1_b_ddr_addr, temp_data);

	// addr

	temp_data = REG32_READ(SAFETY_CRM_ADDR + SAFENOC_ERR_INJ_CTRL);
	temp_data = 0x0;
	REG32_WRITE(SAFETY_CRM_ADDR + SAFENOC_ERR_INJ_CTRL, temp_data);

	temp_data = REG32_READ(SAFETY_CRM_ADDR + SAFENOC_ERR_INJ1_CTRL);
	temp_data |= 0x05;
	REG32_WRITE(SAFETY_CRM_ADDR + SAFENOC_ERR_INJ1_CTRL, temp_data);

	temp_data = REG32_READ(gpio0_b_ddr_addr);
	temp_data = 0x9d7a6543;
	REG32_WRITE(gpio0_b_ddr_addr, temp_data);

	temp_data = REG32_READ(gpio1_b_ddr_addr);
	temp_data = 0x9d7a6543;
	REG32_WRITE(gpio1_b_ddr_addr, temp_data);

	temp_data = REG32_READ(SAFETY_CRM_ADDR + SAFENOC_ERR_INJ1_CTRL);
	temp_data &= ~0x05;
	REG32_WRITE(SAFETY_CRM_ADDR + SAFENOC_ERR_INJ1_CTRL, temp_data);

	while (int_flag_lsp0 == HVTE_FALSE) {
		i--;
		udelay(100);
		if (i <= 0) {
			hprintf(ERROR, "lsp0 parity irq timeout.\r\n");
			break;
		}
	}
	while (int_flag_lsp1 == HVTE_FALSE) {
		i--;
		udelay(100);
		if (i <= 0) {
			hprintf(ERROR, "lsp1 parity irq timeout.\r\n");
			break;
		}
	}

	HCOMP(HVTE_TRUE, int_flag_lsp0);
	HCOMP(HVTE_TRUE, int_flag_lsp1);

	return result;
}

void handler_irq_lsp_parity(int param)
{
	printf_intr("\r\n");
	if (param == FIQ_IRQ_LSP0_PARITY_IRQ) {
		// SAFETY_IRQ_DISABLE(118);
		u32 temp_data =
			REG32_READ(SAFETY_CRM_ADDR + SAFENOC_ERR_INJ1_CTRL);
		temp_data &= ~0x04;
		REG32_WRITE(SAFETY_CRM_ADDR + SAFENOC_ERR_INJ1_CTRL, temp_data);
		REG32_WRITE(
			LSP0_CRM_ADDR + LSP0_SAFETY_FUN_CTRL_REG,
			REG32_READ(LSP0_CRM_ADDR + LSP0_SAFETY_FUN_CTRL_REG) |
				(0x3 << 2));
		printf_intr("lsp0 parity interrupt triggered \r\n");
		int_flag_lsp0 = HVTE_TRUE;

	} else if (param == FIQ_IRQ_LSP1_PARITY_IRQ) {
		// SAFETY_IRQ_DISABLE(119);
		u32 temp_data =
			REG32_READ(SAFETY_CRM_ADDR + SAFENOC_ERR_INJ1_CTRL);
		temp_data &= ~0x01;
		REG32_WRITE(SAFETY_CRM_ADDR + SAFENOC_ERR_INJ1_CTRL, temp_data);
		REG32_WRITE(
			LSP1_CRM_ADDR + LSP1_SAFETY_FUN_CTRL_REG,
			REG32_READ(LSP1_CRM_ADDR + LSP1_SAFETY_FUN_CTRL_REG) |
				(0x3 << 2));
		printf_intr("lsp1 parity interrupt triggered \r\n");
		int_flag_lsp1 = HVTE_TRUE;
	}
}

/*==================================LSP_PARITY ZEBU TEST=====================================================*/

zebu_test_mod_t zebu_test_lsp_parity_mods[] = {
	{ "lsp_parity", 0x0 },
};

zebu_test_entry_t zebu_test_lsp_parity_entrys[] = {

	{ '0', 0, "lsp parity irq test", lsp_parity_test_irq },

};

static zebu_test_ctrl_t zebu_mod_test_ctrl = {
	"lsp_parity",
	SIZE_ARRAY(zebu_test_lsp_parity_mods),
	zebu_test_lsp_parity_mods,
	SIZE_ARRAY(zebu_test_lsp_parity_entrys),
	zebu_test_lsp_parity_entrys,
	0
};

void lspParityMain(int para)
{
	int ret;
	zebu_mod_test_ctrl.current_mod = 0;
	if (para) {
		ret = zebu_mod_test_all_mod(&zebu_mod_test_ctrl);
		hprintf(TSENV, "！！auto test err %d!!!!\r\n", ret);
	} else {
		//TestLoop();
		TestLoop(&zebu_mod_test_ctrl);
	}
}

IRQ_DEFINE(FIQ_IRQ_LSP0_PARITY_IRQ, handler_irq_lsp_parity,
	   FIQ_IRQ_LSP0_PARITY_IRQ, "lsp0 parity irq",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_LSP1_PARITY_IRQ, handler_irq_lsp_parity,
	   FIQ_IRQ_LSP1_PARITY_IRQ, "lsp1 parity irq",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);

CMD_DEFINE(lsp_parity, lspParityMain, 0, "lsp parity test", 0);
