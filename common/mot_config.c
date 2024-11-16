#include "mot_config.h"
#include <linux/string.h>

// BEGIN IKSWR-130356
typedef struct moto_product {
	char hw_device[ARRAY_VALUE_MAX];
	char hw_radio[ARRAY_VALUE_MAX];
	char wifi_cfg_name[ARRAY_VALUE_MAX];
	char txpowerctrl_name[ARRAY_VALUE_MAX];
} moto_product;

static moto_product products_list[] = {
		{"ellis",    "NCA", "wifi", "txpowerctrl"},
		{"ellis",    "CA",  "wifi_EPA", "txpowerctrl_EPA"},


		{{0},        {0},   {0},	{0}},
};


void read_cmdline(char* value, char* root_cmdline)
{
	char format[ARRAY_VALUE_MAX] = { 0 };
	char* rootfsmtd_ptr = strstr(saved_command_line, root_cmdline);

	if (rootfsmtd_ptr) {
		memcpy(format, root_cmdline, strlen(root_cmdline));
		strcat(format, "%s");
		DBGLOG(RLM, ERROR, "[MOTO]format:%s\n", format);

		sscanf(rootfsmtd_ptr, format, value);

		DBGLOG(RLM, ERROR, "[MOTO]read_cmdline:%s%s\n", root_cmdline, value);
	}

}

void get_moto_config_file_name(char* name, WIFI_CFG_ENUM index)
{
	char device[ARRAY_VALUE_MAX] = { 0 };
	char radio[ARRAY_VALUE_MAX] = { 0 };

	int num = 0;
	int i = 0;

	DBGLOG(RLM, ERROR, "[MOTO]get_moto_config_file_name.\n");

	read_cmdline(device, "androidboot.device=");
	read_cmdline(radio, "androidboot.radio=");


    num = sizeof(products_list) / sizeof(moto_product);
    for (i = 0; i < num; i++) {
        if (strncmp(device, (products_list + i)->hw_device, ARRAY_VALUE_MAX) == 0) {
            if (strncmp(radio, (products_list + i)->hw_radio, ARRAY_VALUE_MAX) == 0 ||
                strncmp((products_list + i)->hw_radio, "all", ARRAY_VALUE_MAX) == 0) {
                if (index == WIFI_CFG_INDEX) {
                    snprintf(name, ARRAY_VALUE_MAX, "%s.cfg", (products_list + i)->wifi_cfg_name);
                }
                else {
                    snprintf(name, ARRAY_VALUE_MAX, "%s.cfg", (products_list + i)->txpowerctrl_name);
                }
                DBGLOG(RLM, ERROR, "[MOTO]Use moto config file name: %s\n", name);
                return;
            }
        }
    }

}
// END IKSWR-130356