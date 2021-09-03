#undef _HAS_STD_BYTE
#define WIN32_LEAN_AND_MEAN
#define _HAS_STD_BYTE 0
#include "userlogon.h"
#include <iostream>
#include <fstream>
#include "krypt.cpp"
#include <Windows.h>
#include <mysql.h>


bool registered = false;
bool loggedin = false;
bool fileexists = false;
bool shouldcontinue = true;
bool savedname = false;

int qstate;
int qstate2;

int antisql;

#pragma warning(disable : 4996)
void logon::login_system(fungal::control*) {
	fungal::page* succesl = new fungal::page("succesl", "Login Succesful! Redirecting to main menu..."); {
		fungal::ui << succesl;
	}
	fungal::page* loginerror = new fungal::page("errorl", "ERROR: Username or password incorrect. Try again..."); {
		fungal::ui << loginerror;
	}
	fungal::page* connectionerror = new fungal::page("connectionerrorl", "ERROR: Could not establish connection to the main server! Try again..."); {
		fungal::ui << connectionerror;
	}

	auto connect = [succesl, loginerror, connectionerror](fungal::control*) {
		
		string loginusername = fungal::ui.get_value<std::string>("loginusername");
		string loginpassword = fungal::ui.get_value<std::string>("loginpassword");

		string encryptedloginusername = krypt(krypt(krypt(loginusername, false), false), false);
		string encryptedloginpassword = krypt(krypt(krypt(loginpassword, false), false), false);
		
		unsigned long usernamelegnth = encryptedloginusername.length();
		unsigned long passwordlength = encryptedloginpassword.length();

		//initialize connection
		MYSQL* conn;
		MYSQL_ROW row;
		MYSQL_RES* res;
		conn = mysql_init(0);

		conn = mysql_real_connect(conn, "localhost", "root", "", "login", 3306, NULL, 0);

		if (conn) {
			
			//anti sql-injection
			auto* stmt = mysql_stmt_init(conn);
			string q = "select * from users where username = ? and password = ?";
			mysql_stmt_prepare(stmt, q.c_str(), q.length());

			MYSQL_BIND bind[2];
			
			memset(bind, 0, sizeof(bind));

			bind[0].buffer_type = MYSQL_TYPE_STRING;
			bind[0].buffer = const_cast<char*>(encryptedloginusername.c_str());
			bind[0].is_null = 0;
			bind[0].length = &usernamelegnth;

			bind[1].buffer_type = MYSQL_TYPE_STRING;
			bind[1].buffer = const_cast<char*>(encryptedloginpassword.c_str());
			bind[1].is_null = 0;
			bind[1].length = &passwordlength;


			mysql_stmt_bind_param(stmt, bind);
			mysql_stmt_execute(stmt);

			mysql_stmt_store_result(stmt);
			auto result = mysql_stmt_num_rows(stmt);

			if (result >= 1) {
				loggedin = true;
				fungal::ui.display(succesl);
				Sleep(1000);
				fungal::ui.display("mainmenu");
			}
			else {
				fungal::ui.display(loginerror);
				Sleep(750);
				fungal::ui.display("loginres");
			}

			mysql_stmt_close(stmt);

		}
		else {
			fungal::ui.display(connectionerror);
			Sleep(1000);
			fungal::ui.display("loginres");
		}
	};

	fungal::page* loginpage = new fungal::page("login", "Sign in"); {
		*loginpage << new fungal::textfield("loginusername", "Username", "", false);
		*loginpage << new fungal::textfield("loginpassword", "Password", "", true);
		*loginpage << new fungal::button("loginbtn", "Login", connect);
		fungal::ui << loginpage;
	}
	fungal::ui.display(loginpage);
}

