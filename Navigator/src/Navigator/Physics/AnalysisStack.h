/*
    AnalysisStack.h
    Container for the analyses in the PhysicsLayer. Really just a specialized wrapper around std::vector.

    GWM -- Feb 2022
*/

#ifndef ANALYSIS_STACK_H
#define ANALYSIS_STACK_H

#include "AnalysisStage.h"
#include "Navigator/NavCore.h"

namespace Navigator {

    class NAV_API AnalysisStack
    {
    public:
        AnalysisStack();
        ~AnalysisStack();
        
        void PushStage(AnalysisStage* stage);
        void PopStage(AnalysisStage* stage);
        
        std::vector<AnalysisStage*>::iterator begin() { return m_stack.begin(); }
        std::vector<AnalysisStage*>::iterator end() { return m_stack.end(); }
    private:
        std::vector<AnalysisStage*> m_stack; //The analysis stack owns the analysis stages
        unsigned int m_insertIndex=0;
    };

}


#endif