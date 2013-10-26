/*
 * Phytec AM33XX Device based muxxing.
 *
 * Copyright (C) 2013 Phytec Embedded Pvt Ltd - http://www.phytec.in/
 *
 * Author: Ashutosh Singh <ashutosh.s@phytec.in>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#define DEVICE 11

/* convert GPIO signal to GPIO pin number */
#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))

static int i, wifien, i2c2en, led_btn;
char cosmic_am335_devices_setup_str[80] = "none";

static void wifibt_rgmii2_gpio_config(void);
static void uart1_i2c2_config(void);
static void uart1_i2c1_config(void);

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

/* pin-mux for mmc0 */
static struct pinmux_config mmc0_pin_mux[] = {
	{"mmc0_dat3.mmc0_dat3", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"mmc0_dat2.mmc0_dat2", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"mmc0_dat1.mmc0_dat1", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"mmc0_dat0.mmc0_dat0", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"mmc0_clk.mmc0_clk",   OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"mmc0_cmd.mmc0_cmd",   OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"spi0_cs1.mmc0_sdcd",  OMAP_MUX_MODE7 | AM33XX_PIN_INPUT_PULLUP},
	{NULL, 0},
};

/* Pin-mux for GPIO */
static struct pinmux_config gpio_pin_mux[] = {
	{"mcasp0_fsr.gpio3_19", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
					AM33XX_PIN_OUTPUT},
	{"gpmc_csn1.gpio1_30", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
					AM33XX_PIN_OUTPUT},
	{"gpmc_csn2.gpio1_31", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
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

/* Pin-mux for GPIO which having conflict with rgmii2 & WIFI-BT */
static struct pinmux_config gpio_wifi_rgmi_pin_mux[] = {
	{"gpmc_a0.gpio1_16", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
					AM33XX_PIN_OUTPUT},
	{"gpmc_a1.gpio1_17", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
					AM33XX_PIN_OUTPUT},
	{"gpmc_a2.gpio1_18", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
					AM33XX_PIN_OUTPUT},
	{"gpmc_a3.gpio1_19", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
						AM33XX_PIN_OUTPUT},
	{"gpmc_a6.gpio1_22", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
						AM33XX_PIN_OUTPUT},
	{"gpmc_a7.gpio1_23", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
						AM33XX_PIN_OUTPUT},
	{"gpmc_a9.gpio1_25", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
						AM33XX_PIN_OUTPUT},
	{"gpmc_ben1.gpio1_28", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
						AM33XX_PIN_OUTPUT},
	{"gpmc_csn3.gpio2_0", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
						AM33XX_PIN_OUTPUT},
	{"gpmc_clk.gpio2_1", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
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

/* Module pin mux for wlan and bluetooth */
static struct pinmux_config mmc2_wl12xx_pin_mux[] = {
	{"gpmc_a1.mmc2_dat0", OMAP_MUX_MODE3 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_a2.mmc2_dat1", OMAP_MUX_MODE3 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_a3.mmc2_dat2", OMAP_MUX_MODE3 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ben1.mmc2_dat3", OMAP_MUX_MODE3 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_csn3.mmc2_cmd", OMAP_MUX_MODE3 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_clk.mmc2_clk", OMAP_MUX_MODE3 | AM33XX_PIN_INPUT_PULLUP},
	{NULL, 0},
};

static struct pinmux_config uart1_wl12xx_pin_mux[] = {
	{"uart1_ctsn.uart1_ctsn", OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
	{"uart1_rtsn.uart1_rtsn", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT},
	{"uart1_rxd.uart1_rxd", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"uart1_txd.uart1_txd", OMAP_MUX_MODE0 | AM33XX_PULL_ENBL},
	{NULL, 0},
};

static struct pinmux_config wl12xx_pin_mux[] = {
	{"gpmc_a6.gpio1_22", OMAP_MUX_MODE7 | AM33XX_PIN_OUTPUT_PULLUP},
	{"gpmc_a7.gpio1_23", OMAP_MUX_MODE7 | AM33XX_PIN_OUTPUT},
	{"gpmc_a9.gpio1_25", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT},
	{"mdio_data.mdio_data", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"mdio_clk.mdio_clk", OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT_PULLUP},
	{NULL, 0},
};

/* Module pin mux for I2C2 */
static struct pinmux_config i2c2_pin_mux[] = {
	{"uart1_ctsn.i2c2_sda", OMAP_MUX_MODE3 | AM33XX_SLEWCTRL_SLOW |
					AM33XX_PULL_UP | AM33XX_INPUT_EN},
	{"uart1_rtsn.i2c2_scl", OMAP_MUX_MODE3 | AM33XX_SLEWCTRL_SLOW |
					AM33XX_PULL_UP | AM33XX_INPUT_EN},
	{NULL, 0},
};

/* Module pin mux for I2C1 */
static struct pinmux_config i2c1_pin_mux[] = {
	{"uart1_rxd.i2c1_sda", OMAP_MUX_MODE3 | AM33XX_SLEWCTRL_SLOW |
					AM33XX_PULL_UP | AM33XX_INPUT_EN},
	{"uart1_txd.i2c1_scl", OMAP_MUX_MODE3 | AM33XX_SLEWCTRL_SLOW |
					AM33XX_PULL_UP | AM33XX_INPUT_EN},
	{NULL, 0},
};

/* Module pin mux for UART1 Tx & Rx */
static struct pinmux_config uart1_pin_mux[] = {
	{"uart1_rxd.uart1_rxd", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"uart1_txd.uart1_txd", OMAP_MUX_MODE0 | AM33XX_PULL_ENBL},
	{NULL, 0},
};

/* Module pin mux for UART2 */
static struct pinmux_config uart2_pin_mux[] = {
	{"mii1_txclk.uart2_rxd_mux0", OMAP_MUX_MODE1 | AM33XX_PIN_INPUT_PULLUP},
	{"mii1_rxclk.uart2_txd_mux0", OMAP_MUX_MODE1 | AM33XX_PULL_ENBL},
	{NULL, 0},
};

/* Module pin mux for UART3 */
static struct pinmux_config uart3_pin_mux[] = {
	{"mii1_rxd3.uart3_rxd_mux0", OMAP_MUX_MODE1 | AM33XX_PIN_INPUT_PULLUP},
	{"mii1_rxd2.uart3_txd_mux0", OMAP_MUX_MODE1 | AM33XX_PULL_ENBL},
	{NULL, 0},
};

/* Module pin mux for UART4 */
static struct pinmux_config uart4_pin_mux[] = {
	{"uart0_ctsn.uart4_rxd", OMAP_MUX_MODE1 | AM33XX_PIN_INPUT_PULLUP},
	{"uart0_rtsn.uart4_txd", OMAP_MUX_MODE1 | AM33XX_PULL_ENBL},
	{NULL, 0},
};

/* Module pin mux for SPI1 */
static struct pinmux_config spi1_pin_mux[] = {
	{"mii1_col.spi1_sclk", OMAP_MUX_MODE2 | AM33XX_PULL_ENBL
							| AM33XX_INPUT_EN},
	{"mcasp0_fsx.spi1_d0", OMAP_MUX_MODE3 | AM33XX_PULL_ENBL |
					AM33XX_PULL_UP | AM33XX_INPUT_EN},
	{"mcasp0_axr0.spi1_d1", OMAP_MUX_MODE3 | AM33XX_PULL_ENBL
							| AM33XX_INPUT_EN},
	{"xdma_event_intr0.spi1_cs1", OMAP_MUX_MODE4 | AM33XX_PULL_ENBL |
					AM33XX_PULL_UP | AM33XX_INPUT_EN},
	{"mcasp0_ahclkr.spi1_cs0", OMAP_MUX_MODE3 | AM33XX_PULL_ENBL |
					AM33XX_PULL_UP | AM33XX_INPUT_EN},
	{NULL, 0},
};

/* Module pin mux for Gpio1_8 */
static struct pinmux_config gpio1_8_pin_mux[] = {
	{"uart0_ctsn.gpio1_8", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT |
						AM33XX_PIN_OUTPUT},
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

/* RGMII2 Initialization */
static void rgmii2_init(void)
{
	printk(KERN_INFO"Phytec AM335X : RGMII2 Init\n");
	setup_pin_mux(rgmii2_pin_mux);
	return;
}

/* GPIO Initialization */
static void gpio_init(void)
{
	printk(KERN_INFO"Phytec AM335X : GPIO Init\n");
	setup_pin_mux(gpio_wifi_rgmi_pin_mux);
	return;
}

/* Platform Data for WIFI-BT */
#define PCM051_WLAN_IRQ_GPIO    GPIO_TO_PIN(1, 25)
#define PCM051_WLAN_EN          GPIO_TO_PIN(1, 23)
#define PCM051_BT_EN            GPIO_TO_PIN(1, 22)

struct wl12xx_platform_data pcm051_wlan_data = {
	.irq = OMAP_GPIO_IRQ(PCM051_WLAN_IRQ_GPIO),
	.board_ref_clock = WL12XX_REFCLOCK_38_XTAL, /* 38.4Mhz */
	.bt_enable_gpio = PCM051_BT_EN,
	.wlan_enable_gpio = PCM051_WLAN_EN,
};

/* MMC2 for WIFI initialization */
static void mmc2_wl12xx_init(void)
{
	setup_pin_mux(mmc2_wl12xx_pin_mux);

	am335x_mmc[1].mmc = 3;
	am335x_mmc[1].name = "wl1271";
	am335x_mmc[1].caps = MMC_CAP_4_BIT_DATA | MMC_CAP_POWER_OFF_CARD;
	am335x_mmc[1].nonremovable = true;
	am335x_mmc[1].gpio_cd = -EINVAL;
	am335x_mmc[1].gpio_wp = -EINVAL;
	/* am335x_mmc[1].ocr_mask = MMC_VDD_165_195; */ /* 1V8 */
	am335x_mmc[1].ocr_mask = MMC_VDD_32_33 | MMC_VDD_33_34; /* 3V3 */

	/* mmc will be initialized when mmc0_init is called */
	return;
}

/* UART1 full modem initialization */
static void uart1_wl12xx_init(void)
{
	printk(KERN_INFO"Phytec-AM335X : UART1 Full modem support\n");
	setup_pin_mux(uart1_wl12xx_pin_mux);
}

/* Enabling Bluetooth */
static void wl12xx_bluetooth_enable(void)
{
	int status = gpio_request(pcm051_wlan_data.bt_enable_gpio,
		"bt_en\n");
	if (status < 0)
		pr_err("Failed to request gpio for bt_enable");

	pr_info("Configure Bluetooth Enable pin...\n");
	gpio_direction_output(pcm051_wlan_data.bt_enable_gpio, 0);
}

static int wl12xx_set_power(struct device *dev, int slot, int on, int vdd)
{
	if (on) {
		gpio_direction_output(pcm051_wlan_data.wlan_enable_gpio, 1);
		mdelay(70);
	} else {
		gpio_direction_output(pcm051_wlan_data.wlan_enable_gpio, 0);
	}

	return 0;
}

/* Wl12xx initialization for WIFI-BT */
static void wl12xx_init(void)
{
	struct device *dev;
	struct omap_mmc_platform_data *pdata;
	int ret;

	setup_pin_mux(wl12xx_pin_mux);
	wl12xx_bluetooth_enable();

	if (wl12xx_set_platform_data(&pcm051_wlan_data))
		pr_err("error setting wl12xx data\n");

	dev = am335x_mmc[1].dev;
	if (!dev) {
		pr_err("wl12xx mmc device initialization failed\n");
		goto out;
	}

	pdata = dev->platform_data;
	if (!pdata) {
		pr_err("Platfrom data of wl12xx device not set\n");
		goto out;
	}

	ret = gpio_request_one(pcm051_wlan_data.wlan_enable_gpio,
		GPIOF_OUT_INIT_LOW, "wlan_en");
	if (ret) {
		pr_err("Error requesting wlan enable gpio: %d\n", ret);
		goto out;
	}

	pdata->slots[0].set_power = wl12xx_set_power;
out:
	return;
}

/* MMC0 Initialization */
static void mmc0_init(void)
{
	setup_pin_mux(mmc0_pin_mux);

	omap2_hsmmc_init(am335x_mmc);
	return;
}

/* WIFI-BT Initialization */
static void wifi_bt_init(void)
{
	printk(KERN_INFO"Phytec AM335X : WIFI-BT Init\n");
	mmc2_wl12xx_init();
	mmc0_init();
	uart1_wl12xx_init();
	wl12xx_init();
	wifien++;
	return;
}

/* I2C2 Initialization */
static void i2c2_init(void)
{
	if (wifien == 1)
		printk(KERN_INFO"\nYou can not use I2C2 when using WIFI-BT\n");
	else {
		setup_pin_mux(i2c2_pin_mux);
		printk(KERN_INFO"Phytec-AM335X : I2C2 support\n");
		i2c2en++;
	}
	return;
}

/* I2C1 Initialization */
static void i2c1_init(void)
{
	setup_pin_mux(i2c1_pin_mux);
	printk(KERN_INFO"Phytec-AM335X : I2C1 support\n");
	return;
}

/* UART1 Initialization */
static void uart1_init(void)
{
	setup_pin_mux(uart1_pin_mux);
	printk(KERN_INFO"Phytec-AM335X : UART1 support\n");
	return;
}

/* UART2 Initialization */
static void uart2_init(void)
{
	setup_pin_mux(uart2_pin_mux);
	printk(KERN_INFO"Phytec-AM335X : UART2 support\n");
	return;
}

/* UART3 Initialization */
static void uart3_init(void)
{
	setup_pin_mux(uart3_pin_mux);
	printk(KERN_INFO"Phytec-AM335X : UART3 support\n");
	return;
}

/* UART4 Initialization */
static void uart4_init(void)
{
	if (led_btn == 1)
		printk(KERN_INFO"\nYou can not use UART4 when using user LED & Button\n");
	else {
		setup_pin_mux(uart4_pin_mux);
		printk(KERN_INFO"Phytec-AM335X : UART4 support\n");
	}
	return;
}

/* SPI1 Initialization */
static void spi1_init(void)
{
	setup_pin_mux(spi1_pin_mux);
	printk(KERN_INFO"Phytec-AM335X : SPI1 support\n");
	return;
}

struct devices {
	char *device_name;
	void (*device_init) (void);
};

struct devices cosmic_am335x_device[] = {
	{"GPIO", wifibt_rgmii2_gpio_config},
	{"RGMII2", wifibt_rgmii2_gpio_config},
	{"WIFI-BT", wifibt_rgmii2_gpio_config},
	{"UART1FULL", uart1_i2c2_config},
	{"I2C2", uart1_i2c2_config},
	{"UART1", uart1_i2c1_config},
	{"I2C1", uart1_i2c1_config},
	{"UART2", uart2_init},
	{"UART3", uart3_init},
	{"UART4", uart4_init},
	{"SPI1", spi1_init},
	{"NULL", NULL },
};

/* Configuration function for wifi-bt, gpio, rgmii2 */
static void wifibt_rgmii2_gpio_config(void)
{
	static int count;
	static int mux_val;

	if (strcmp("GPIO", cosmic_am335x_device[i].device_name) == 0)
			mux_val = 1;
	else if (strcmp("RGMII2", cosmic_am335x_device[i].device_name) == 0)
			mux_val = 2;
	else if (strcmp("WIFI-BT", cosmic_am335x_device[i].device_name) == 0)
			mux_val = 3;
	if (count < 1) {
		switch (mux_val) {
		case 1:
			gpio_init();
			count++;
			break;
		case 2:
			if (led_btn == 1)
				printk(KERN_INFO"\nYou can not use RGMII2 when using user LED & Button\n");
			else
				rgmii2_init();
			count++;
			break;
		case 3:
			if (i2c2en == 1)
				printk(KERN_INFO"\nYou can use only I2C2 or WIFI-BT at a time\n");
			else
				wifi_bt_init();
			count++;
			break;
		}
	} else
	printk(KERN_INFO"\nYou can use only RGMII2 or Conflict GPIO at a time\n");
	setup_pin_mux(gpio_pin_mux);
	return;
}

/* Configuration function for uart1 & i2c2 */
static void uart1_i2c2_config(void)
{
	static int count;
	static int mux_val;

	if (strcmp("UART1FULL", cosmic_am335x_device[i].device_name) == 0)
			mux_val = 1;
	if (strcmp("I2C2", cosmic_am335x_device[i].device_name) == 0)
			mux_val = 2;
	if (count < 1) {
		switch (mux_val) {
		case 1:
			uart1_wl12xx_init();
			count++;
			break;
		case 2:
			i2c2_init();
			count++;
			break;
		}
	} else
	printk(KERN_INFO"\nYou can use only UART1-Full modem or I2C2 at a time\n");
	return;
}

/* Configuration function for uart1 & i2c1 */
static void uart1_i2c1_config(void)
{
	static int count;
	static int mux_val;

	if (strcmp("UART1", cosmic_am335x_device[i].device_name) == 0)
			mux_val = 1;
	if (strcmp("I2C1", cosmic_am335x_device[i].device_name) == 0)
			mux_val = 2;
	if (count < 1) {
		switch (mux_val) {
		case 1:
			uart1_init();
			count++;
			break;
		case 2:
			i2c1_init();
			count++;
			break;
		}
	} else
	printk(KERN_INFO"\nYou can use only UART1 or I2C1 at a time\n");
	return;
}

/* Specific device will be called by this function
 * it will compare each device name if matches then
 * call specific init function for the particular device.
 */
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

/* This function will seperate the bootargs which is catched by the
 * board file into different devices then device_parser function will
 * call specific init function for the particular device.
 */
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

/* This function is used to initialise the expansion based devices. */
void expansion_init(void)
{
	printk(KERN_INFO"\nPhytec AM335x : Device Muxxing\n");
	bootarg_seperator(cosmic_am335_devices_setup_str);
	return;
}
