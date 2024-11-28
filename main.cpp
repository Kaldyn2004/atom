#include "Automata/MealyAutomata.h"
#include "Automata/MooreAutomata.h"
#include "Converter/MealyToMooreConverter.h"
#include "Converter/MooreToMealyConverter.h"
#include <memory>
#include <iostream>
#include <string>

void convertMealyToMoore(const std::string& mealyFile, const std::string& mooreFile)
{
    try
    {
        std::unique_ptr<MealyAutomata> mealy = std::make_unique<MealyAutomata>();
        std::cout << "Reading Mealy Automata from file: " << mealyFile << std::endl;
        mealy->ReadFromFile(mealyFile);

        MealyToMooreConverter converter(std::move(mealy));
        auto moore = converter.GetMooreAutomata();

        std::cout << "Writing Moore Automata to file: " << mooreFile << std::endl;
        moore->PrintToFile(mooreFile);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error during Mealy to Moore conversion: " << e.what() << std::endl;
    }
}

void convertMooreToMealy(const std::string& mooreFile, const std::string& mealyFile)
{
    try
    {
        std::unique_ptr<MooreAutomata> moore = std::make_unique<MooreAutomata>();
        std::cout << "Reading Moore Automata from file: " << mooreFile << std::endl;
        moore->ReadFromFile(mooreFile);

        MooreToMealyConverter converter(std::move(moore));
        auto mealy = converter.GetMealyAutomata();

        std::cout << "Writing Mealy Automata to file: " << mealyFile << std::endl;
        mealy->PrintToFile(mealyFile);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error during Moore to Mealy conversion: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        std::cerr << "Wrong input data" << std::endl;
        std::cerr << "Usage: " << argv[0] << " mealy-to-moore <mealy.csv> <moore.csv>" << std::endl;
        std::cerr << "   or: " << argv[0] << " moore-to-mealy <moore.csv> <mealy.csv>" << std::endl;
        return 1;
    }

    std::string command = argv[1];
    std::string inputFile = argv[2];
    std::string outputFile = argv[3];

    if (command == "mealy-to-moore")
    {
        convertMealyToMoore(inputFile, outputFile);
    }
    else if (command == "moore-to-mealy")
    {
        convertMooreToMealy(inputFile, outputFile);
    }
    else
    {
        std::cerr << "Unknown command: " << command << std::endl;
        return 1;
    }

    return 0;
}
