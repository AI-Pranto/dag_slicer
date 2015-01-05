
//c++ includes
#include <assert.h>
#include <iostream> 

//slicer includes
#include "slicer.hpp"

//SIGMA includes
#include "MBCore.hpp"
#include "MBInterface.hpp"

#include "testutils.hpp"

using namespace moab;

//global moab instance
MBInterface *mbi = new MBCore(); 

//test functions
void create_surface_intersections_test();
void get_intersection_test();
void get_sets_by_category_test();
void get_surfaces_test();
void get_all_volumes_test();
void test_point_match();


int main( int /* argc */, char** /* argv */) 
{ 

  MBErrorCode result = mbi->load_mesh("cyl.h5m");
  ERR_CHECK(result);
  
  //get_sets_by_category_test( mbi );
  int failed_tests = 0; 
  failed_tests += RUN_TEST(get_sets_by_category_test);
  failed_tests += RUN_TEST(get_surfaces_test);
  failed_tests += RUN_TEST(get_all_volumes_test);
  failed_tests += RUN_TEST(test_point_match);
  failed_tests += RUN_TEST(create_surface_intersections_test);
  failed_tests += RUN_TEST(get_intersection_test);
}

void create_surface_intersections_test()
{

  MBRange surfs;
  get_surfaces(mbi, surfs);

  std::map<MBEntityHandle, std::vector<Loop> >int_map;
  MBErrorCode result = create_surface_intersections( mbi, surfs, 0, 0, int_map);
  ERR_CHECK(result);

  CHECK( (int)int_map.size() == 3);

}

void get_intersection_test()
{

  MBCartVect point0, point1;
  point0[0] = 1; point0[1] = 1; point0[2] = 0;
  point1[0] = -1; point1[1] = 1; point1[2] = 0;

  Line test_line;

  get_intersection( point0, point1, 0, 0.0, test_line);

  CHECK( test_line.begin[0] == 0.0 );
  CHECK( test_line.begin[1] == 1.0 ); 
  CHECK( test_line.begin[2] == 0.0 );

  point1[0] = 1; point1[1] = -1; point1[2] = 0; 

  get_intersection( point0, point1, 1 , 0.0, test_line ); 
  
  CHECK( test_line.end[0] == 1.0 );
  CHECK( test_line.end[1] == 0.0 );
  CHECK( test_line.end[2] == 0.0 );

  point1[0] = 1; point1[1] = 1; point1[2] = 2; 

  Line test_line2; 

  get_intersection( point0, point1, 2 , 1.0, test_line2 ); 
  
  CHECK( test_line2.begin[0] = 1.0 );
  CHECK( test_line2.begin[1] = 1.0 ); 
  CHECK( test_line2.begin[2] = 1.0 );

  //this function expects an intersection to exist
  //an intentional failure case will not be created
}

void get_sets_by_category_test()
{

  MBRange sets; 
  char category1[CATEGORY_TAG_SIZE] = "Volume";
  MBErrorCode result = get_sets_by_category( mbi, sets, category1); 
  ERR_CHECK(result); 
 
  CHECK_EQUAL( 1 , (int)sets.size() ); 

  sets.clear();
  char category2[CATEGORY_TAG_SIZE] = "Surface";
  result = get_sets_by_category( mbi, sets, category2 );
 
  CHECK_EQUAL(3, (int)sets.size() );

}  
  

void get_surfaces_test()
{

  MBRange surfaces; 
  MBErrorCode result = get_surfaces( mbi, surfaces );
  ERR_CHECK(result);
  //test file is a cylinder and should have 3 surfaces
  CHECK_EQUAL( 3, (int)surfaces.size() );

}

void get_all_volumes_test()
{

  MBRange volumes; 
  MBErrorCode result = get_all_volumes( mbi, volumes );
  ERR_CHECK(result);
  //test file is a cylinder and should have 1 volume
  CHECK_EQUAL( 1, (int)volumes.size() ); 

}

void test_point_match()
{

  //setup two cart vectors that will represent points 
  MBCartVect a,b;
 
  //initialize
  a[0] = 1.0; a[1] = 1.0; a[2] = 1-1e-8;
  
  //make b equal to a
  b=a; 

  //These should be recognized as coincident
  CHECK( point_match(a,b) );

  //Now alter b a little bit
  b[2] = 1.0;

  //These should still be recognized as coincident
  CHECK( point_match(a,b) );

  //Now alter b a lot (relatively)
  b[2] = 2.0;

  //This should not be recognized as coincident
  CHECK( !point_match(a,b) );

}

