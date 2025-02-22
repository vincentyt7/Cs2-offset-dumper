#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <map>

#ifdef _WIN32
#include <windows.h>
#endif

void downloadOffsets(const std::string& url, const std::string& outputFile) {
    std::string command = "curl -s -L " + url + " -o " + outputFile;
    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "Failed to download offsets.\n";
    }
    else {
        std::cout << "Offsets successfully downloaded and saved to " << outputFile << "\n";
    }
}

bool checkOffsets(const std::string& filePath, const std::string& outputTxt) {
    std::ifstream file(filePath);
    std::ofstream outputFile(outputTxt);

    if (!file || !outputFile) {
        std::cerr << "Failed to open offsets file or create output file.\n";
        return false;
    }

    std::string line;
    std::map<std::string, std::string> foundOffsets;
    std::string requiredOffsets[] = {
        "dwCSGOInput", "dwEntityList", "dwGameEntitySystem", "dwGameEntitySystem_highestEntityIndex",
        "dwGameRules", "dwGlobalVars", "dwGlowManager", "dwLocalPlayerController", "dwLocalPlayerPawn",
        "dwPlantedC4", "dwPrediction", "dwSensitivity", "dwSensitivity_sensitivity", "dwViewAngles",
        "dwViewMatrix", "dwViewRender", "dwWeaponC4", "dwBuildNumber", "dwNetworkGameClient",
        "dwNetworkGameClient_clientTickCount", "dwNetworkGameClient_deltaTick", "dwNetworkGameClient_isBackgroundMap",
        "dwNetworkGameClient_localPlayer", "dwNetworkGameClient_maxClients", "dwNetworkGameClient_serverTickCount",
        "dwNetworkGameClient_signOnState", "dwWindowHeight", "dwWindowWidth", "dwInputSystem",
        "dwGameTypes", "dwGameTypes_mapName", "dwSoundSystem", "dwSoundSystem_engineViewData"
    };

    while (std::getline(file, line)) {
        for (const auto& offset : requiredOffsets) {
            if (line.find(offset) != std::string::npos) {
                std::istringstream iss(line);
                std::string name, equals, address;
                iss >> name >> equals >> address; 

                if (!address.empty()) {
                    foundOffsets[name] = address;
                }
            }
        }
    }

    outputFile << "CS2 Offsets:\n";
    for (const auto& offset : requiredOffsets) {
        if (foundOffsets.find(offset) != foundOffsets.end()) {
            outputFile << offset << " = " << foundOffsets[offset] << "\n";
        }
        else {
            std::cerr << "Warning: Offset missing -> " << offset << "\n";
        }
    }

    file.close();
    outputFile.close();
    return true;
}

int main() {
    std::string url = "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/offsets.cs";
    std::string outputFile = "offsets.cs";
    std::string outputTxt = "offsets_output.txt";

    std::cout << "Updating CS2 offsets...\n";
    downloadOffsets(url, outputFile);

    std::cout << "Checking offsets...\n";
    if (checkOffsets(outputFile, outputTxt)) {
        std::cout << "Offsets successfully saved to " << outputTxt << "\n";
    }
    else {
        std::cerr << "Some offsets may be missing or outdated. Check " << outputTxt << " for details.\n";
    }

    std::cout << "Done.\n";
    return 0;
}
