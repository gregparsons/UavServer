// ********************************************************************************
//
//  GpDatabase.h
//  UavServer
//
//	Server user/asset database.
//
// ********************************************************************************



#ifndef __UavServer__GpDatabase__
#define __UavServer__GpDatabase__

/*
#import "mysql_connection.h"
#import "mysql_driver.h"
#import "mysql_error.h"


#import <cppconn/connection.h>
#import <cppconn/driver.h>
#import <cppconn/prepared_statement.h>
#import <cppconn/resultset.h>
#import <cppconn/resultset_metadata.h>
#import <cppconn/statement.h>
*/

class GpDatabase {
	
	
public:
	bool authenticateUser(int id);
	bool authenticateAsset(int id);
	

	
	
private:
	
	// singleton connection
	
};







#endif /* defined(__UavServer__GpDatabase__) */
