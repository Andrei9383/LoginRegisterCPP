# LoginRegisterCPP
A login and register system using FungalUI, with C++.

### How does it work?
The application connects to a localhosted MySQL database and adds user information (username, password) upon registration. To log in, it checks the database for the received username and password. The database connection is setup with prepared statements to prevent SQL injections. It uses [Project Krypt](https://github.com/Andrei9383/project-krypt) for encrypting the usernames and passwords of users.
