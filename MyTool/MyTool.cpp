#include <fstream>
#include <iostream>
#include <regex>
#include <cstdlib>

#include "json.hpp"

using json = nlohmann::json;

void showProjectInfos(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "Error opening file " << path << std::endl;
        return;
    }

    try {
        json data;
        file >> data;

        std::string projectName = "Nom inconnu";
        if (data.contains("Modules")) {
            const auto& modules = data["Modules"];
            if (!modules.empty() && modules[0].contains("Name")) {
                projectName = modules[0].value("Name", "Nom inconnu");
            }
        }
        std::cout << "Nom du projet : " << projectName << std::endl;

        std::regex versionRegex("^\\d+\\.\\d+$");
        std::string engineVersion = data.value("EngineAssociation", "Version inconnue");

        if (std::regex_match(engineVersion, versionRegex)) {
            std::cout << "Version d'Unreal Engine : " << engineVersion << std::endl;
        } else {
            std::cout << "Version d'Unreal Engine : From Source" << std::endl;
        }

        if (data.contains("Plugins")) {
            std::cout << "Plugins utilises :" << std::endl;
            for (const auto& plugin : data["Plugins"]) {
                std::string pluginName = plugin.value("Name", "Nom inconnu");
                bool isEnabled = plugin.value("Enabled", false);

                std::cout << "- " << pluginName;
                if (isEnabled) {
                    std::cout << " (Active)";
                } else {
                    std::cout << " (Desactive)";
                }
                std::cout << std::endl;
            }
        } else {
            std::cout << "Aucun plugin utilise." << std::endl;
        }
    } catch (std::exception& e) {
        std::cout << "Erreur : " << e.what() << std::endl;
    }
}

void buildProject(const std::string& project_path) {
    std::string command;

#ifdef _WIN32
    command = "powershell /C \"./Engine/Build/BatchFiles/Build.bat " + project_path + " Development Win64 -waitmutex\"";
#elif __APPLE__
    command = "sh -c \"./Engine/Build/BatchFiles/Mac/Build.sh " + project_path + " Development Mac -waitmutex\"";
#else
    std::cout << "Build non supporté sur cette plateforme." << std::endl;
    return;
#endif

    int result = std::system(command.c_str());
    if (result == 0)
        std::cout << "Build terminé avec succès !" << std::endl;
    else
        std::cout << "Erreur lors du build. Code : " << result << std::endl;
}

// Fonction pour packager le projet Unreal
void packageProject(const std::string& project_path, const std::string& project_package_path) {
    std::string command;

#ifdef _WIN32
    command = "./Engine/Build/BatchFiles/RunUAT.bat -ScriptsForProject=" + project_path + " BuildCookRun -project=" + project_path + " -noP4 -clientconfig=Shipping -serverconfig=Shipping -nocompileeditor -utf8output -platform=Win64 -build -cook -unversionedcookedcontent -stage -package -archive -archivedirectory="+project_package_path;
#elif __APPLE__
    command = "sh -c \"./Engine/Build/BatchFiles/RunUAT.sh BuildCookRun -project=\"" + project_path + "\" -noP4 -platform=Mac -clientconfig=Shipping -serverconfig=Shipping -nocompile -nocompileeditor -utf8output\"";
#else
    std::cout << "Packaging non supporté sur cette plateforme." << std::endl;
    return;
#endif

    int result = std::system(command.c_str());
    if (result == 0)
        std::cout << "Packaging terminé avec succès !" << std::endl;
    else
        std::cout << "Erreur lors du packaging. Code : " << result << std::endl;
}

int main(int argc, char* argv[]) {
    if ((std::string(argv[2]) != "show-infos" && std::string(argv[2]) != "build" && std::string(argv[2]) != "package")) {
        std::cout << "Usage : MyTool [CHEMIN DU UPROJECT] show-infos/build/package" << std::endl;
        return 0;
    }

    std::string path = argv[1];
    std::string project_package = argv[3];
    std::string command = argv[2];

    if (command == "show-infos") {
        showProjectInfos(path);
    } else if (command == "build") {
        buildProject(path);
    } else if (command == "package") {
        packageProject(path,project_package);
    }

    return 0;
}
