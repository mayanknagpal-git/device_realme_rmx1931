/*
   Copyright (c) 2015, The Linux Foundation. All rights reserved.
   Copyright (C) 2016 The CyanogenMod Project.
   Copyright (C) 2019-2020 The LineageOS Project.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cstdlib>
#include <string.h>
#include <fstream>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <android-base/properties.h>

#include "property_service.h"
#include "vendor_init.h"

using android::base::GetProperty;
using std::string;

void property_override(char const prop[], char const value[]) {
    auto pi = (prop_info*) __system_property_find(prop);

    if (pi != nullptr)
        __system_property_update(pi, value, strlen(value));
    else
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void set_ro_build_prop(const string &source, const string &prop,
                        const string &value, bool product = true) {
    string prop_name;

    if (product)
        prop_name = "ro.product." + source + prop;
    else
        prop_name = "ro." + source + "build." + prop;

    property_override(prop_name.c_str(), value.c_str());
}

void vendor_load_properties() {
    /*
     * Detect device and configure properties
     */
    std::ifstream infile("/proc/oppoVersion/operatorName");
    string operatorName;
    string device;
    string model;

    getline(infile, operatorName);

    switch (stoi(operatorName)) {
        case 5:
            device = "RMX1931L1";
            model = "RMX1931";
            break;
        // case 6: 
        //     device = "RMX1931EX";
        //     model = "RMX1931";
        //     break;
        // case 7: 
        //     device = "RMX1931";
        //     model = "RMX1931";
        //     break;
        default:
            device = "RMX1931";
            model = "RMX1931";
    }

    // list of partitions to override props
    string source_partitions[] = { "", "bootimage.", "odm.", "product.",
                                   "system.", "system_ext.", "vendor." };

    for (const string &source : source_partitions) {
        set_ro_build_prop(source, "device", device);
        set_ro_build_prop(source, "product", model, false);
        set_ro_build_prop(source, "model", model);
        set_ro_build_prop(source, "name", model);
    }

    // property_override("ro.vendor.version_variant", project.c_str());
    property_override("ro.product.marketname", "realme X2 Pro");
}