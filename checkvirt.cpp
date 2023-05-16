#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

// 检测KVM
bool detect_kvm()
{
    bool detected = false;

    // 检测是否加载了KVM模块
    ifstream f("/proc/modules");
    string line;
    while (getline(f, line)) {
        if (line.find("kvm") != string::npos) {
            detected = true;
            break;
        }
    }
    f.close();

    if (!detected) {
        // 检测是否支持Intel VT或AMD-V
        f.open("/proc/cpuinfo");
        while (getline(f, line)) {
            if (line.find("vmx") != string::npos || line.find("svm") != string::npos) {
                detected = true;
                break;
            }
        }
        f.close();
    }

    return detected;
}

// 检测VMware
bool detect_vmware()
{
    bool detected = false;

    // 检测是否加载了vmw_vmci和vmw_vsock模块
    ifstream f("/proc/modules");
    string line;
    bool vmw_vmci_loaded = false;
    bool vmw_vsock_loaded = false;
    while (getline(f, line)) {
        if (line.find("vmw_vmci") != string::npos) {
            vmw_vmci_loaded = true;
        }
        if (line.find("vmw_vsock") != string::npos) {
            vmw_vsock_loaded = true;
        }
    }
    f.close();

    if (vmw_vmci_loaded && vmw_vsock_loaded) {
        detected = true;
    }

    if (!detected) {
        // 检测是否为VMware Virtual Platform
        stringstream ss;
        ss << "/usr/sbin/dmidecode -s system-product-name";
        string command = ss.str();
        string output;
        const int LINE_LENGTH = 1024;
        char line[LINE_LENGTH];
        FILE* fp = popen(command.c_str(), "r");
        if (fp) {
            while (fgets(line, LINE_LENGTH, fp)) {
                output += line;
            }
            pclose(fp);
        }

        if (output.find("VMware Virtual Platform") != string::npos) {
            detected = true;
        }
    }

    return detected;
}

// 检测Xen
bool detect_xen()
{
    bool detected = false;

    // 检测是否加载了xen模块
    ifstream f("/proc/modules");
    string line;
    while (getline(f, line)) {
        if (line.find("xen") != string::npos) {
            detected = true;
            break;
        }
    }
    f.close();

    if (!detected) {
        // 检测是否支持Xen
        f.open("/proc/cpuinfo");
        while (getline(f, line)) {
            if (line.find("hypervisor") != string::npos && line.find("Xen") != string::npos) {
                detected = true;
                break;
            }
        }
        f.close();
    }

    return detected;
}

// 检测VirtualBox
bool detect_virtualbox()
{
    bool detected = false;

    // 检测是否加载了vboxguest和vboxsf模块
    ifstream f("/proc/modules");
    string line;
    bool vboxguest_loaded = false;
    bool vboxsf_loaded = false;
    while (getline(f, line)) {
        if (line.find("vboxguest") != string::npos) {
            vboxguest_loaded = true;
        }
        if (line.find("vboxsf") != string::npos) {
            vboxsf_loaded = true;
        }
    }
    f.close();

    if (vboxguest_loaded && vboxsf_loaded) {
        detected = true;
    }

    if (!detected) {
        // 检测是否为VirtualBox
        stringstream ss;
        ss << "/usr/sbin/dmidecode -s system-product-name";
        string command = ss.str();
        string output;
        const int LINE_LENGTH = 1024;
        char line[LINE_LENGTH];
        FILE* fp = popen(command.c_str(), "r");
        if (fp) {
            while (fgets(line, LINE_LENGTH, fp)) {
                output += line;
            }
            pclose(fp);
        }

        if (output.find("VirtualBox") != string::npos) {
            detected = true;
        }
    }

    return detected;
}

// 检测虚拟化类型
string detect_virtualization_type()
{
    string virtualization_type = "unknown";

    if (detect_kvm()) {
        virtualization_type = "KVM";
    }
    else if (detect_vmware()) {
        virtualization_type = "VMware";
    }
    else if (detect_xen()) {
        virtualization_type = "Xen";
    }
    else if (detect_virtualbox()) {
        virtualization_type = "VirtualBox";
    }

    return virtualization_type;
}

#ifdef _WIN32
// 在Windows平台上运行的代码
#include <windows.h>
#include <intrin.h>

// 检测是否支持硬件虚拟化
bool detect_hardware_virtualization_support()
{
    int cpu_info[4];
    __cpuid(cpu_info, 1);

    return ((cpu_info[2] & (1 << 5)) != 0);
}

int main()
{
    // 检测虚拟化类型和硬件虚拟化支持
    string virtualization_type = detect_virtualization_type();
    bool hardware_virtualization_support = detect_hardware_virtualization_support();

    cout << "Virtualization type: " << virtualization_type << endl;
    cout << "Hardware virtualization support: " << (hardware_virtualization_support ? "Yes" : "No") << endl;

    return 0;
}
#else
// 在Linux平台上运行的代码
#include <unistd.h>

// 检测是否为虚拟机
bool detect_virtual_machine()
{
    return (access("/proc/vz", F_OK) == 0 || access("/proc/virtualization", F_OK) == 0);
}

int main()
{
    // 检测虚拟化类型和虚拟机状态
    string virtualization_type = detect_virtualization_type();
    bool virtual_machine = detect_virtual_machine();

    cout << "Virtualization type: " << virtualization_type << endl;
    //cout << "Virtual machine: " << (virtual_machine ? "Yes" : "No") << endl;

    return 0;
}
#endif
