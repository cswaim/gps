/*
 * gps_survey.h
 *
 *  Created on: Oct 24, 2016
 *      Author: cswaim
 */

#ifndef GPS_SURVEY_H_
#define GPS_SURVEY_H_

#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <string>
#include <iostream>

#include <tsip.h>


namespace po = boost::program_options;

po::variables_map vm;
int wait_sec;
int survey_cnt;
tsip gps;
std::string gps_port;     // "/dev/ttyUSB0"
int rc;                   // program return code 

int proc_args(int argc,char** argv, po::variables_map &vm);

#endif /* GPS_SURVEY_H_ */
