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
  const size_t ERROR_IN_COMMAND_LINE = 1;
  const size_t SUCCESS = 0;
  const size_t ERROR_UNHANDLED_EXCEPTION = 2;
}
using namespace std;

int proc_args(int argc,char**  argv) {
	// Declare the supported options.
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "display help text")
		("sleep,s", po::value<int>(), "sleep delay seconds, default is 100")
		("position,p", po::value<int>(), "position samples, default is 100")
		("gps-port", po::value<string>(), "gps port, default is /dev/ttyUSB0")
	;

	//po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);

		if (vm.count("help")) {
			cout << "gps_survey will reset the gps location" << endl << endl;
			cout << desc << "\n";
			return 1;
		}

		po::notify(vm);  //throws on error, do after help

	} catch(po::error& e) {
		cerr << "ERROR: " << e.what() << std::endl << endl;
		cerr << desc << endl;
		return ERROR_IN_COMMAND_LINE;
	}

	//set run parms
	if (vm.count("sleep")) {
		sleep_sec = vm["sleep"].as<int>();
	} else {
		sleep_sec = 100;
	}

	if (vm.count("survey")) {
		survey_cnt = vm["survey"].as<int>();
	} else {
		survey_cnt = 200;
	}

	if (vm.count("gps_port")) {
		gps_port = vm["gps_port"].as<string>();
	} else {
		gps_port = "/dev/ttyUSB0";
	}

	return 0;
}

void test_prt(int argc,char **argv) {
	cout << "----from test_prt--------" << endl;
	cout << "number parms: " << argc << " -- parms: " << argv << endl;
		for (int i =0; i < argc; i++) {
			cout << argv[i] << endl;
		}
	if (vm.count("sleep")) {
		cout << "sleep: " << vm["sleep"].as<int>() << endl;
	} else {
		cout << "variable sleep not found" << endl;
		//cout << vm;
	}
	cout << endl;
	cout << boost::format("  gps_port: %s") % gps_port << endl;
	cout << boost::format("survey_cnt: %i") % survey_cnt << endl;
	cout << boost::format(" sleep_cnt: %i") % sleep_sec << endl;
}

int main(int argc,char **argv) {

	try {
		int rtn = proc_args(argc,argv);
		if (rtn == 1) {
			cout << "error encountered in parms" << endl;
		}

		test_prt(argc,argv);

		//instantiate gps class
		cout << boost::format("gps is on port: %s") % gps_port<< endl;
		tsip gps;
		gps.set_gps_port(gps_port);             
		//gps_time = gps.get_gps_time_utc();
		//struct tm *gps_tminfo = gmtime(&gps_time);
		
		bool rc;
		//set the survey count
		rc = gps.set_survey_params(survey_cnt);
		//set auto save feature
		rc = gps.set_auto_save();
		//start self survey
		rc = gps.start_self_survey();


	} catch(exception& e) {
		cerr << "Unhandled Exception reached the top of main: "
		     << e.what() << ", application will now exit" << endl;
		    return ERROR_UNHANDLED_EXCEPTION;
	}
}


