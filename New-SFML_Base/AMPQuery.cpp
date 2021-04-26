#include "AMPQuery.h"

// Using 'iostream' functionality.
using std::cout;
using std::endl;


void AMPQuery::ReportAccelerator(const accelerator a)
{
	const std::wstring bs[2] = { L"false", L"true" };
	std::wcout << ": " << a.description << " "
		<< endl << "       device_path                       = " << a.device_path
		<< endl << "       dedicated_memory                  = " << std::setprecision(4) << float(a.dedicated_memory) / (1024.0f * 1024.0f) << " Mb"
		<< endl << "       has_display                       = " << bs[a.has_display]
		<< endl << "       is_debug                          = " << bs[a.is_debug]
		<< endl << "       is_emulated                       = " << bs[a.is_emulated]
		<< endl << "       supports_double_precision         = " << bs[a.supports_double_precision]
		<< endl << "       supports_limited_double_precision = " << bs[a.supports_limited_double_precision]
		<< endl;
} // report_accelerator

void AMPQuery::ListAccelerators()
{
	// Get all available accelerators and store in a vector for access to details.
	std::vector<accelerator> accls = accelerator::get_all();

	// Iterates over all accelerators and prints characteristics.
	for (unsigned i = 0; i < accls.size(); i++)
	{
		accelerator a = accls[i];
		ReportAccelerator(a);
		//if ((a.dedicated_memory > 0) & (a.dedicated_memory < 0.5*(1024.0f * 1024.0f)))
		//accelerator::set_default(a.device_path);
	}

	//accelerator::set_default(accls[2].device_path);
	accelerator acc = accelerator(accelerator::default_accelerator);
	std::wcout << " default acc = " << acc.description << endl;

} // list_accelerators


void AMPQuery::QueryAMPSupport()
{
	std::vector<accelerator> accls = accelerator::get_all();
	if (accls.empty())
	{
		cout << "No accelerators found that are compatible with C++ AMP" << std::endl;
	}
	else
	{
		cout << "Accelerators found that are compatible with C++ AMP" << std::endl;
		ListAccelerators();
	}
} // query_AMP_support