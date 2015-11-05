
#include <fstream>
#include <iomanip>
#include <string>

#include "moab/ProgOptions.hpp"
#include "dag_slicer.hpp"

int main( int argc, char ** argv )
{

  ProgOptions po("Program for slicing a dagmc model.");

  std::string filename;
  int axis = 0;
  double coord = 0.0;
  bool write_to_file = false;
  bool by_group = false;
  
  po.addRequiredArg<std::string>("input_file", "Filename of the .h5m model to slice.", &filename);

  po.addOpt<int>("ax", "Axis along which to slice the model. Default is x (0).", &axis);

  po.addOpt<double>("coord", "Coordinate for the slice. Default is 0.0", &coord);

  po.addOpt<void>("w", "Write slice points to file slicepnts.txt.", &write_to_file);

  po.addOpt<void>("by-group","Write out the slice points by group.",&by_group);

  po.parseCommandLine(argc, argv); 

  Dag_Slicer ds(filename, axis, coord, by_group);
  ds.create_slice(); 
  
  std::ofstream output, coding; 
  output.precision(6);

  output.open("slicepnts.txt");
  coding.open("coding.txt");
  
  //write the slice points to file
  for(unsigned int i = 0; i < ds.slice_x_pnts.size(); i++) {
          if(by_group) {
	output << ds.group_names[i] << std::endl;
	coding << ds.group_names[i] << std::endl;
      }
      for(unsigned int j = 0; j < ds.slice_x_pnts[i].size(); j++) {
	  //write x point
	output << ds.slice_x_pnts[i][j] << std::fixed << " ";
	//write y point
	output << ds.slice_y_pnts[i][j] << std::fixed << " ";
	output << std::endl;
	//write path coding
	coding << ds.path_coding[i][j] << std::endl;
      }
      output << std::endl;
      coding << std::endl;
  }
  
  //close open filestreams
  output.close();
  coding.close();
  
  return 0;
} //end main

