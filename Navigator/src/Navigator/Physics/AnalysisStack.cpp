//
//  AnalysisStack.cpp
//  Navigator
//
//  Created by Gordon McCann on 12/26/21.
//

#include "AnalysisStack.h"

namespace Navigator {

    AnalysisStack::AnalysisStack() {}

    AnalysisStack::~AnalysisStack()
    {
        for(AnalysisStage* stage : m_stack)
            delete stage;
    }

    void AnalysisStack::PushStage(AnalysisStage* stage)
    {
        m_stack.emplace(m_stack.begin()+m_insertIndex, stage);
        m_insertIndex++;
    }

    void AnalysisStack::PopStage(AnalysisStage* stage)
    {
        auto iter = std::find(m_stack.begin(), m_stack.end(), stage);
        if(iter != m_stack.end())
        {
            m_stack.erase(iter);
            m_insertIndex--;
        }
    }

}
