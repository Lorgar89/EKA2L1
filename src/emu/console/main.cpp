#include <cstring>
#include <iostream>
#include <string>

#include <yaml-cpp/yaml.h>
#include <loader/rom.h>
#include <common/log.h>
#include <common/cvt.h>
#include <core.h>

using namespace eka2l1;

std::string rom_path = "SYM.ROM";
std::string sis_install_path = "-1";

uint8_t adrive;

eka2l1::system symsys;
eka2l1::epocver ever = epocver::epoc9;

YAML::Node config;

int app_idx = -1;

bool help_printed = false;
bool list_app = false;

bool quit = false;

void print_help() {
    std::cout << "Usage: Drag and drop Symbian file here, ignore missing dependencies" << std::endl;
    std::cout << "Options: " << std::endl;
    std::cout << "\t -rom: Specified where the ROM is located. If none is specified, the emu will look for a file named SYM.ROM." << std::endl; 
	std::cout << "\t -ver: Specified Symbian version to emulate (either 6 or 9)." << std::endl;
	std::cout << "\t -app: Specified the app to run. Next to this option is the index number." << std::endl;
	std::cout << "\t -listapp: List all of the apps." << std::endl;
	std::cout << "\t -install: Install a SIS/SISX package" << std::endl;
	std::cout << "\t -h/-help: Print help" << std::endl;
}

void parse_args(int argc, char** argv) {
	if (argc <= 1) {
		print_help();
		quit = true;
		return;
	}

    for (int i = 1; i < argc - 1; i++) {
        if (strncmp(argv[i], "-rom", 4) == 0) {
            rom_path = argv[++i];
			config["rom_path"] = rom_path;
        } else if (((strncmp(argv[i], "-h", 2)) == 0 || (strncmp(argv[i], "-help", 5) == 0))
            && (!help_printed)) {
            print_help();
            help_printed = true;
		}
		else if ((strncmp(argv[i], "-ver", 4) == 0 || (strncmp(argv[i], "-v", 2) == 0))) {
			int ver = std::atoi(argv[++i]);

			if (ver == 6) {
				ever = epocver::epoc6;
				config["epoc_ver"] = (int)ever;
			} else {
				ever = epocver::epoc9;
				config["epoc_ver"] = (int)ever;
			}
		} else if (strncmp(argv[i], "-app", 4) == 0) {
			app_idx = std::atoi(argv[++i]);
		} else if (strncmp(argv[i], "-listapp", 8) == 0) {
			list_app = true;
		}
		else if (strncmp(argv[i], "-install", 8) == 0) {
			adrive = std::atoi(argv[++i]);
			sis_install_path = argv[++i];
		}
    }
}

void read_config() {
	try {
		config = YAML::LoadFile("config.yml");
	}
	catch (YAML::BadFile* badboy) {
		LOG_ERROR("YAML-cpp error: {}", badboy->msg);
		LOG_INFO("Can not load config, use default configuration");
		return;
	}

	rom_path = config["rom_path"].as<std::string>();
	ever = (eka2l1::epocver)(config["epoc_ver"].as<int>());
}

void do_args() {
	auto infos = symsys.app_infos();

	if (list_app) {
		for (auto& info : infos) {
			std::cout << info.id << common::ucs2_to_utf8(info.name) << " [drive: " << ((info.drive == 0) ? 'C' : 'E')
				<< " , executable name: " << common::ucs2_to_utf8(info.executable_name) << "]";
		}

		quit = true;
		return;
	} 

	if (app_idx > -1) {
		if (app_idx >= infos.size()) {
			LOG_ERROR("Invalid app index.");
			quit = true;
			return;
		}

		symsys.load(infos[app_idx].id);
		return;
	}

	if (sis_install_path != "-1") {
		symsys.install_package(std::u16string(sis_install_path.begin(), sis_install_path.end()), adrive);
	}
}

void init() {
	symsys.set_symbian_version_use(ever);

    symsys.init();
    bool res = symsys.load_rom(rom_path);
}

void shutdown() {
    symsys.shutdown();
}

int main(int argc, char** argv) {
    std::cout << "-------------- EKA2L1: Experimental Symbian Emulator -----------------" << std::endl;

	log::setup_log(nullptr);

	read_config();
    parse_args(argc, argv);

	if (quit) {
		return 0;
	}

	init();
	do_args();

	if (quit) {
		return 0;
	}

    return 0;
}