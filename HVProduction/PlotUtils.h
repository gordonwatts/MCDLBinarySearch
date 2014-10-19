//
// Plotting utilities
//
#ifndef __PlotUtils__
#define __PlotUtils__

#include <string>
#include <sstream>

class TH1F;

// Convert the given map that is key-value into
// a 1D plot, with text names along the x axis, and
// values along the y.
template<typename K, typename V, typename StrCnv>
TH1F *ConvertToPlot(const map<K, V> &keyvalues, const std::string &name, const std::string &title, StrCnv &keyToString);

template<typename K, typename V>
TH1F *ConvertToPlot(const map<K, V> &keyvalues, const std::string &name, const std::string &title)
{
	return ConvertToPlot(keyvalues, name, title, convertToString<K>);
}

///////////
// Template implemenattions
///////////
// Default conversion to a string
template<typename K>
std::string convertToString(const K &k)
{
	std::ostringstream str;
	str << k;
	return str.str();
}

template<typename K, typename V, typename StrCnv>
TH1F *ConvertToPlot(const map<K, V> &keyvalues, const std::string &name, const std::string &title, StrCnv &keyToString)
{
	// Create the histogram
	auto h = new TH1F(name.c_str(), title.c_str(), keyvalues.size(), 0.0, 1.0);

	// Put in the main values and the axis values.
	for (auto &v : keyvalues) {
		h->Fill(keyToString(v.first).c_str(), (double)v.second);
	}

	return h;
}

#endif