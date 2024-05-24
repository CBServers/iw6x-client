#pragma once

#include "nt.hpp"
#include <ShlObj.h>
#include <string>

namespace utils::com
{
	bool select_folder(std::string& out_folder, const std::string& title = "Select a Folder", const std::string& selected_folder = {});
	IProgressDialog* create_progress_dialog();
}
