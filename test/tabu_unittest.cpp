#include <limits.h>
#include "gtest/gtest.h"
#include "tabu.hh"

sgp::SGP sgp1(8, 4, 10);

TEST(TabuList, ADD){

	sgp::SGPDotuTabuList tlist(sgp1);
	ASSERT_THROW( tlist.add(-1 , 0, 0), std::domain_error );
	ASSERT_THROW( tlist.add( 10 , 0, 0), std::domain_error );
	ASSERT_NO_THROW (tlist.add(1 , 0, 1));
}

TEST(TabuList, IS_TABU){
		
	sgp::SGPDotuTabuList tlist(sgp1, 4 , 6);
	ASSERT_THROW(tlist.is_tabu(-1, 0 , 0), std::domain_error);
	EXPECT_FALSE(tlist.is_tabu(0 , 1 , 1)); //tlist is empty
	
	ASSERT_NO_THROW(tlist.add(1, 0 ,1));
	EXPECT_FALSE(tlist.is_tabu(0 , 1 , 1)); 
	
	for( int i = 0; i < 4; i++){
		EXPECT_TRUE(tlist.is_tabu(1 , 0 , 1)); 
		EXPECT_TRUE(tlist.is_tabu(1 , 1 , 0));
	   	tlist.incr_iteration();	
	}

	bool found = false;
	int iter = 0;
	for( int i = 0; i < 2; i++){
		if(	!tlist.is_tabu(1 , 0 , 1) &&
			!tlist.is_tabu(1 , 1 , 0))
		{
			if(!found){
				found = true;
				iter = i;
			}
		} 
	   	tlist.incr_iteration();	
	}

	EXPECT_TRUE(found);
	std::cout << " Tabu iter was: " << iter << std::endl;
	

	for( int i = 0; i < 4; i++){
		EXPECT_FALSE(tlist.is_tabu(1 , 0 , 1)); 
		EXPECT_FALSE(tlist.is_tabu(1 , 1 , 0));
	   	tlist.incr_iteration();	
	}
	
	std::cout << "Adding same elem to tabu list" << std::endl;
	ASSERT_NO_THROW(tlist.add(1, 0 ,1));

	for( int i = 0; i < 4; i++){
		EXPECT_TRUE(tlist.is_tabu(1 , 0 , 1)); 
		EXPECT_TRUE(tlist.is_tabu(1 , 1 , 0));
	   	tlist.incr_iteration();	
	}

	found = false;
	iter = 0;
	for( int i = 0; i < 2; i++){
		if(	!tlist.is_tabu(1 , 0 , 1) &&
			!tlist.is_tabu(1 , 1 , 0))
		{
			if(!found){
				found = true;
				iter = i;
			}
		} 
	   	tlist.incr_iteration();	
	}

	EXPECT_TRUE(found);
	std::cout << " Tabu iter was: " << iter << std::endl;
	

	for( int i = 0; i < 4; i++){
		EXPECT_FALSE(tlist.is_tabu(1 , 0 , 1)); 
		EXPECT_FALSE(tlist.is_tabu(1 , 1 , 0));
	   	tlist.incr_iteration();	
	}

}

TEST(TabuList, CLEAR){

	sgp::SGPDotuTabuList tlist(sgp1, 4 , 6);

	ASSERT_NO_THROW(tlist.add(1, 0 ,1));
	ASSERT_NO_THROW(tlist.add(1, 1 ,2));
	ASSERT_NO_THROW(tlist.add(2, 2 ,3));
	ASSERT_NO_THROW(tlist.add(2, 3 ,4));

	ASSERT_THROW(tlist.clear(-1), std::domain_error);
	
	EXPECT_TRUE(tlist.is_tabu(1 , 0 , 1)); 
	EXPECT_TRUE(tlist.is_tabu(1 , 1 , 2)); 
	ASSERT_NO_THROW(tlist.clear(1));
	EXPECT_FALSE(tlist.is_tabu(1 , 0 , 1)); 
	EXPECT_FALSE(tlist.is_tabu(1 , 0 , 1)); 
	
	EXPECT_TRUE(tlist.is_tabu(2 , 2 , 3)); 
	EXPECT_TRUE(tlist.is_tabu(2 , 3 , 4)); 
	ASSERT_NO_THROW(tlist.clear(2));
	EXPECT_FALSE(tlist.is_tabu(2 , 2 , 3)); 
	EXPECT_FALSE(tlist.is_tabu(2 , 3 , 4)); 
	
}

TEST(TabuList, CLEARALL){

	sgp::SGPDotuTabuList tlist(sgp1, 4 , 6);

	ASSERT_NO_THROW(tlist.add(1, 0 ,1));
	ASSERT_NO_THROW(tlist.add(1, 1 ,2));
	ASSERT_NO_THROW(tlist.add(2, 2 ,3));
	ASSERT_NO_THROW(tlist.add(2, 3 ,4));
	
	EXPECT_TRUE(tlist.is_tabu(1 , 0 , 1)); 
	EXPECT_TRUE(tlist.is_tabu(1 , 1 , 2)); 
	EXPECT_TRUE(tlist.is_tabu(2 , 2 , 3)); 
	EXPECT_TRUE(tlist.is_tabu(2 , 3 , 4)); 
	
	ASSERT_NO_THROW(tlist.clearAll());

	EXPECT_FALSE(tlist.is_tabu(1 , 0 , 1)); 
	EXPECT_FALSE(tlist.is_tabu(1 , 1 , 2)); 
	EXPECT_FALSE(tlist.is_tabu(2 , 2 , 3)); 
	EXPECT_FALSE(tlist.is_tabu(2 , 3 , 4)); 
}
