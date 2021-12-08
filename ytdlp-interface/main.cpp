#include "gui.hpp"


int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
//int main()
{
	using namespace nana;

	nlohmann::json jconf;

	std::wstring modpath(4096, '\0');
	modpath.resize(GetModuleFileNameW(0, &modpath.front(), modpath.size()));
	auto appdir {modpath.substr(0, modpath.rfind('\\'))};

	std::filesystem::path confpath {get_sys_folder(FOLDERID_RoamingAppData) + L"\\ytdlp-interface.json"};
	if(std::filesystem::exists(confpath))
	{
		std::ifstream f {confpath};
		try { f >> jconf; }
		catch(nlohmann::detail::exception e)
		{
			msgbox mbox {"ytdlp-interface"};
			mbox.icon(msgbox::icon_error);
			(mbox << "an exception occured when trying to load the settings file: " << e.what())();
		}
		if(!jconf.empty())
		{
			conf.ytdlp_path = std::string {jconf["ytdlp_path"]};
			conf.outpath = std::string {jconf["outpath"]};
			conf.fmt1 = to_wstring(std::string {jconf["fmt1"]});
			conf.fmt2 = to_wstring(std::string {jconf["fmt2"]});
			conf.ratelim = jconf["ratelim"];
			conf.ratelim_unit = jconf["ratelim_unit"];
		}
	}
	else // make defaults
	{
		if(std::filesystem::exists(appdir + L"\\yt-dlp.exe"))
			conf.ytdlp_path = appdir + L"\\yt-dlp.exe";
		else if(std::filesystem::exists(LR"(C:\Program Files\yt-dlp\yt-dlp.exe)"))
			conf.ytdlp_path = LR"(C:\Program Files\yt-dlp\yt-dlp.exe)";
		conf.outpath = get_sys_folder(FOLDERID_Downloads);
	}

	GUI gui;
	gui.icon(paint::image {modpath});
	gui.show();
	gui.events().unload([&]
	{
		jconf["ytdlp_path"] = conf.ytdlp_path;
		jconf["outpath"] = conf.outpath;
		jconf["fmt1"] = to_utf8(conf.fmt1);
		jconf["fmt2"] = to_utf8(conf.fmt2);
		jconf["ratelim"] = conf.ratelim;
		jconf["ratelim_unit"] = conf.ratelim_unit;
		std::ofstream f {confpath};
		f << std::setw(4) << jconf;
	});
	nana::exec();
}