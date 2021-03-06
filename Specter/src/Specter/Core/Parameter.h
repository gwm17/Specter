/*
	Parameter.h
	Contains two data related structures, ParameterData and Parameter. Parameter here refers to a value which
	can be associated with the axis of a histogram. ParameterData is a two component struct which represents the state of a single parameter, which is stored
	in the global SpectrumManager (see SpectrumManager documentation). Parameter is an access point class for use within analysis. It provides scoped access to
	the managed data. There are several important caveats to using the parameters this way, which is mostly related to synchronization:

	- Parameter contains no guarantee of thread safety. THIS IS BY DESIGN. Parameters should only be created within the context of an AnalysisStage (see AnalysisStage documentation).
	  As long as this is true, there is no risk of accessing parameter data outside of the physics thread, and that thread explicitly handles calculation of parameters 
	  and updating of histograms. DO NOT make a Parameter outside of the AnalysisStage context.
	- Parameters must be bound to the active SpectrumManager. This is done using the BindParameter function of the manager; Parameters are keyed based on their name string.
	  If two Parameters are made that are both named "x1", they are in fact the same parameter. In this way, values can be passed from one AnalysisStage to another. If you make a stage 
	  called InitialStage with a Parameter named "x1" where x1 is set to 1.0, and then have a later stage called LaterStage with another Parameter named x1, it implicitly has the value 1.0
	  due to it being set in the previous stage.
	- Each Parameter has a valid flag. This is a boolean which idicates whether or not the parameter is in a valid state (the data event contianed a value  for this parameter). Before using a parameter
	  in a calculation one should check if the parameter is valid using the IsValid() function. When a parameter is set using the SetValue() function, the valid flag is set to true. After the event is completely
	  processed (all analysis stages have been run and histograms have been updated) the manager should be called to run InvalidateParameters() to set all parameters as invalid (valid flag false).

	Those are the key points I can think of now. As more people use the code, I expect this section to grow and change rapidly.

	Credit to nscldaq and in particular NSCLSpecTcl which provided the inspiration for this parameter model.

	GWM -- Feb 2022
*/
#ifndef PARAMETER_H
#define PARAMETER_H

#include "SpecCore.h"

namespace Specter {

	//Underlying data
	struct ParameterData
	{
		double value=0.0;
		bool validFlag=false;
	};

	//Interface to parameter data
	class Parameter
	{

	public:
        Parameter();
		Parameter(const std::string& name);
		~Parameter();
        
        inline bool IsValid() const { return m_pdata->validFlag; }
        inline void Invalidate() { m_pdata->validFlag = false; }
        inline void SetValue(double value) { m_pdata->validFlag = true; m_pdata->value = value; }
        inline double GetValue() const { return m_pdata->value; }
        inline const std::string& GetName() const { return m_name; }
		void SetName(const std::string& name);

		friend class SpectrumManager;
	private:
		std::string m_name;
		std::shared_ptr<ParameterData> m_pdata;

	};

	//Similar to  parameters, sometimes you want to have a numeric input (in calculation terms, a constant)
	//which you can use with your analysis. To be able to expose these numeric values to the UI, we need to implement them
	//in the manager. To help with this, Variables are atomics. So unlike Parameters they are implicity thread safe on read and write.
	//However, this does not mean they can be modified in the analysis! To the AnalysisStage they should be treated as constant, while the UI
	//should view them as modifiable. These are real god damn dangerous, but I think the power they offer outweighs the risk, for now.
	class Variable
	{
	public:
		Variable();
		Variable(const std::string& name);
		~Variable();

		inline void SetValue(double value) { *(m_pdata) = value; }
		inline double GetValue() { return *(m_pdata); }
		inline const std::string& GetName() { return m_name; }
		void SetName(const std::string& name);

		friend class SpectrumManager;
	private:
		std::shared_ptr<std::atomic<double>> m_pdata;
		std::string m_name;
	};

	class Scaler
	{
	public:
		Scaler();
		Scaler(const std::string& name);
		~Scaler();

		inline void Increment() { ++(*m_pdata); }

		inline const std::string& GetName() { return m_name; }
		inline uint64_t GetCounts() { return *m_pdata; }
		void SetName(const std::string& name);

		friend class SpectrumManager;
	private:
		std::shared_ptr<std::atomic<uint64_t>> m_pdata;
		std::string m_name;
	};

}

#endif
