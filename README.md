# tmate plugIN

tmate plugIN is a plugIN to enable tmate web console remotely with allxon DMS. It connects to the allxon DMS Agent running on a device which you want to control remotely.
This project contains source code to build the tmate plugIN executive and script files which are needed when running the plugIN to control tmate application. It includes the following files and folders.

Source Code
- MainScr - main function, websocketclient and states control of websocket connections and Agent / Device plugIN API
- Util - utilities, header files of allxon DMS plugIN SDK library, cJSON library, header files of argon2 library
- Plugins - classes and functions specific related to tmate plugIN.
- websocket/websocketpp - Websocket++, a C++ websocket client/server library
- lib - allxon DMS plugIN SDK library and argon2 library
- Makefile -  used to build, install, uninstall the target, and package it to upload to the allxon DMS server for destributing to managed devices.

Scripts and Configurations
- config - configuration files for setting this plugIN and what this plugIN looks like on the allxon DMS portal.
- scripts - scripts for called when running commands by users remotely via portal and update states for the current status of tmate app. Otherwise, scripts for start and stop the plugIN itself remotely.
- install - scripts for install and uninstall this plugIN when it is destributed from portal.