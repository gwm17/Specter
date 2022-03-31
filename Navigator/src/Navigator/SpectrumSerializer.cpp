/*
	SpectrumSerializer.h
	SpectrumSerializer class providing method to write/read spectra (histograms and cuts) to/from a .nav file. These are formated text files.
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

namespace Navigator {

	SpectrumSerializer::SpectrumSerializer(const std::string& filepath) :
		m_filename(filepath)
	{
	}

	SpectrumSerializer::~SpectrumSerializer() {}

	void SpectrumSerializer::SerializeData(const std::vector<HistogramParameters>& histoList, const std::vector<CutParams>& cutList)
	{
		SpectrumManager& manager = SpectrumManager::GetInstance();

		std::ofstream output(m_filename);
		if (!output.is_open())
		{
			NAV_ERROR("Unable to open {0} to write data.", m_filename);
			return;
		}

		output << "begin_cuts" << std::endl;
		for (auto& cut : cutList)
		{
			if (cut.type == CutType::Cut1D)
			{
				std::vector<double> xpoints = manager.GetCutXPoints(cut.name);
				output << "\tbegin_cut1D" << std::endl;
				output << "\t\tname: " << cut.name << std::endl;
				output << "\t\txparam: " << cut.x_par << std::endl;
				output << "\t\tminValue: " << xpoints[0] << std::endl;
				output << "\t\tmaxValue: " << xpoints[1] << std::endl;
				output << "\tend_cut1D" << std::endl;
			}
			else if (cut.type == CutType::Cut2D)
			{
				std::vector<double> xpoints = manager.GetCutXPoints(cut.name);
				std::vector<double> ypoints = manager.GetCutYPoints(cut.name);
				output << "\tbegin_cut2D" << std::endl;
				output << "\t\tname: " << cut.name << std::endl;
				output << "\t\txparam: " << cut.x_par << std::endl;
				output << "\t\typaram: " << cut.y_par << std::endl;
				output << "\t\tbegin_xvalues" << std::endl;
				for (const auto& value : xpoints)
				{
					output << "\t\t\t" << value << std::endl;
				}
				output << "\t\tend_xvalues" << std::endl;
				output << "\t\tbegin_yvalues" << std::endl;
				for (const auto& value : ypoints)
				{
					output << "\t\t\t" << value << std::endl;
				}
				output << "\t\tend_yvalues" << std::endl;
				output << "\tend_cut2D" << std::endl;
			}
		}
		output << "end_cuts" << std::endl;

		output << "begin_histograms" << std::endl;
		for (auto& params : histoList)
		{
			if (params.type == SpectrumType::Histo1D)
			{
				output << "\tbegin_histogram1D" << std::endl;
				output << "\t\tname: " << params.name << std::endl;
				output << "\t\txparam: " << params.x_par << std::endl;
				output << "\t\tNxbins: " << params.nbins_x << std::endl;
				output << "\t\tXMin: " << params.min_x << std::endl;
				output << "\t\tXMax: " << params.max_x << std::endl;
				output << "\t\tbegin_cutsdrawn" << std::endl;
				for (const auto& name : params.cutsDrawnUpon)
				{
					output << "\t\t\t" << name << std::endl;
				}
				output << "\t\tend_cutsdrawn" << std::endl;
				output << "\t\tbegin_cutsapplied" << std::endl;
				for (const auto& name : params.cutsAppliedTo)
				{
					output << "\t\t\t" << name << std::endl;
				}
				output << "\t\tend_cutsapplied" << std::endl;
				output << "\tend_histogram1D" << std::endl;
			}
			else if (params.type == SpectrumType::Histo2D)
			{
				output << "\tbegin_histogram2D" << std::endl;
				output << "\t\tname: " << params.name << std::endl;
				output << "\t\txparam: " << params.x_par << std::endl;
				output << "\t\typaram: " << params.y_par << std::endl;
				output << "\t\tNxbins: " << params.nbins_x << std::endl;
				output << "\t\tXMin: " << params.min_x << std::endl;
				output << "\t\tXMax: " << params.max_x << std::endl;
				output << "\t\tNybins: " << params.nbins_y << std::endl;
				output << "\t\tYMin: " << params.min_y << std::endl;
				output << "\t\tYMax: " << params.max_y << std::endl;
				output << "\t\tbegin_cutsdrawn" << std::endl;
				for (const auto& name : params.cutsDrawnUpon)
				{
					output << "\t\t\t" << name << std::endl;
				}
				output << "\t\tend_cutsdrawn" << std::endl;
				output << "\t\tbegin_cutsapplied" << std::endl;
				for (const auto& name : params.cutsAppliedTo)
				{
					output << "\t\t\t" << name << std::endl;
				}
				output << "\t\tend_cutsapplied" << std::endl;
				output << "\tend_histogram2D" << std::endl;
			}
			else if (params.type == SpectrumType::Summary)
			{
				output << "\tbegin_histogramSummary" << std::endl;
				output << "\t\tname: " << params.name << std::endl;
				output << "\t\tNxbins: " << params.nbins_x << std::endl;
				output << "\t\tXMin: " << params.min_x << std::endl;
				output << "\t\tXMax: " << params.max_x << std::endl;
				output << "\t\tbegin_subhistos" << std::endl;
				std::vector<std::string> subhistos = manager.GetSubHistograms(params.name);
				for (auto& name : subhistos)
				{
					output << "\t\t\t" << name << std::endl;
				}
				output << "\t\tend_subhistos" << std::endl;
				output << "\t\tbegin_cutsdrawn" << std::endl;
				for (const auto& name : params.cutsDrawnUpon)
				{
					output << "\t\t\t" << name << std::endl;
				}
				output << "\t\tend_cutsdrawn" << std::endl;
				output << "\t\tbegin_cutsapplied" << std::endl;
				for (const auto& name : params.cutsAppliedTo)
				{
					output << "\t\t\t" << name << std::endl;
				}
				output << "\t\tend_cutsapplied" << std::endl;
				output << "\tend_histogram1D" << std::endl;
			}
		}
		output << "end_histograms" << std::endl;

		NAV_INFO("Successfully saved data to {0}", m_filename);
		output.close();
	}

	void SpectrumSerializer::DeserializeData()
	{
		SpectrumManager& manager = SpectrumManager::GetInstance();
		manager.RemoveAllSpectra(); //When loading in, we remove all extant data, to avoid any potential collisions.

		std::ifstream input(m_filename);
		if (!input.is_open())
		{
			NAV_ERROR("Unable to open {0} to read data!", m_filename);
			return;
		}

		std::string check;
		double value_doub;
		CutParams cut_data, reset_cut;
		std::vector<double> cut_xdata;
		std::vector<double> cut_ydata;
		std::vector<std::string> subhistos;
		HistogramParameters hist_data, reset_hist;

		while (input >> check)
		{
			if (check == "begin_cuts")
			{
				while (input >> check)
				{
					cut_data = reset_cut;
					cut_xdata.clear();
					cut_ydata.clear();
					if (check == "begin_cut1D")
					{
						cut_data.type = CutType::Cut1D;
						input >> check >> cut_data.name;
						input >> check >> cut_data.x_par;
						input >> check >> value_doub;
						cut_xdata.push_back(value_doub);
						input >> check >> value_doub;
						cut_xdata.push_back(value_doub);
						input >> check;
						manager.AddCut(cut_data, cut_xdata[0], cut_xdata[1]);
					}
					else if (check == "begin_cut2D")
					{
						cut_data.type = CutType::Cut2D;
						input >> check >> cut_data.name;
						input >> check >> cut_data.x_par;
						input >> check >> cut_data.y_par;
						while (input >> check)
						{
							if (check == "begin_xvalues")
								continue;
							else if (check == "end_xvalues")
								break;
							else
								cut_xdata.push_back(std::stod(check));
						}
						while (input >> check)
						{
							if (check == "begin_yvalues")
								continue;
							else if (check == "end_yvalues")
								break;
							else
								cut_ydata.push_back(std::stod(check));
						}
						input >> check;
						manager.AddCut(cut_data, cut_xdata, cut_ydata);
					}
					else if (check == "end_cuts")
						break;
					else
					{
						NAV_ERROR("Deserialization error; unexpected check condition while parsing cut data! Current value: {0}", check);
						input.close();
						return;
					}
				}
			}
			else if (check == "begin_histograms")
			{
				while (input >> check)
				{
					hist_data = reset_hist;
					if (check == "begin_histogram1D")
					{
						hist_data.type = SpectrumType::Histo1D;
						input >> check >> hist_data.name;
						input >> check >> hist_data.x_par;
						input >> check >> hist_data.nbins_x;
						input >> check >> hist_data.min_x;
						input >> check >> hist_data.max_x;
						while (input >> check)
						{
							if (check == "begin_cutsdrawn")
								continue;
							else if (check == "end_cutsdrawn")
								break;
							else
							{
								hist_data.cutsDrawnUpon.push_back(check);
							}
						}
						while (input >> check)
						{
							if (check == "begin_cutsapplied")
								continue;
							else if (check == "end_cutsapplied")
								break;
							else
							{
								hist_data.cutsAppliedTo.push_back(check);
							}
						}
						input >> check;
						manager.AddHistogram(hist_data);
					}
					else if (check == "begin_histogram2D")
					{
						hist_data.type = SpectrumType::Histo2D;
						input >> check >> hist_data.name;
						input >> check >> hist_data.x_par;
						input >> check >> hist_data.y_par;
						input >> check >> hist_data.nbins_x;
						input >> check >> hist_data.min_x;
						input >> check >> hist_data.max_x;
						input >> check >> hist_data.nbins_y;
						input >> check >> hist_data.min_y;
						input >> check >> hist_data.max_y;
						while (input >> check)
						{
							if (check == "begin_cutsdrawn")
								continue;
							else if (check == "end_cutsdrawn")
								break;
							else
							{
								hist_data.cutsDrawnUpon.push_back(check);
							}
						}
						while (input >> check)
						{
							if (check == "begin_cutsapplied")
								continue;
							else if (check == "end_cutsapplied")
								break;
							else
							{
								hist_data.cutsAppliedTo.push_back(check);
							}
						}
						input >> check;
						manager.AddHistogram(hist_data);
					}
					else if (check == "begin_histogramSummary")
					{
						subhistos.clear();
						hist_data.type = SpectrumType::Summary;
						input >> check >> hist_data.name;
						input >> check >> hist_data.nbins_x;
						input >> check >> hist_data.min_x;
						input >> check >> hist_data.max_x;
						while (input >> check)
						{
							if (check == "begin_subhistos")
								continue;
							else if (check == "end_subhistos")
								break;
							else
							{
								subhistos.push_back(check);
							}
						}
						while (input >> check)
						{
							if (check == "begin_cutsdrawn")
								continue;
							else if (check == "end_cutsdrawn")
								break;
							else
							{
								hist_data.cutsDrawnUpon.push_back(check);
							}
						}
						while (input >> check)
						{
							if (check == "begin_cutsapplied")
								continue;
							else if (check == "end_cutsapplied")
								break;
							else
							{
								hist_data.cutsAppliedTo.push_back(check);
							}
						}
						input >> check;
						manager.AddHistogramSummary(hist_data, subhistos);
					}
					else if (check == "end_histograms")
						break;
					else
					{
						NAV_ERROR("Deserialization error; unexpected check condition while parsing histogram data! Current value: {0}", check);
						input.close();
						return;
					}
				}
			}
			else
			{
				NAV_ERROR("Deserialization error; unexpected check condition at top level! Current value: {0}", check);
				input.close();
				return;
			}
		}

		NAV_INFO("Successfully loaded data from {0}", m_filename);
		input.close();
	}

}