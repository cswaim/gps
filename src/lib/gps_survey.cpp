/*
 * gps_survey.cpp
 *
 * Access the gps unit and force a self-survey to acquire the new
 * position of the station.
 *
 *  Created on: Oct 24, 2016
 *      Author: cswaim
 *
 * Copyright 2016 Vandevender Enterprises.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#include <gps_survey.h>

namespace {
  const size_t ERROR_IN_COMMAND_LINE = 3;
  const size_t SUCCESS = 0;
  const size_t FAILURE = 1;
  const size_t ERROR_UNHANDLED_EXCEPTION = 4;
  const size_t EXIT_HELP = 2;
}
using namespace std;

int proc_args(int argc,char**  argv) {
	// Declare the supported options.
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "display help text")
		("wait-sec,w", po::value<int>(), "sleep delay seconds, default is 100")
		("survey-cnt,s", po::value<int>(), "survey sample cnt to fix position, default is 100")
		("gps-port,g", po::value<string>(), "gps port, default is /dev/ttyUSB0")
	;

	//po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);

		if (vm.count("help")) {
			cout << "gps_survey will reset the gps location" << endl << endl;
			cout << desc << "\n";
			return EXIT_HELP;
		}

		po::notify(vm);  //throws on error, do after help

	} catch(po::error& e) {
		cerr << "ERROR: " << e.what() << std::endl << endl;
		cerr << desc << endl;
		return ERROR_IN_COMMAND_LINE;
	}

	cout << endl;
	//set run parms
	if (vm.count("wait-sec")) {
		wait_sec = vm["wait-sec"].as<int>();
	} else {
		wait_sec = 100;
	}
	cout << "wait_sec set: " << wait_sec << endl;

	if (vm.count("survey-cnt")) {
		survey_cnt = vm["survey-cnt"].as<int>();
	} else {
		survey_cnt = 100;
	}
	cout << "survey_cnt set: " << survey_cnt << endl;


	if (vm.count("gps-port")) {
		gps_port = vm["gps-port"].as<string>();
	} else {
		gps_port = "/dev/ttyUSB0";
	}
	cout << "gps_port set: " << gps_port << endl;


	return 0;
}

void test_prt(int argc,char **argv) {
	cout << endl << "----from test_prt--------" << endl;
	cout << "number parms: " << argc << " -- parms: " << argv << endl;
		for (int i =0; i < argc; i++) {
			cout << argv[i] << endl;
		}
	if (vm.count("wait-sec")) {
		cout << "wait-sec: " << vm["wait-sec"].as<int>() << endl;
	} else {
		cout << "variable wait-sec not found" << endl;
		//cout << vm;
	}
	cout << endl;
	cout << boost::format("  gps-port: %s") % gps_port << endl;
	cout << boost::format("survey-cnt: %i") % survey_cnt << endl;
	cout << boost::format("  wait-sec: %i") % wait_sec << endl;
	
	cout << endl << "-------------------------" << endl << endl;
}

int main(int argc,char **argv) {
	rc = 0;
	
	try {
		int rtn = proc_args(argc,argv);
		if (rtn > EXIT_SUCCESS) {
			if (rtn != EXIT_HELP) {
				cout << " ***(" << rtn << ") error encountered in parms***" << endl << endl;
			}
			throw ERROR_IN_COMMAND_LINE;
		}
		
		//test_prt(argc,argv);

		//instantiate gps class
		cout << boost::format("gps is on port: %s") % gps_port<< endl;
		tsip gps(gps_port);
		
		if (!gps.port_status) {
			cout << "Unable to open port - terminating run" << endl;
			throw 99;
		}
		
		bool rc;
		//set the survey count
		cout << "setting survey_count to " << survey_cnt << endl;
		rc = gps.set_survey_params(survey_cnt);
		
		//set auto save feature
		cout << "setting autosave" << endl;
		rc = gps.set_auto_save();
		
		//start self survey
		cout << "starting self survey for " << survey_cnt << " position readings" << endl;
		rc = gps.start_self_survey();
		
		//wait for survey to finish
		if (wait_sec > 0) {
			cout << "waiting for " << wait_sec << " seconds." << endl;
			sleep(wait_sec);
			cout << "OK...done!" << endl;
		}
		
		rc = 0;
	} 
	catch(exception& e) {
		cerr << "Unhandled Exception reached the top of main: "
		     << e.what() << ", application will now exit" << endl;
		rc = ERROR_UNHANDLED_EXCEPTION;
		return ERROR_UNHANDLED_EXCEPTION;
	} 
	catch (int n) {
		rc = n;
	}
	catch(...) {
	}
	
	return rc;
}


