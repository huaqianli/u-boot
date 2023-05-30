// SPDX-License-Identifier: GPL-2.0+
/*
 */

#include <common.h>
#include <dm.h>
#include <log.h>
#include <sysinfo.h>
#include <net.h>
#include <asm/arch/hardware.h>

#include "iot2050.h"

#define IOT2050_INFO_MAGIC		0x20502050

struct iot2050_info {
	u32 magic;
	u16 size;
	char name[20 + 1];
	char serial[16 + 1];
	char mlfb[18 + 1];
	char uuid[32 + 1];
	char a5e[18 + 1];
	u8 mac_addr_cnt;
	u8 mac_addr[8][ARP_HLEN];
	char seboot_version[40 + 1];
	u32 ddr_size_mb;
};

/**
 * struct sysinfo_iot2050_priv - sysinfo private data
 * @info: iot2050 board info
 */
struct sysinfo_iot2050_priv {
	struct iot2050_info *info;
};

static int sysinfo_iot2050_detect(struct udevice *dev)
{
	struct sysinfo_iot2050_priv *priv = dev_get_priv(dev);

	log_err("---Entering sysinfo_iot2050_detect\n");

	if (priv->info == NULL || priv->info->magic != IOT2050_INFO_MAGIC)
		return -EFAULT;

	log_err("---priv->info->magic: 0x%08x\n", priv->info->magic);

	return 0;
}

static int sysinfo_iot2050_get_str(struct udevice *dev, int id, size_t size, char *val)
{
	struct sysinfo_iot2050_priv *priv = dev_get_priv(dev);

	switch (id) {
	case BOARD_NAME:
		strncpy(val, priv->info->name, size);
		break;
	case BOARD_SERIAL:
		strncpy(val, priv->info->serial, size);
		break;
	case BOARD_MLFB:
		strncpy(val, priv->info->mlfb, size);
		break;
	case BOARD_UUID:
		strncpy(val, priv->info->uuid, size);
		break;
	case BOARD_A5E:
		strncpy(val, priv->info->a5e, size);
		break;
	case BOARD_SEBOOT_VER:
		strncpy(val, priv->info->seboot_version, size);
		break;
	case BOARD_MAC_ADDR_1:
	case BOARD_MAC_ADDR_2:
	case BOARD_MAC_ADDR_3:
	case BOARD_MAC_ADDR_4:
	case BOARD_MAC_ADDR_5:
	case BOARD_MAC_ADDR_6:
	case BOARD_MAC_ADDR_7:
	case BOARD_MAC_ADDR_8:
		memcpy(val, priv->info->mac_addr[id - BOARD_MAC_ADDR_START],
		       ARP_HLEN);
		log_err("Lee-%s-%d: %s\n", __func__, __LINE__, val);
		return 0;
	case BOARD_DDR_SIZE:
		memcpy(val, &priv->info->ddr_size_mb, sizeof(priv->info->ddr_size_mb));
		log_err("Lee-%s-%d: %s\n", __func__, __LINE__, val);
		return 0;
	default:
		return -EINVAL;
	};

	val[size - 1] = '\0';
	log_err("\n---Exiting %s, val:%s id: %d\n", __func__, val, id);
	return 0;
}

static int sysinfo_iot2050_get_int(struct udevice *dev, int id, char *val)
{
	struct sysinfo_iot2050_priv *priv = dev_get_priv(dev);

	switch (id) {
	case BOARD_MAC_ADDR_CNT:
		*val = priv->info->mac_addr_cnt;
		return 0;
	default:
		return -EINVAL;
	};
}

static const struct sysinfo_ops sysinfo_iot2050_ops = {
	.detect = sysinfo_iot2050_detect,
	.get_str = sysinfo_iot2050_get_str,
	.get_int = sysinfo_iot2050_get_int,
};

static int sysinfo_iot2050_probe(struct udevice *dev)
{
	struct sysinfo_iot2050_priv *priv = dev_get_priv(dev);
	u32 offset;

	offset = dev_read_u32_default(dev, "offset",
				      TI_SRAM_SCRATCH_BOARD_EEPROM_START);
	priv->info = (struct iot2050_info *)(offset);

	return 0;
}

static const struct udevice_id sysinfo_iot2050_ids[] = {
	{ .compatible = "siemens,iot2050-sysinfo" },
	{ /* sentinel */ }
};

U_BOOT_DRIVER(sysinfo_iot2050) = {
	.name           = "sysinfo_iot2050",
	.id             = UCLASS_SYSINFO,
	.of_match       = sysinfo_iot2050_ids,
	.ops		= &sysinfo_iot2050_ops,
	.priv_auto	= sizeof(struct sysinfo_iot2050_priv),
	.probe          = sysinfo_iot2050_probe,
};
