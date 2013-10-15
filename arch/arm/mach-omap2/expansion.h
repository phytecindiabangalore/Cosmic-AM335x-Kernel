#define DEVICE 2

/* convert GPIO signal to GPIO pin number */
#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))

static int i;
char cosmic_am335_devices_setup_str[80] = "none";

static void rgmii2_gpio_config(void);

/* module pin mux structure */
struct pinmux_config {
	const char *string_name; /* signal name format */
	int val; /* options for the mux register value */
};

/*
* @pin_mux - single module pin-mux structure which defines
*       pin-mux details for all its pins
*/
static void setup_pin_mux(struct pinmux_config *pin_mux)
{
	int i;

	for (i = 0; pin_mux->string_name != NULL; pin_mux++)
		omap_mux_init_signal(pin_mux->string_name, pin_mux->val);
}

/* Pin-mux for GPIO */
static struct pinmux_config gpio_pin_mux[] = {
	{"mcasp0_fsr.gpio3_19", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
					AM33XX_PIN_OUTPUT},
	{"mii1_rxdv.gpio3_4", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
					AM33XX_PIN_OUTPUT},
	{"gpmc_csn1.gpio1_30", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
					AM33XX_PIN_OUTPUT},
	{"emu0.gpio3_7", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
					AM33XX_PIN_OUTPUT},
	{"gpmc_csn2.gpio1_31", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
					AM33XX_PIN_OUTPUT},
	{"gpmc_wpn.gpio0_31", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
					AM33XX_PIN_OUTPUT},
	{"mcasp0_aclkx.gpio3_14", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
					AM33XX_PIN_OUTPUT},
	{"mcasp0_ahclkx.gpio3_21", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
					AM33XX_PIN_OUTPUT},
	{"mcasp0_axr1.gpio3_20", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
					AM33XX_PIN_OUTPUT},
	{"xdma_event_intr1.gpio0_20", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
					AM33XX_PIN_OUTPUT},
	{NULL, 0},
};

/* Pin-mux for RGMII2 */
static struct pinmux_config rgmii2_pin_mux[] = {
	{"gpmc_a0.rgmii2_tctl", OMAP_MUX_MODE2 | AM33XX_PIN_OUTPUT},
	{"gpmc_a1.rgmii2_rctl", OMAP_MUX_MODE2 | AM33XX_PIN_INPUT_PULLDOWN},
	{"gpmc_a2.rgmii2_td3", OMAP_MUX_MODE2 | AM33XX_PIN_OUTPUT},
	{"gpmc_a3.rgmii2_td2", OMAP_MUX_MODE2 | AM33XX_PIN_OUTPUT},
	{"gpmc_a4.rgmii2_td1", OMAP_MUX_MODE2 | AM33XX_PIN_OUTPUT},
	{"gpmc_a5.rgmii2_td0", OMAP_MUX_MODE2 | AM33XX_PIN_OUTPUT},
	{"gpmc_a6.rgmii2_tclk", OMAP_MUX_MODE2 | AM33XX_PIN_OUTPUT},
	{"gpmc_a7.rgmii2_rclk", OMAP_MUX_MODE2 | AM33XX_PIN_INPUT_PULLDOWN},
	{"gpmc_a8.rgmii2_rd3", OMAP_MUX_MODE2 | AM33XX_PIN_INPUT_PULLDOWN},
	{"gpmc_a9.rgmii2_rd2", OMAP_MUX_MODE2 | AM33XX_PIN_INPUT_PULLDOWN},
	{"gpmc_a10.rgmii2_rd1", OMAP_MUX_MODE2 | AM33XX_PIN_INPUT_PULLDOWN},
	{"gpmc_a11.rgmii2_rd0", OMAP_MUX_MODE2 | AM33XX_PIN_INPUT_PULLDOWN},
	{"mdio_data.mdio_data", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"mdio_clk.mdio_clk", OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT_PULLUP},
	{NULL, 0},
};

/* Platform Data for MMC */

static struct omap2_hsmmc_info am335x_mmc[] __initdata = {
	{
		.mmc            = 1,
		.caps           = MMC_CAP_4_BIT_DATA,
		.gpio_cd        = GPIO_TO_PIN(0, 6),
		.gpio_wp        = -EINVAL,
		.ocr_mask       = MMC_VDD_32_33 | MMC_VDD_33_34, /* 3V3 */
	},
	{
		.mmc            = 0,    /* will be set at runtime */
	},
	{
		.mmc            = 0,    /* will be set at runtime */
	},
	{}      /* Terminator */
};

static void rgmii2_init(void)
{
	printk(KERN_INFO"Phytec AM335X : RGMII2 Init\n");
	setup_pin_mux(rgmii2_pin_mux);
	return;
}

struct devices {
	char *device_name;
	void (*device_init) (void);
};

struct devices cosmic_am335x_device[] = {
	{"GPIO", rgmii2_gpio_config},
	{"RGMII2", rgmii2_gpio_config},
	{"NULL", NULL },
};

static void rgmii2_gpio_config(void)
{
	rgmii2_init();
	setup_pin_mux(gpio_pin_mux);
	return;
}

void device_parser(char *device)
{
	for (i = 0; i < DEVICE; i++) {
		if (device != NULL) {
			if (strcmp(device, \
				cosmic_am335x_device[i].device_name) == 0)
				cosmic_am335x_device[i].device_init();
		}
	}
}

struct structure {
	char one[10];
	};

void bootarg_seperator(char *string)
{
	struct structure my_structure;
	char *token, *a, *c;
	if (string != NULL) {
		c = string;
		while ((token = strsep(&string, ",")) != NULL) {
			a = token;
			memcpy(&my_structure, a, sizeof(my_structure));
			device_parser(my_structure.one);
		}
	}
}

void expansion_init(void)
{
	printk(KERN_INFO"\nPhytec AM335x : Device Muxxing\n");
	bootarg_seperator(cosmic_am335_devices_setup_str);
	return;
}
