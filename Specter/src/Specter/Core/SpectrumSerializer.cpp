/*
	SpectrumSerializer.h
	SpectrumSerializer class providing method to write/read spectra (histograms and cuts) to/from a .spec file. These are formated text files.
	Note that by virtue of the way that cuts work, they are written first.

	A couple of notes:
	- Writing/reading data is a pretty expensive concept from a thread-locking perspective. Not recommended for use during active analysis.
	- Deserializing (reading) by default removes all current histograms and cuts. This avoids any issues with collisions, but may not always be desireable.
	- There is no intrinsic checking of whether a parameter for a cut/histogram to be loaded exists within the current project. If you load something and the histograms 
	  don't fill, it is most likely due to the parameter not being defined in the current project.

	GWM -- Feb 2022
*/
#include "SpectrumSerializer.h"
#include "SpectrumManager.h"

#include <fstream>
#include "yaml-cpp/yaml.h"

namespace Specter {

	static void SerializeCut(YAML::Emitter& output, const CutArgs& args)
	{
		SpectrumManager& manager = SpectrumManager::GetInstance();
		output << YAML::BeginMap;
		output << YAML::Key << "Cut" << YAML::Value << args.name;
		output << YAML::Key << "Type" << YAML::Value << ConvertCutTypeToString(args.type);
		output << YAML::Key << "XParameter" << YAML::Value << args.x_par;
		output << YAML::Key << "YParameter" << YAML::Value << args.y_par;
		if (args.type == CutType::Cut1D)
		{
			std::vector<double> xpoints = manager.GetCutXPoints(args.name);
			output << YAML::Key << "XMin" << YAML::Value << xpoints[0];
			output << YAML::Key << "XMax" << YAML::Value << xpoints[1];
		}
		else if (args.type == CutType::Cut2D)
		{
			std::vector<double> xpoints = manager.GetCutXPoints(args.name);
			std::vector<double> ypoints = manager.GetCutYPoints(args.name);
			output << YAML::Key << "XPoints" << YAML::Value << xpoints;
			output << YAML::Key << "YPoints" << YAML::Value << ypoints;
		}
		else if (args.type == CutType::CutSummaryAll || args.type == CutType::CutSummaryAny)
		{
			std::vector<std::string> subhistos = manager.GetCutSubHistograms(args.name);
			std::vector<double> xpoints = manager.GetCutXPoints(args.name);
			output << YAML::Key << "XMin" << YAML::Value << xpoints[0];
			output << YAML::Key << "XMax" << YAML::Value << xpoints[1];
			output << YAML::Key << "SubHistos" << YAML::Value << subhistos;
		}
		output << YAML::EndMap;
	}

	static void SerializeHistogram(YAML::Emitter& output, const HistogramArgs& args)
	{
		output << YAML::BeginMap;
		output << YAML::Key << "Histogram" << YAML::Value << args.name;
		output << YAML::Key << "Type" << YAML::Value << ConvertSpectrumTypeToString(args.type);
		output << YAML::Key << "XParameter" << YAML::Value << args.x_par;
		output << YAML::Key << "XMin" << YAML::Value << args.min_x;
		output << YAML::Key << "XMax" << YAML::Value << args.max_x;
		output << YAML::Key << "XBins" << YAML::Value << args.nbins_x;
		output << YAML::Key << "YParameter" << YAML::Key << args.y_par;
		output << YAML::Key << "YMin" << YAML::Value << args.min_y;
		output << YAML::Key << "YMax" << YAML::Value << args.max_y;
		output << YAML::Key << "YBins" << YAML::Value << args.nbins_y;
		if (args.type == SpectrumType::Summary)
		{
			std::vector<std::string> subhistos = SpectrumManager::GetInstance().GetSubHistograms(args.name);
			output << YAML::Key << "SubHistos" << YAML::Value << subhistos;
		}
		output << YAML::Key << "CutsDrawn" << YAML::Value << args.cutsDrawnUpon;
		output << YAML::Key << "CutsApplied" << YAML::Value << args.cutsAppliedTo;
		output << YAML::EndMap;

	}

	SpectrumSerializer::SpectrumSerializer(const std::string& filepath) :
		m_filename(filepath)
	{
	}

