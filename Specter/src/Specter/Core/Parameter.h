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

	Variables added; similar to Parameters, but intend to be an interface with UI feedback. See SpecProject for examples. -- GWM April 2023

	Scalers added. In nuclear phyiscs, scalers refer to time counters of data, to track the rate of different detector components. -- GWM April 2023
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
        
        bool IsValid() const { return m_pdata->validFlag; }
        void Invalidate() { m_pdata->validFlag = false; }
        void SetValue(double value) { m_pdata->validFlag = true; m_pdata->value = value; }
        double GetValue() const { return m_pdata->value; }
        const std::string& GetName() const { return m_name; }
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
	struct VariableData
	{
		std::atomic<double> value = 0.0;
		bool isValid = true;
	};

	class Variable
	{
	public:
		Variable();
		Variable(const std::string& name);
		~Variable();

		void SetValue(double value) { m_pdata->value = value; }
		double GetValue() { return m_pdata->value; }
		const std::string& GetName() { return m_name; }
		void SetName(const std::string& name);

		friend class SpectrumManager;
	private:
		std::shared_ptr<VariableData> m_pdata;
		std::string m_name;
	};


	//Similar to parameters are scalers. Scalers are basically just counters tied to specific
	//data channels in an experiment. They can be quite useful for checking the status of
	//various experimental components. In our implementation, simply bind a Scaler as you would do for a
	//Parameter, and then call the Increment() function when the appropriate data channel is present.
	struct ScalerData
	{
		std::atomic<uint64_t> value;
		bool isValid = true;
	};

	class Scaler
	{
	public:
		Scaler();
		Scaler(const std::string& name);
		~Scaler();

		void Increment() { ++(m_pdata->value); }
		const std::string& GetName() { return m_name; }
		uint64_t GetCounts() { return m_pdata->value; }
		void SetName(const std::string& name);

		friend class SpectrumManager;
	private:
		std::shared_ptr<ScalerData> m_pdata;
		std::string m_name;
	};

}

#endif