void logon::register_system(fungal::control*) {
	fungal::page* succesr = new fungal::page("succesr", "Registration Succesful! Redirecting to main menu..."); {
		fungal::ui << succesr;
	}
	fungal::page* useralreadyexists = new fungal::page("useralreadyexists", "ERROR: User already exists! Try again..."); {
		fungal::ui << useralreadyexists;
	}
	fungal::page* connectionerror = new fungal::page("connectionerror", "ERROR: Could not establish connetion to the main server! Try again..."); {
		fungal::ui << connectionerror;
	}

	auto confirm_registration = [succesr, useralreadyexists, connectionerror](fungal::control*) {
		
		string username = fungal::ui.get_value<std::string>("username");
		string password = fungal::ui.get_value<std::string>("password");

		string encryptedregisterusername = krypt(krypt(krypt(username, false), false), false);
		string encryptedregisterpassword = krypt(krypt(krypt(password, false), false), false);
		
		unsigned long registerusernamelength = encryptedregisterusername.length();
		unsigned long registerpasswordlength = encryptedregisterpassword.length();

		//initialize connection
		MYSQL* conn;
		MYSQL_ROW row;
		MYSQL_RES* res;
		conn = mysql_init(0);

		conn = mysql_real_connect(conn, "localhost", "root", "", "login", 3306, NULL, 0);

		if (conn) {
			//check if the user already exists

				//anti sql-injection
				auto* stmt = mysql_stmt_init(conn);
				string q = "select * from users where username = ?";
				mysql_stmt_prepare(stmt, q.c_str(), q.length());

				MYSQL_BIND bind[2];

				memset(bind, 0, sizeof(bind));

				bind[0].buffer_type = MYSQL_TYPE_STRING;
				bind[0].buffer = const_cast<char*>(encryptedregisterusername.c_str());
				bind[0].is_null = 0;
				bind[0].length = &registerusernamelength;

				bind[1].buffer_type = MYSQL_TYPE_STRING;
				bind[1].buffer = const_cast<char*>(encryptedregisterpassword.c_str());
				bind[1].is_null = 0;
				bind[1].length = &registerpasswordlength;

				mysql_stmt_bind_param(stmt, bind);
				mysql_stmt_execute(stmt);

				mysql_stmt_store_result(stmt);
				auto result = mysql_stmt_num_rows(stmt);

				if (result >= 1) {
					fungal::ui.display(useralreadyexists);
					Sleep(750);
					fungal::ui.display("loginres");
				}
				else {

					//anti sql-injection
					auto* stmt2 = mysql_stmt_init(conn);
					string q2 = "insert into users (username, password) values (?, ?)";
					mysql_stmt_prepare(stmt2, q2.c_str(), q2.length());

					MYSQL_BIND bind2[2];

					memset(bind2, 0, sizeof(bind2));

					bind2[0].buffer_type = MYSQL_TYPE_STRING;
					bind2[0].buffer = const_cast<char*>(encryptedregisterusername.c_str());
					bind2[0].is_null = 0;
					bind2[0].length = &registerusernamelength;

					bind2[1].buffer_type = MYSQL_TYPE_STRING;
					bind2[1].buffer = const_cast<char*>(encryptedregisterpassword.c_str());
					bind2[1].is_null = 0;
					bind2[1].length = &registerpasswordlength;

					mysql_stmt_bind_param(stmt2, bind2);
					mysql_stmt_execute(stmt2);
					mysql_stmt_close(stmt2);

					registered = true;
					fungal::ui.display(succesr);
					Sleep(1000);
					fungal::ui.display("mainmenu");
				}
			mysql_stmt_close(stmt);
		}
		else {
			fungal::ui.display(connectionerror);
			Sleep(1000);
			fungal::ui.display("loginres");
		}
	};

	fungal::page* registerpage = new fungal::page("register", "Register"); {
		*registerpage << new fungal::textfield("username", "Username", "", false);
		*registerpage << new fungal::textfield("password", "Password", "", true);
		*registerpage << new fungal::button("confirm_register", "Confirm registration", confirm_registration);
		fungal::ui << registerpage;
	}
	fungal::ui.display(registerpage);
}