	SpectrumSerializer::~SpectrumSerializer() {}

	void SpectrumSerializer::SerializeData(const std::vector<HistogramArgs>& histoList, const std::vector<CutArgs>& cutList)
	{
		std::ofstream output(m_filename);
		if (!output.is_open())
		{
			SPEC_ERROR("Unable to open {0} to write data.", m_filename);
			return;
		}

		YAML::Emitter yamlStream;
		yamlStream << YAML::BeginMap;
		yamlStream << YAML::Key << "Cuts" << YAML::Value << YAML::BeginSeq;
		for (auto& cut : cutList)
		{
			SerializeCut(yamlStream, cut);
		}
		yamlStream << YAML::EndSeq;
		yamlStream << YAML::Key << "Histograms" << YAML::Value << YAML::BeginSeq;
		for (auto& histo : histoList)
		{
			SerializeHistogram(yamlStream, histo);
		}
		yamlStream << YAML::EndSeq << YAML::EndMap;

		output << yamlStream.c_str();
		SPEC_INFO("Successfully saved data to {0}", m_filename);
		output.close();
	}

	void SpectrumSerializer::DeserializeData()
	{
		SpectrumManager& manager = SpectrumManager::GetInstance();
		manager.RemoveAllSpectra(); //When loading in, we remove all extant data, to avoid any potential collisions.

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(m_filename);
		}
		catch (YAML::ParserException& execp)
		{
			SPEC_ERROR("Unable to open {0} to read data.", m_filename);
			return;
		}

		auto cuts = data["Cuts"];
		if (cuts)
		{
			CutArgs tempArgs;
			for (auto& cut : cuts)
			{
				tempArgs.name = cut["Cut"].as<std::string>();
				tempArgs.type = ConvertStringToCutType(cut["Type"].as<std::string>());
				tempArgs.x_par = cut["XParameter"].as<std::string>();
				tempArgs.y_par = cut["YParameter"].as<std::string>();
				if (tempArgs.type == CutType::Cut1D)
				{
					manager.AddCut(tempArgs, cut["XMin"].as<double>(), cut["XMax"].as<double>());
				}
				else if (tempArgs.type == CutType::Cut2D)
				{
					manager.AddCut(tempArgs, cut["XPoints"].as<std::vector<double>>(), cut["YPoints"].as<std::vector<double>>());
				}
				else if (tempArgs.type == CutType::CutSummaryAll || tempArgs.type == CutType::CutSummaryAny)
				{
					manager.AddCut(tempArgs, cut["SubHistos"].as<std::vector<std::string>>(), cut["XMin"].as<double>(), cut["XMax"].as<double>());
				}
			}
		}
		auto histos = data["Histograms"];
		if (histos)
		{
			HistogramArgs tempArgs;
			std::vector<std::string> tempSubHistos;
			for (auto& histo : histos)
			{
				tempArgs.name = histo["Histogram"].as<std::string>();
				tempArgs.type = ConvertStringToSpectrumType(histo["Type"].as<std::string>());
				tempArgs.x_par = histo["XParameter"].as<std::string>();
				tempArgs.y_par = histo["YParameter"].as<std::string>();
				tempArgs.min_x = histo["XMin"].as<double>();
				tempArgs.max_x = histo["XMax"].as<double>();
				tempArgs.nbins_x = histo["XBins"].as<int>();
				tempArgs.min_y = histo["YMin"].as<double>();
				tempArgs.max_y = histo["YMax"].as<double>();
				tempArgs.nbins_y = histo["YBins"].as<int>();
				tempArgs.cutsDrawnUpon = histo["CutsDrawn"].as<std::vector<std::string>>();
				tempArgs.cutsAppliedTo = histo["CutsApplied"].as<std::vector<std::string>>();
				if (tempArgs.type == SpectrumType::Summary)
				{
					manager.AddHistogramSummary(tempArgs, histo["SubHistos"].as<std::vector<std::string>>());
				}
				else
				{
					manager.AddHistogram(tempArgs);
				}
			}
		}

		SPEC_INFO("Successfully loaded data from {0}", m_filename);
	}

}