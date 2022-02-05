#include "SpectrumSerializer.h"

#include <fstream>

namespace Navigator {

	SpectrumSerializer::SpectrumSerializer(const std::string& filepath) :
		m_filename(filepath)
	{
	}

	SpectrumSerializer::~SpectrumSerializer() {}

	void SpectrumSerializer::SerializeData(const std::vector<HistogramParameters>& histoList, const std::vector<CutParams>& cutList)
	{
		//HistogramMap& histMap = HistogramMap::GetInstance();
		CutMap& cutMap = CutMap::GetInstance();

		std::ofstream output(m_filename);
		if (!output.is_open())
		{
			NAV_ERROR("Unable to open {0} to write data.", m_filename);
			return;
		}

		output << "begin_cuts" << std::endl;
		for (auto& cut : cutList)
		{
			if (cut.y_par == "None")
			{
				auto xpoints = cutMap.GetCutXPoints(cut.name);
				output << "\tbegin_cut1D" << std::endl;
				output << "\t\tname: " << cut.name << std::endl;
				output << "\t\txparam: " << cut.x_par << std::endl;
				output << "\t\tminValue: " << xpoints[0] << std::endl;
				output << "\t\tmaxValue: " << xpoints[1] << std::endl;
				output << "\tend_cut1D" << std::endl;
			}
			else
			{
				auto xpoints = cutMap.GetCutXPoints(cut.name);
				auto ypoints = cutMap.GetCutYPoints(cut.name);
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
			if (params.y_par == "None")
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
			else
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
		}
		output << "end_histograms" << std::endl;

		NAV_INFO("Successfully saved data to {0}", m_filename);
		output.close();
	}

	void SpectrumSerializer::DeserializeData()
	{
		HistogramMap& histMap = HistogramMap::GetInstance();
		CutMap& cutMap = CutMap::GetInstance();

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
						input >> check >> cut_data.name;
						input >> check >> cut_data.x_par;
						input >> check >> value_doub;
						cut_xdata.push_back(value_doub);
						input >> check >> value_doub;
						cut_xdata.push_back(value_doub);
						input >> check;
						cutMap.AddCut(cut_data, cut_xdata[0], cut_xdata[1]);
					}
					else if (check == "begin_cut2D")
					{
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
						cutMap.AddCut(cut_data, cut_xdata, cut_ydata);
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
						histMap.AddHistogram(hist_data);
					}
					else if (check == "begin_histogram2D")
					{
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
						histMap.AddHistogram(hist_data);
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