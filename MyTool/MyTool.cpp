#include <fstream>
#include <iostream>
#include <regex>

#include "json.hpp"
using json = nlohmann::json;

int main(int argc, char* argv[])
{
    // if (argc < 4 || (std::string(argv[2]) != "show-infos" && std::string(argv[2]) != "build" && std::string(argv[2]) != "package")) {
    //     std::cout << "Usage : MyTool [CHEMIN DU UPROJECT] show-infos/build/package" << std::endl;
    //     return 0;
    // }

    std::string path = argv[1];
    std::string command = argv[2];
    std::string packagepath;
    if (argc > 3) {
        packagepath = argv[3];
    } else {
        packagepath = ""; // Valeur par défaut si aucun argument n'est fourni
    }
    
    if(command == "show-infos")
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            std::cout << "Error opening file " << path << std::endl;
            return 1;
        }

        try
        {
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

            if (std::regex_match(engineVersion, versionRegex))
            {
                std::cout << "Version d'Unreal Engine : " << engineVersion << std::endl;
            }
            else
            {
                std::cout << "Version d'Unreal Engine : From Source"<< std::endl;
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
        } catch (std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
        
    };

    if(command == "build")
    {
        std::string command;
        std::string project_name = path.substr(path.find_last_of("/\\") + 1);
        project_name = project_name.substr(0, project_name.find_last_of(".")); 
        command = "powershell /C \"./Engine/Build/BatchFiles/Build.bat " + 
                  project_name + " Win64 Development \"" + 
                  path + "\" -waitmutex\"";

        int result = std::system(command.c_str());
        if (result == 0)
            std::cout << "Build terminé avec succès !" << std::endl;
        else
            std::cout << "Erreur lors du build. Code : " << result << std::endl;
    };

    if(command == "package")
    {
        std::string command;
        command = "powershell /C ./Engine/Build/BatchFiles/RunUAT.bat -ScriptsForProject=" + path + "BuildCookRun -project=" + path + " -noP4 -clientconfig=Shipping -serverconfig=Shipping -nocompileeditor -utf8output -platform=Win64 -build -cook -unversionedcookedcontent -stage -package -archive -archivedirectory=" + packagepath;

        int result = std::system(command.c_str());
        if (result == 0)
            std::cout << "Packaging terminé avec succès !" << std::endl;
        else
            std::cout << "Erreur lors du packaging. Code : " << result << std::endl;
    };
    return 0;
}
