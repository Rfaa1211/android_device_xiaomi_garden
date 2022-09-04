/*
   Copyright (c) 2015, The Linux Foundation. All rights reserved.
   Copyright (C) 2016 The CyanogenMod Project.
   Copyright (C) 2019 The LineageOS Project.
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
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include <android-base/properties.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include "vendor_init.h"
#include "property_service.h"

using android::base::GetProperty;
using android::base::SetProperty;
using std::string;

char const *heapstartsize;
char const *heapgrowthlimit;
char const *heapsize;
char const *heapminfree;
char const *heapmaxfree;
char const *heaptargetutilization;

void property_override(string prop, string value)
{
    auto pi = (prop_info*) __system_property_find(prop.c_str());

    if (pi != nullptr)
        __system_property_update(pi, value.c_str(), value.size());
    else
        __system_property_add(prop.c_str(), prop.size(), value.c_str(), value.size());
}

void check_device()
{
    struct sysinfo sys;

    sysinfo(&sys);

    if (sys.totalram > 5072ull * 1024 * 1024) {
        // from - phone-xhdpi-6144-dalvik-heap.mk
        heapstartsize = "16m";
        heapgrowthlimit = "256m";
        heapsize = "512m";
        heaptargetutilization = "0.5";
        heapminfree = "8m";
        heapmaxfree = "32m";
    } else if (sys.totalram > 3072ull * 1024 * 1024) {
        // from - phone-xxhdpi-4096-dalvik-heap.mk
        heapstartsize = "8m";
        heapgrowthlimit = "256m";
        heapsize = "512m";
        heaptargetutilization = "0.6";
        heapminfree = "8m";
        heapmaxfree = "16m";
    } else {
        // from - phone-xhdpi-2048-dalvik-heap.mk
        heapstartsize = "8m";
        heapgrowthlimit = "192m";
        heapsize = "512m";
        heaptargetutilization = "0.75";
        heapminfree = "512k";
        heapmaxfree = "8m";
    }
}

void vendor_load_properties()
{
    check_device();

    property_override("dalvik.vm.heapstartsize", heapstartsize);
    property_override("dalvik.vm.heapgrowthlimit", heapgrowthlimit);
    property_override("dalvik.vm.heapsize", heapsize);
    property_override("dalvik.vm.heaptargetutilization", heaptargetutilization);
    property_override("dalvik.vm.heapminfree", heapminfree);
    property_override("dalvik.vm.heapmaxfree", heapmaxfree);

    string model = "Redmi MT6765 G  Series";

    // Override odm and vendor partitions' props
    string prop_partitions[] = { "odm.", "vendor." };
    for (const string &prop : prop_partitions) {
        property_override(string("ro.product.") + prop + string("model"), model);
    }

    // Override Privapp permissions whitelisting props
    property_override(string("ro.control_privapp_permissions"), string("log"));

    // Override Power Saving props
    property_override(string("ro.config.hw_power_saving"), string("true"));

    // Override Perf props
    property_override(string("profiler.force_disable_err_rpt"), string("true"));
    property_override(string("profiler.force_disable_ulog"), string("true"));

    // Override GPU Perf props
    property_override(string("debug.composition.type"), string("hw"));

    // Override Debug props
    property_override(string("ro.secure"), string("0"));
    property_override(string("ro.debuggable"), string("1"));

    // Override IMS props
    property_override(string("persist.vendor.vilte_support"), string("0"));

    // Override Charger props
    property_override(string("ro.charger.enable_suspend"), string("true"));

    std::string region = GetProperty("ro.boot.hwc", "");
    std::string product_name = GetProperty("ro.product.name", "");

    if (product_name == "angelica") {
        property_override("ro.product.brand", "Redmi");
        property_override("ro.product.model", "Redmi 9C");
        property_override("ro.product.device", "angelica");
    } else if (product_name == "angelicain") {
        property_override("ro.product.brand", "POCO");
        property_override("ro.product.model", "POCO C3");
        property_override("ro.product.device", "angelicain");
    } else if (product_name == "dandelion_id2") { // Redmi 10A Indonesia
        property_override("ro.product.model", "Redmi 10A");
    } else if (product_name == "angelican") {
        property_override("ro.product.brand", "Redmi");
        property_override("ro.product.model", "Redmi 9C NFC");
        property_override("ro.product.device", "angelican");
    } else if (product_name == "cattail") {
        property_override("ro.product.brand", "Redmi");
        property_override("ro.product.model", "Redmi 9");
        property_override("ro.product.device", "cattail");
    } else if (product_name == "dandelion") {
        property_override("ro.product.brand", "Redmi");
        property_override("ro.product.device", "dandelion");
        if (region == "India_9i") {
            property_override("ro.product.model", "Redmi 9I");
        } else {
            property_override("ro.product.model", "Redmi 9A");
    }
 }
}
