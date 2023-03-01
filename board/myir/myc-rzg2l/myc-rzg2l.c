#include <common.h>
#include <cpu_func.h>
#include <image.h>
#include <init.h>
#include <malloc.h>
#include <netdev.h>
#include <dm.h>
#include <dm/platform_data/serial_sh.h>
#include <asm/processor.h>
#include <asm/mach-types.h>
#include <asm/io.h>
#include <linux/bitops.h>
#include <linux/errno.h>
#include <asm/arch/sys_proto.h>
#include <asm/gpio.h>
#include <asm/arch/gpio.h>
#include <asm/arch/rmobile.h>
#include <asm/arch/rcar-mstp.h>
#include <asm/arch/sh_sdhi.h>
#include <i2c.h>
#include <mmc.h>
#include <linux/delay.h>

DECLARE_GLOBAL_DATA_PTR;

#define PFC_BASE	0x11030000

#define ETH_CH0		(PFC_BASE + 0x300c)
#define ETH_CH1		(PFC_BASE + 0x3010)
#define I2C_CH1 	(PFC_BASE + 0x1870)
#define ETH_PVDD_3300	0x00
#define ETH_PVDD_1800	0x01
#define ETH_PVDD_2500	0x02
#define ETH_MII_RGMII	(PFC_BASE + 0x3018)

/* CPG */
#define CPG_BASE					0x11010000
#define CPG_CLKON_BASE				(CPG_BASE + 0x500)
#define CPG_RESET_BASE				(CPG_BASE + 0x800)
#define CPG_RESET_ETH				(CPG_RESET_BASE + 0x7C)
#define CPG_RESET_I2C                           (CPG_RESET_BASE + 0x80)
#define CPG_PL2_SDHI_DSEL                           (CPG_BASE + 0x218)
#define CPG_CLK_STATUS                           (CPG_BASE + 0x280)

/* PFC */
#define PFC_P37						(PFC_BASE + 0x037)
#define PFC_PM37					(PFC_BASE + 0x16E)
#define PFC_PMC37					(PFC_BASE + 0x237)


#define PFC_P37                               (PFC_BASE + 0x037)
#define PFC_PM37                              (PFC_BASE + 0x16E)
#define PFC_PMC37                             (PFC_BASE + 0x237)

/*P44_3*/
#define PFC_P3C                               (PFC_BASE + 0x03C)
#define PFC_PM3C                              (PFC_BASE + 0x178)
#define PFC_PMC3C                             (PFC_BASE + 0x23C)
/*P43_3*/
#define PFC_P3B                               (PFC_BASE + 0x03B)
#define PFC_PM3B                              (PFC_BASE + 0x176)
#define PFC_PMC3B                             (PFC_BASE + 0x23B)

/*P4_0*//*belongs to v3lite*/
#define PFC_P14                               (PFC_BASE + 0x014)
#define PFC_PM14                              (PFC_BASE + 0x128)
#define PFC_PMC14                             (PFC_BASE + 0x214)

void s_init(void)
{
	/* SD1 power control: P39_1 = 0; P39_2 = 1; */
	*(volatile u32 *)(PFC_PMC37) &= 0xFFFFFFF9; /* Port func mode 0b00 */
	*(volatile u32 *)(PFC_PM37) = (*(volatile u32 *)(PFC_PM37) & 0xFFFFFFC3) | 0x28; /* Port output mode 0b1010 */
//#if CONFIG_TARGET_SMARC_RZG2L
#if CONFIG_TARGET_MYC_RZG2L
	*(volatile u32 *)(PFC_P37) = (*(volatile u32 *)(PFC_P37) & 0xFFFFFFF9) | 0x6;	/* Port 39[2:1] output value 0b11*/
#else

	*(volatile u32 *)(PFC_P37) = (*(volatile u32 *)(PFC_P37) & 0xFFFFFFF9) | 0x4;	/* Port 39[2:1] output value 0b10*/
#endif

	/* can go in board_eht_init() once enabled */
	*(volatile u32 *)(ETH_CH0) = (*(volatile u32 *)(ETH_CH0) & 0xFFFFFFFC) | ETH_PVDD_1800;
	*(volatile u32 *)(ETH_CH1) = (*(volatile u32 *)(ETH_CH1) & 0xFFFFFFFC) | ETH_PVDD_1800;
	/* Enable RGMII for both ETH{0,1} */
	*(volatile u32 *)(ETH_MII_RGMII) = (*(volatile u32 *)(ETH_MII_RGMII) & 0xFFFFFFFC);
	/* ETH CLK */
	*(volatile u32 *)(CPG_RESET_ETH) = 0x30003;
	/* I2C CLK */
	*(volatile u32 *)(CPG_RESET_I2C) = 0xF000F;
	/* I2C pin non GPIO enable */
	*(volatile u32 *)(I2C_CH1) = 0x01010101;
	/* SD CLK */
	*(volatile u32 *)(CPG_PL2_SDHI_DSEL) = 0x00110011;
	while (*(volatile u32 *)(CPG_CLK_STATUS) != 0)
		;
}

int board_early_init_f(void)
{

	return 0;
}

int board_init(void)
{
	/* adress of boot parameters */
	gd->bd->bi_boot_params = CONFIG_SYS_TEXT_BASE + 0x50000;

	mdelay(140);
	/*belongs to v3lite*/
 	*(volatile u32 *)(PFC_PMC14) &= 0xFFFFFFFE; /* Port func mode  */
 	*(volatile u32 *)(PFC_PM14) = (*(volatile u32 *)(PFC_PM14) & 0xFFFFFF8) | 0x2; /* Port output mode 0b10 */
 	*(volatile u32 *)(PFC_P14) = (*(volatile u32 *)(PFC_P14) & 0xFFFFFFF4) | 0x05;
	mdelay(13);

	// phy1 reset
	*(volatile u32 *)(PFC_PMC3C) &= 0xFFFFFFF7; /* Port func mode  */
	*(volatile u32 *)(PFC_PM3C) = (*(volatile u32 *)(PFC_PM3C) & 0xFFFFFF3F) | 0x80; /* Port output mode 0b10 */

	//mdelay(13);
	*(volatile u32 *)(PFC_P3C) = (*(volatile u32 *)(PFC_P3C) & 0xFFFFFFF7) | 0x08; //high
	mdelay(20);
	/*value*/
	*(volatile u32 *)(PFC_P3C) = (*(volatile u32 *)(PFC_P3C) & 0xFFFFFFF7) | 0x0; // low
	mdelay(13);
	*(volatile u32 *)(PFC_P3C) = (*(volatile u32 *)(PFC_P3C) & 0xFFFFFFF7) | 0x08; //high


	// phy2 reset
	*(volatile u32 *)(PFC_PMC3B) &= 0xFFFFFFFB; /* Port func mode  */
	*(volatile u32 *)(PFC_PM3B) = (*(volatile u32 *)(PFC_PM3B) & 0xFFFFFF3F) | 0x80; /* Port output mode 0b10 */

	*(volatile u32 *)(PFC_P3B) = (*(volatile u32 *)(PFC_P3B) & 0xFFFFFFF7) | 0x08;
	mdelay(20);
	*(volatile u32 *)(PFC_P3B) = (*(volatile u32 *)(PFC_P3B) & 0xFFFFFFF7) | 0x0;
	mdelay(13);
	*(volatile u32 *)(PFC_P3B) = (*(volatile u32 *)(PFC_P3B) & 0xFFFFFFF7) | 0x08;

	return 0;
}

void reset_cpu(void)
{

}
