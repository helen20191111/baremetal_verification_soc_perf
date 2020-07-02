#!/usr/bin/python3
import re
import sys


if len(sys.argv) != 2:
    print("argv err!")
    exit()

restr = r'write_reg\(ddr_phy_base_addr\+/\*32\'h\*/(\w+)\*/\*32\'h\*/0x4,/\*32\'h\*/(\w+)\);'

relpddr40 = r'lpddr4_dwc_ddrphy_phyinit_userCustom_customPostTrain'
relpddr41 = r'lpddr4_dwc_ddrphy_phyinit_userCustom_H_readMsgBlock'
relpddr42 = r'lpddr4_dwc_ddrphy_phyinit_userCustom_G_waitFwDone'
relpddr43 = r'lpddr4_dwc_ddrphy_phyinit_userCustom_B_startClockResetPhy'
redelay = r'delay_1k'

lpddr4_data0 = {"name": "lpddr_data0", "str": "", "data": []}
lpddr4_data1 = {"name": "lpddr_data1", "str": "", "data": []}
lpddr4_data2 = {"name": "lpddr_data2", "str": "", "data": []}
lpddr4_data3 = {"name": "lpddr_data3", "str": "", "data": []}
lpddr4_data4 = {"name": "lpddr_data4", "str": "", "data": []}
lpddr4_data5 = {"name": "lpddr_data5", "str": "", "data": []}
lpddr4_data6 = {"name": "lpddr_data6", "str": "", "data": []}
lpddr4_data7 = {"name": "lpddr_data7", "str": "", "data": []}
lpddr4_data8 = {"name": "lpddr_data8", "str": "", "data": []}
lpddr4_data9 = {"name": "lpddr_data9", "str": "", "data": []}

lpddr4_tmp = [lpddr4_data0, lpddr4_data1, lpddr4_data2, lpddr4_data3, \
        lpddr4_data4, lpddr4_data5, lpddr4_data6, lpddr4_data7, lpddr4_data8, lpddr4_data9]
lpddr4_config = []

strhead = '''/*
 * Automatically generated file; DO NOT EDIT.
 */'''

filename = str(sys.argv[1])
basename = filename.split("/")[-1]
objbasename = basename.replace("lpddr4_apb_config", "config")
objname = filename.replace(basename, objbasename)

fr = open(filename, "r")
fw = open(objname, "w")

index = 0
while True:
    line = fr.readline()
    if line:
        ff = re.match(restr, line)
        if ff:
            lpddr4_tmp[index]["data"].append((ff.group(2), ff.group(1)))
        fstr0 = re.match(relpddr40, line)
        if fstr0:
            lpddr4_config.append(lpddr4_tmp[index])
            lpddr4_tmp[index]["str"] = fstr0.group(0) + "();"
            index += 1
        fstr1 = re.match(relpddr41, line)
        if fstr1:
            lpddr4_config.append(lpddr4_tmp[index])
            lpddr4_tmp[index]["str"] = fstr1.group(0) + "(ddr_phy_base_addr);"
            index += 1
        fstr2 = re.match(relpddr42, line)
        if fstr2:
            lpddr4_config.append(lpddr4_tmp[index])
            lpddr4_tmp[index]["str"] = fstr2.group(0) + "(bst_lpddr4_base_addr);"
            index += 1
        fstr3 = re.match(relpddr43, line)
        if fstr3:
            lpddr4_config.append(lpddr4_tmp[index])
            lpddr4_tmp[index]["str"] = fstr3.group(0) + "(name, bst_lpddr4_base_addr);"
            index += 1
        fdelay = re.match(redelay, line)
        if fdelay:
            lpddr4_config.append(lpddr4_tmp[index])
            lpddr4_tmp[index]["str"] = fdelay.group(0) + "(120);"
            index += 1
    else:
        break

fw.write(f'''{strhead}
#include "a1000_evb.h"\n''')

for config in lpddr4_config:
    if not config["data"]:
        continue
    fw.write('''
static u32 %s[][2] = {''' % (config["name"]))
    for data in config["data"]:
        fw.write('''
    {%s, %s},''' % (data[0], data[1]))
    fw.write('''\n};\n''')

fw.write('''void lpddr4_apb_config(char *name, void *bst_lpddr4_base_addr)
{
    int i;
    void *ddr_phy_base_addr = bst_lpddr4_base_addr + 0x2000;\n''')

for config2 in lpddr4_config:

    if config2["data"]:
        fw.write('''\n
    for(i = 0; i < sizeof %s/sizeof %s[0]; i++)
        writel(%s[i][0], ddr_phy_base_addr + %s[i][1] * 0x4);''' % (config2["name"], config2["name"], \
                config2["name"], config2["name"]))
    fw.write('''
    %s''' % (config2["str"]))

fw.write('''\n};\n''')
fr.close()
fw.close()
