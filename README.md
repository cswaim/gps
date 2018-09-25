TSIP (Trimble Standard Interface Protocol)
===================
Intro
--------
This is a class implementation of the TSIP.

This class will access a Trimple GPS unit, sending and receiving msgs
based on the TSIP protocol.  The class currently supports specialized
calls to get the gps time in utc and to get the lat/long/elev.

Install
---------
To install,
* cd {yourpath}/gps
* mkdir build
* cd build
* cmake ..
* make
* sudo make install

two modules are installed - gps_test & gps_survey


Usage
---------
  * @code
  * 	std::string prot = '/dev/ttyS0'
  * 	tsip::xyz_t xyz;
  *		time_t gps_time;
  * 
  * 	tsip gps(port);
  *   or
  * 	tsip gps;
  * 	gps.set_gps_port(port);
  * 
  * 	gps_time = gps.get_gps_time_utc();
  * 	xyz = gps.get_xyz();
  * @endcode
  
Changes
---------
2012-02-26 - Initial add
2012-04-18 - Modified constructor so the port name can be set after
             instantiation.
