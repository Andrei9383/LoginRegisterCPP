#include <fui/fui.h>
#include <ClipboardXX.hpp>
#include "krypt.cpp"
#include <fstream>
#include "userlogon.h"
#include "dekrypt.cpp"

bool showagain = true;
bool shouldwrite = false;
extern bool loggedin;
extern bool registered;
extern bool savedname;

void toclipboard(fungal::control*) {
	std::string s = fungal::ui.get_value<std::string>("eoutput");
	if (s == "") {
		return;
	}
	OpenClipboard(0);
	EmptyClipboard();
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size() + 1);
	if (!hg) {
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg), s.c_str(), s.size() + 1);
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
	GlobalFree(hg);
}

void encrypter(fungal::control*) {

	auto encrypt = [](fungal::control*) {
		fungal::page* output = new fungal::page("output", "Output"); {
			*output << new fungal::textfield("eoutput", "Output", krypt(fungal::ui.get_value<std::string>("einput"), true), false);
			*output << new fungal::button("ecopy", "Copy to clipboard", toclipboard);
			fungal::ui << output;
		}
		fungal::ui.display(output);
	};

	fungal::page* encrypter = new fungal::page("encrypter", "Encryper"); {
		*encrypter << new fungal::textfield("einput", "Input your text here", "", false);
		*encrypter << new fungal::button("econfirm", "Encrypt", encrypt);
		fungal::ui << encrypter;
	}

	fungal::ui.display(encrypter);
}

#pragma warning(disable : 4996)
int main() {
	fungal::ui.init();

	auto write_to_file = [](fungal::control*) {
		ofstream file;
		string path = getenv("appdata");
		path += "\\speedy";
		CreateDirectoryA(path.c_str(), NULL);
		path += "\\appconfig.json";

		string line;
		ifstream check;
		check.open(path.c_str());
		if (check.is_open()) {
			while (getline(check, line)) {
				if (line.find(krypt("showagain", false), 0) != string::npos) {
					if (getline(check, line)) {
						if (line.find(krypt("true", false), 0) != string::npos) {
							showagain = true;
						}
						else if (line.find(krypt("false", false), 0) != string::npos) {
							showagain = false;
						}
						else {
							shouldwrite = true;
						}
					}
					else {
						shouldwrite = true;
					}
				}
				else {
					shouldwrite = true;
				}
			}
			check.close();
		}
		else {
			shouldwrite = true;
		}

		file.open(path.c_str());
		if (file.is_open()) {
			file << krypt("showagain", false) << endl;
			file << krypt(fungal::ui.get_value<bool>("showagain") == true ? "false" : "true", false) << endl;
			file.close();
		}

		if (loggedin || registered) {
			fungal::ui.display("mainmenu");
		}
		else {
			fungal::ui.display("loginres");
		}
	};

	bool shouldshowagain = true;

	string path = getenv("appdata");
	path += "\\speedy";
	CreateDirectoryA(path.c_str(), NULL);
	path += "\\appconfig.json";

	string line;
	ifstream check;
	check.open(path.c_str());
	if (check.is_open()) {
		while (getline(check, line)) {
			if (line.find(krypt("showagain", false), 0) != string::npos) {
				if (getline(check, line)) {
					if (line.find(krypt("true", false), 0) != string::npos) {
						shouldshowagain = true;
					}
					else if (line.find(krypt("false", false), 0) != string::npos) {
						shouldshowagain = false;
					}
				}
			}
		}
		check.close();
	}

	auto logout = [](fungal::control*) {
		loggedin = false;
		registered = false;
		fungal::ui.display("loginres");
	};

	auto bypass_login = [](fungal::control*) {
		loggedin = true;
		fungal::ui.display("mainmenu");
	};

	fungal::page* tutorial = new fungal::page("tutorial", "Introduction"); {
		*tutorial << new fungal::textfield("hello", "", "Hello user! This is an introduction to the controls of the app");
		*tutorial << new fungal::textfield("controls", "", "You can move left, right, up and down with the corresponding arrow keys.");
		*tutorial << new fungal::textfield("buttons", "", "You can use buttons by pressing enter.");
		*tutorial << new fungal::textfield("back", "", "You can always go back to the previous page or exit the app with escape.");
		*tutorial << new fungal::checkbox("showagain", "Do not show again", true);
		*tutorial << new fungal::button("exittutorial", "Continue", write_to_file);
		fungal::ui << tutorial;
	}

	fungal::page* mainmenu = new fungal::page("mainmenu", "Main Menu"); {
		*mainmenu << new fungal::radioselect("tools", "Tools", { "Encryptor", "Decryptor" });
		*mainmenu << new fungal::button("logout", "Log out", logout);
		fungal::ui << mainmenu;
	}

	bool saveduser = false;
	string name;
	ifstream altfile;
	string altline;
	string altpath;
	altpath = getenv("appdata");
	altpath += "\\speedy";
	altpath += "\\savedusers.krypt";

	altfile.open(altpath.c_str());
	if (altfile.is_open()) {
		if (getline(altfile, altline)) {
			name = dekrypt(altline);
			saveduser = true;
		}
	}

	fungal::page* loginalt = new fungal::page("loginalt", "Authentification"); {
		*loginalt << new fungal::button("shouldloginalt", ("Sign in as " + name), bypass_login);
		*loginalt << new fungal::button("shouldregisteralt", "Register", logon::register_system);

		fungal::ui << loginalt;
	}

	fungal::page* loginres = new fungal::page("loginres", "Authentification"); {
		*loginres << new fungal::button("shouldlogin", "Sign in", logon::login_system);
		*loginres << new fungal::button("shouldregister", "Register", logon::register_system);

		fungal::ui << loginres;
	}

	if (shouldshowagain) {
		fungal::ui.display(tutorial);
	}
	else {
		if (saveduser) {
			fungal::ui.display(loginalt);
		}
		else {
			fungal::ui.display(loginres);
		}
	}

	fungal::ui.exec();
}
