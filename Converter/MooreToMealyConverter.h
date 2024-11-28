#pragma once
#include <memory>

#include "../Automata/MealyAutomata.h"
#include "../Automata/MooreAutomata.h"

class MooreToMealyConverter
{
public:
    static constexpr char STATE_CHAR = 'F';
    static constexpr size_t FIRST_STATE_INDEX = 0;

    explicit MooreToMealyConverter(std::unique_ptr<MooreAutomata> moore)
        : m_moore(std::move(moore))
    {}

    [[nodiscard]] std::unique_ptr<MealyAutomata> GetMealyAutomata() const
    {
        std::vector<std::pair<std::string, std::string>> mooreStatesInfo = m_moore->GetStatesInfo();
        std::list<std::pair<std::string, std::vector<std::string>>> mooreTransitionTable = m_moore->GetTransitionTable();

        auto mealyStates = GetMealyStates(mooreStatesInfo);
        auto mealyTransitionTable = GetMealyTransitionTable(mooreStatesInfo, mooreTransitionTable);

        return std::make_unique<MealyAutomata>(mealyStates, mealyTransitionTable);
    }

private:
    static std::vector<std::string> GetMealyStates(const std::vector<std::pair<std::string, std::string>>& statesInfo)
    {
        std::vector<std::string> mealyStates;
        for (const auto& info : statesInfo)
        {
            std::string state = info.first;
            state[0] = STATE_CHAR;

            mealyStates.emplace_back(state);
        }

        return mealyStates;
    }

    static std::list<std::pair<std::string, std::vector<Transition>>> GetMealyTransitionTable(const std::vector<std::pair<std::string, std::string>>& statesInfo,
        const std::list<std::pair<std::string, std::vector<std::string>>>& mooreTransitionTable)
    {
        std::list<std::pair<std::string, std::vector<Transition>>> mealyTransitionTable;

        auto stateToOutputSymbolMap = GetStateToOutputSymbolMap(statesInfo);

        for (auto& mooreRow : mooreTransitionTable)
        {
            std::pair<std::string, std::vector<Transition>> mealyRow;
            mealyRow.first = mooreRow.first;

            std::vector<Transition>& mealyTransitions = mealyRow.second;
            for (std::string state : mooreRow.second)
            {
                std::string outputSymbol = stateToOutputSymbolMap[state];
                state[0] = STATE_CHAR;

                Transition transition(state, outputSymbol);

                mealyTransitions.emplace_back(transition);
            }

            mealyTransitionTable.emplace_back(std::move(mealyRow));
        }

        return mealyTransitionTable;
    }

    static std::map<std::string, std::string> GetStateToOutputSymbolMap(const std::vector<std::pair<std::string, std::string>>& statesInfo)
    {
        std::map<std::string, std::string> stateToOutputSymbolMap;
        for (const auto& info : statesInfo)
        {
            stateToOutputSymbolMap[info.first] = info.second;
        }

        return stateToOutputSymbolMap;
    }

    std::unique_ptr<MooreAutomata> m_moore;
};