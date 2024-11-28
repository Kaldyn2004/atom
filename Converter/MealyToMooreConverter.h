#pragma once
#include <algorithm>
#include <map>
#include <memory>
#include <set>

#include "../Automata/MealyAutomata.h"
#include "../Automata/MooreAutomata.h"

class MealyToMooreConverter
{
public:
    static constexpr char STATE_CHAR = 'q';
    static constexpr size_t FIRST_STATE_INDEX = 0;

    explicit MealyToMooreConverter(std::unique_ptr<MealyAutomata> mealy)
        : m_mealy(std::move(mealy))
    {}

    [[nodiscard]] std::unique_ptr<MooreAutomata> GetMooreAutomata() const
    {
        auto mealyTransitionTable = m_mealy->GetTransition();
        auto mealyStates = m_mealy->GetStates();
        auto inputSymbols = m_mealy->GetInputSymbols();

        auto uniqueTransitions = GetUniqueTransitions(mealyTransitionTable, mealyStates);

        auto stateToTransitions = GetStateToTransitionsMap(uniqueTransitions);

        auto transitionToNewStateName = GetNewStateNamesFromTransitions(uniqueTransitions, mealyStates, stateToTransitions);

        auto mooreStatesInfo = GetMooreStatesInfo(transitionToNewStateName);

        std::list<std::pair<std::string, std::vector<std::string>>> mooreTransitionTable;
        for (auto& row: mealyTransitionTable)
        {
            std::string inputSymbol = row.first;
            std::vector<std::string> states;
            for (unsigned index = 0; auto& it: row.second)
            {
                const std::string& state = mealyStates[index++];
                std::string newState = transitionToNewStateName[it];

                auto transitionsCount = GetTransitionsCountWithEqualState(state, stateToTransitions);

                states.insert(states.end(), transitionsCount, newState);
            }
            mooreTransitionTable.emplace_back(inputSymbol, states);
        }

        return std::make_unique<MooreAutomata>(
            std::move(inputSymbols),
            std::move(mooreStatesInfo),
            std::move(mooreTransitionTable));
    }

private:
    static unsigned GetTransitionsCountWithEqualState(const std::string& state,
        std::map<std::string, std::set<Transition>>& stateToTransitions)
    {
        return stateToTransitions[state].size();
    }

    static std::vector<std::pair<std::string, std::string>> GetMooreStatesInfo(std::map<Transition, std::string>& transitionToNewStateName)
    {
        std::vector<std::pair<std::string, std::string>> mooreStateInfo;
        for (auto& it: transitionToNewStateName)
        {
            mooreStateInfo.emplace_back(it.second, it.first.outputSymbol);
        }

        std::sort(mooreStateInfo.begin(), mooreStateInfo.end(), SortStringFromIndexesComp);

        return mooreStateInfo;
    }

    static std::map<std::string, std::set<Transition>> GetStateToTransitionsMap(std::vector<Transition>& transitions)
    {
        std::map<std::string, std::set<Transition>> stateToTransitions;
        for (auto& transition: transitions)
        {
            if (!stateToTransitions.contains(transition.nextState))
            {
                stateToTransitions[transition.nextState] = {};
            }

            if (!stateToTransitions[transition.nextState].contains(transition))
            {
                stateToTransitions[transition.nextState].emplace(transition);
            }
        }

        return stateToTransitions;
    }

    static std::map<Transition, std::string> GetNewStateNamesFromTransitions(std::vector<Transition>& transitions,
                                                                             std::vector<std::string>& states, std::map<std::string, std::set<Transition>>& stateToTransitions)
    {
        std::map<Transition, std::string> transitionToNewStateNames;
        for (unsigned index = FIRST_STATE_INDEX; auto& state: states)
        {
            for (auto& transition: stateToTransitions[state])
            {
                transitionToNewStateNames[transition] = STATE_CHAR + std::to_string(index++);
            }
        }

        return transitionToNewStateNames;
    }

    static std::vector<Transition> GetUniqueTransitions(std::list<std::pair<std::string, std::vector<Transition>>>& transitionTable, std::vector<std::string>& states)
    {
        std::set<std::string> statesInTransitions;
        std::set<Transition> uniqueTransitions;
        std::vector<Transition> uniqueTransitionVector;

        for (auto& row: transitionTable)
        {
            for (auto& transition: row.second)
            {
                if (!uniqueTransitions.contains(transition))
                {
                    uniqueTransitions.emplace(transition);
                    uniqueTransitionVector.emplace_back(transition);
                    statesInTransitions.emplace(transition.nextState);
                }
            }
        }

        for (auto& it: states)
        {
            if (!statesInTransitions.contains(it))
            {
                uniqueTransitionVector.emplace_back(it, "");
                uniqueTransitions.emplace(it, "");
            }
        }

        return uniqueTransitionVector;
    }

    static bool SortStringFromIndexesComp(const std::pair<std::string, std::string>& a, const std::pair<std::string, std::string>& b)
    {
        int numA = std::stoi(a.first.substr(1));
        int numB = std::stoi(b.first.substr(1));

        return numA < numB;
    }

    std::unique_ptr<MealyAutomata> m_mealy;
};
