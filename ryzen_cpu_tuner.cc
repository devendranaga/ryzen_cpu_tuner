/**
 * Copyright 2023-present Devendra Naga (devendra.aaru@outlook.com)
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted provided
 * that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*/
#include <iostream>
#include <string.h>
#include <getopt.h>

const std::string CPUINFO_FILE = "/proc/cpuinfo";
const std::string CPUINFO_PATH = "/sys/devices/system/cpu/";
const std::string CPUINFO_CUR_FREQ = "cpufreq/cpuinfo_cur_freq";
const std::string CPUINFO_MIN_FREQ = "cpufreq/cpuinfo_min_freq";
const std::string CPUINFO_MAX_FREQ = "cpufreq/cpuinfo_max_freq";
const std::string SCALING_GOVERNOR = "cpufreq/scaling_governor";
const std::string SCALING_MIN_FREQ = "cpufreq/scaling_min_freq";
const std::string SCALING_MAX_FREQ = "cpufreq/scaling_max_freq";
const std::string SCALING_SET_SPEED = "cpufreq/scaling_setspeed";
const std::string SCALING_AVAILABLE_GOVERNORS = "cpufreq/scaling_available_governors";
const std::string SCALING_AVAILABLE_FREQUENCIES = "cpufreq/scaling_available_frequencies";

static void usage(const char *progname)
{
    fprintf(stderr, "<%s> [l]\n"
                    "\t <-l list cpus>\n"
                    "\t <-p all performance>\n", progname);
}

static void get_content(const std::string path, std::string &val)
{
    char val_str[1024];
    FILE *fp;

    fp = fopen(path.c_str(), "r");
    if (!fp) {
        return;
    }

    fgets(val_str, sizeof(val_str), fp);
    val_str[strlen(val_str) - 1] = '\0';
    fclose(fp);

    val = std::string(val_str);
}

static void print_cpu_info(int index,
                           const std::string &print_name,
                           const std::string &cpu_freq_str,
                           const std::string &ext)
{
    std::string freq_val;
    std::string freq = CPUINFO_PATH + "/cpu" +
                           std::to_string(index) + "/" +
                           cpu_freq_str;
    get_content(freq, freq_val);

    printf("\t %s %s %s\n", print_name.c_str(), freq_val.c_str(), ext.c_str());
}

static int get_num_cpus()
{
    FILE *fp;
    char msg[1024];
    int processor_count = 0;

    fp = fopen(CPUINFO_FILE.c_str(), "r");
    if (!fp) {
        return 0;
    }

    while (fgets(msg, sizeof(msg), fp)) {
        if (strstr(msg, "processor")) {
            processor_count ++;
        }
    }

    fclose(fp);

    return processor_count;
}

static void list_cpus()
{
    int processor_count = get_num_cpus();
    int i;

    printf("num cores: %d\n", processor_count);

    for (i = 0; i < processor_count; i ++) {

        printf("cpu [%d]: {\n", i);
        print_cpu_info(i, "cur_freq:", CPUINFO_CUR_FREQ, "Hz");
        print_cpu_info(i, "min_freq:", CPUINFO_MIN_FREQ, "Hz");
        print_cpu_info(i, "max_freq:", CPUINFO_MAX_FREQ, "Hz");
        print_cpu_info(i, "scaling_governor:", SCALING_GOVERNOR, "");
        print_cpu_info(i, "scaling_min_freq:", SCALING_MIN_FREQ, "Hz");
        print_cpu_info(i, "scaling_max_freq:", SCALING_MAX_FREQ, "Hz");
        print_cpu_info(i, "scaling_governors: [", SCALING_AVAILABLE_GOVERNORS, "]");
        printf("}\n");
    }
}

static void write_content(const std::string &path, const std::string &val)
{
    FILE *fp;

    fp = fopen(path.c_str(), "w");
    if (!fp) {
        return;
    }

    fprintf(fp, "%s", val.c_str());
    fclose(fp);
}

static void set_cpu_governor(int index, const std::string &governor)
{
    std::string governor_str = CPUINFO_PATH + "/cpu" +
                               std::to_string(index) + "/" +
                               SCALING_GOVERNOR;
    write_content(governor_str, governor);
}

static void set_cpu_governor_all(const std::string &governor)
{
    int processor_count = get_num_cpus();
    int i;

    for (i = 0; i < processor_count; i ++) {
        set_cpu_governor(i, governor);
    }
}

static int cpu_tuner(int argc, char **argv)
{
    int ret;

    while ((ret = getopt(argc, argv, "lp")) != -1) {
        switch (ret) {
            case 'l':
                list_cpus();
            break;
            case 'p':
                set_cpu_governor_all("performance");
            break;
            default:
                usage(argv[0]);
                return -1;
        }
    }

    return 0;
}

int main(int argc, char **argv)
{
    cpu_tuner(argc, argv);
}

