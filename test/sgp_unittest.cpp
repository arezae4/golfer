#include <limits.h>
#include "gtest/gtest.h"
#include "sgp.hh"
#include "tabu.hh"

extern sgp::SGP sgp1;

namespace sgp{
class SGPTest : public ::testing::Test{

	protected:
		SGP sgp;
		SGPTest() : sgp(8 , 4, 9) {}

		void inline clean_conf(){
			for(auto 	iter = sgp.conflict_set.begin();
						iter != sgp.conflict_set.end() ; iter++ ){
			while(iter->erased){
				iter = sgp.conflict_set.erase(iter);
				if(iter == sgp.conflict_set.end())
					return;
			}
			}
		}

};

TEST_F(SGPTest, INIT_TEST)
{
	sgp.init_solution();
	EXPECT_GT(sgp.get_eval(), 0);

	SGP sgp2(sgp);
	EXPECT_EQ(sgp.get_eval(), sgp2.get_eval());

	sgp::SGPDotuTabuList tlist(sgp, 2, 6);
	
	for(int i = 0 ; i < 100 ; i++){
		int before = sgp.get_eval();
		ASSERT_NO_THROW( sgp.local_search(tlist, sgp.get_eval()));
		std::cout << "before :" << before << " after: " << 
			sgp.get_eval() << std::endl;

		//EXPECT_LE(sgp.get_eval(), sgp2.get_eval());
		sgp2 = sgp;
		EXPECT_EQ(sgp.get_eval(), sgp2.get_eval());
		tlist.incr_iteration();
	}
}

TEST_F(SGPTest, EVAL){

	EXPECT_EQ( sgp.get_eval(), 0);
}

/*TEST_F(SGPTest, LOCAL_SEARCH)
{
	sgp::SGPDotuTabuList tlist(sgp, 2, 6);
	for(int i = 0; i < 1000 ; i++){
		int before = sgp.get_eval();
		ASSERT_NO_THROW( sgp.local_search(tlist, sgp.get_eval()));
		std::cout << "before :" << before << " after: " << 
			sgp.get_eval() << std::endl;
		tlist.incr_iteration();
	}
}*/		

TEST_F(SGPTest, SET_FIELD)
{
	EXPECT_EQ(sgp.best_eval , 0);	
	EXPECT_EQ(sgp.conflict_set.size() , 0);	
	
	//set group 0 in week 0 [0, 1, 2, 3]
	
	sgp.set_field(0,0,UNSET,0);
	EXPECT_EQ(sgp.best_eval , 0);	
	EXPECT_EQ(sgp.conflict_set.size() , 0);	
	
	sgp.set_field(0,0,UNSET,1);
	EXPECT_EQ(sgp.best_eval , 0);	
	EXPECT_EQ(sgp.conflict_set.size() , 0);	
	
	sgp.set_field(0,0,UNSET,2);
	EXPECT_EQ(sgp.best_eval , 0);	
	EXPECT_EQ(sgp.conflict_set.size() , 0);	

	sgp.set_field(0,0,UNSET,3);
	EXPECT_EQ(sgp.best_eval , 0);	
	EXPECT_EQ(sgp.conflict_set.size() , 0);	
	
	// set group 0 in week 1 [0 , 1, 2, 3]

	sgp.set_field(1,0,UNSET,0);
	EXPECT_EQ(sgp.best_eval , 0);	
	EXPECT_EQ(sgp.conflict_set.size() , 0);	
	
	sgp.set_field(1,0,UNSET,1);
	EXPECT_EQ(sgp.best_eval , 1);	
	EXPECT_EQ(sgp.conflict_set.size() , 4);	
	
	sgp.set_field(1,0,UNSET,2);
	EXPECT_EQ(sgp.best_eval , 3);	
	EXPECT_EQ(sgp.conflict_set.size() , 6);	

	sgp.set_field(1,0,UNSET,3);
	EXPECT_EQ(sgp.best_eval , 6);	
	EXPECT_EQ(sgp.conflict_set.size() , 8);	
	
	// set group 1 in week 1 [4, 5, 6, 7]

	sgp.set_field(1,1,UNSET,4);
	EXPECT_EQ(sgp.best_eval , 6);	
	EXPECT_EQ(sgp.conflict_set.size() , 8);	
	
	sgp.set_field(1,1,UNSET,5);
	EXPECT_EQ(sgp.best_eval , 6);	
	EXPECT_EQ(sgp.conflict_set.size() , 8);	
	
	sgp.set_field(1,1,UNSET,6);
	EXPECT_EQ(sgp.best_eval , 6);	
	EXPECT_EQ(sgp.conflict_set.size() , 8);	

	sgp.set_field(1,1,UNSET,7);
	EXPECT_EQ(sgp.best_eval , 6);	
	EXPECT_EQ(sgp.conflict_set.size() , 8);	
	
	// set group 0 in week 1 [8, 9, 10, 11]

	sgp.set_field(1,0,0,8);
	EXPECT_EQ(sgp.best_eval , 3);	
	EXPECT_EQ(sgp.conflict_set.size() , 8);	
	decision elem(1, 0, 0);
	auto iter = sgp.conflict_set.find(elem);	
	EXPECT_TRUE(iter->erased );	
	clean_conf();
	EXPECT_EQ(sgp.conflict_set.size() , 6);	


	sgp.set_field(1,0,1,9);
	EXPECT_EQ(sgp.best_eval , 1);	
	EXPECT_EQ(sgp.conflict_set.size() , 6);	
	elem.val = 1;
	iter = sgp.conflict_set.find(elem);	
	EXPECT_TRUE(iter->erased);	
	clean_conf();
	EXPECT_EQ(sgp.conflict_set.size() , 4);	


	sgp.set_field(1,0,2,10);
	EXPECT_EQ(sgp.best_eval , 0);	
	EXPECT_EQ(sgp.conflict_set.size() , 4);	
	elem.val = 2;
	iter = sgp.conflict_set.find(elem);	
	EXPECT_TRUE(iter->erased);	
	clean_conf();
	EXPECT_EQ(sgp.conflict_set.size() , 0);	


	sgp.set_field(1,0,3,11);
	EXPECT_EQ(sgp.best_eval , 0);	
	EXPECT_EQ(sgp.conflict_set.size() , 0);	
	elem.val = 3;
	iter = sgp.conflict_set.find(elem);	
	EXPECT_TRUE(iter->erased);	
	clean_conf();
	EXPECT_EQ(sgp.conflict_set.size() , 0);	


	EXPECT_TRUE(true);	
}

TEST_F(SGPTest, CALC_CONFLICTS)
{
EXPECT_EQ(sgp.best_eval , 0);	
	EXPECT_EQ(sgp.conflict_set.size() , 0);	
	
	//set group 0 in week 0 [0, 1, 2, 3]
	
	sgp.set_field(0,0,UNSET,0);
	EXPECT_EQ(sgp.best_eval , 0);	
	EXPECT_EQ(sgp.conflict_set.size() , 0);	
	
	sgp.set_field(0,0,UNSET,1);
	EXPECT_EQ(sgp.best_eval , 0);	
	EXPECT_EQ(sgp.conflict_set.size() , 0);	
	
	sgp.set_field(0,0,UNSET,2);
	EXPECT_EQ(sgp.best_eval , 0);	
	EXPECT_EQ(sgp.conflict_set.size() , 0);	

	sgp.set_field(0,0,UNSET,3);
	EXPECT_EQ(sgp.best_eval , 0);	
	EXPECT_EQ(sgp.conflict_set.size() , 0);	
	
	// set group 0 in week 1 [0 , 1, 2, 3]

	sgp.set_field(1,0,UNSET,0);
	EXPECT_EQ(sgp.best_eval , 0);	
	EXPECT_EQ(sgp.conflict_set.size() , 0);	
	
	sgp.set_field(1,0,UNSET,1);
	EXPECT_EQ(sgp.best_eval , 1);	
	EXPECT_EQ(sgp.conflict_set.size() , 4);	
	
	sgp.set_field(1,0,UNSET,2);
	EXPECT_EQ(sgp.best_eval , 3);	
	EXPECT_EQ(sgp.conflict_set.size() , 6);	

	sgp.set_field(1,0,UNSET,3);
	EXPECT_EQ(sgp.best_eval , 6);	
	EXPECT_EQ(sgp.conflict_set.size() , 8);	
	
	// set group 1 in week 1 [4, 5, 6, 7]

	sgp.set_field(1,1,UNSET,4);
	EXPECT_EQ(sgp.best_eval , 6);	
	EXPECT_EQ(sgp.conflict_set.size() , 8);	
	
	sgp.set_field(1,1,UNSET,5);
	EXPECT_EQ(sgp.best_eval , 6);	
	EXPECT_EQ(sgp.conflict_set.size() , 8);	
	
	sgp.set_field(1,1,UNSET,6);
	EXPECT_EQ(sgp.best_eval , 6);	
	EXPECT_EQ(sgp.conflict_set.size() , 8);	

	sgp.set_field(1,1,UNSET,7);
	EXPECT_EQ(sgp.best_eval , 6);	
	EXPECT_EQ(sgp.conflict_set.size() , 8);	
	
	// set group 0 in week 1 [8, 9, 10, 11]

	sgp.set_field(1,0,0,8);
	EXPECT_EQ(sgp.best_eval , 3);	
	EXPECT_EQ(sgp.conflict_set.size() , 8);	
	decision elem(1, 0, 0);
	auto iter = sgp.conflict_set.find(elem);	
	EXPECT_TRUE(iter->erased );	
	clean_conf();
	EXPECT_EQ(sgp.conflict_set.size() , 6);	


	sgp.set_field(1,0,1,9);
	EXPECT_EQ(sgp.best_eval , 1);	
	EXPECT_EQ(sgp.conflict_set.size() , 6);	
	elem.val = 1;
	iter = sgp.conflict_set.find(elem);	
	EXPECT_TRUE(iter->erased);	
	clean_conf();
	EXPECT_EQ(sgp.conflict_set.size() , 4);	


	sgp.set_field(1,0,2,10);
	EXPECT_EQ(sgp.best_eval , 0);	
	EXPECT_EQ(sgp.conflict_set.size() , 4);	
	elem.val = 2;
	iter = sgp.conflict_set.find(elem);	
	EXPECT_TRUE(iter->erased);	
	clean_conf();
	EXPECT_EQ(sgp.conflict_set.size() , 0);	


	sgp.set_field(1,0,3,11);
	EXPECT_EQ(sgp.best_eval , 0);	
	EXPECT_EQ(sgp.conflict_set.size() , 0);	
	elem.val = 3;
	iter = sgp.conflict_set.find(elem);	
	EXPECT_TRUE(iter->erased);	
	clean_conf();
	EXPECT_EQ(sgp.conflict_set.size() , 0);	


	//calc swap of 0<->8
		
	EXPECT_EQ(sgp.calc_conflicts_in_group(1, 0 , 8, 0) , 0);
	
	sgp.set_field(1,0,8,0);
	EXPECT_EQ(sgp.best_eval , 0);	
	EXPECT_EQ(sgp.conflict_set.size() , 0);	
	
	EXPECT_EQ(sgp.calc_conflicts_in_group(1, 0 , 9, 1) , 1);
	
	sgp.set_field(1,0,9,1);
	EXPECT_EQ(sgp.best_eval , 1);	
	EXPECT_EQ(sgp.conflict_set.size() , 4);	
	
	EXPECT_EQ(sgp.calc_conflicts_in_group(1, 0 , 10, 2) , 2);
	
	sgp.set_field(1,0,10,2);
	EXPECT_EQ(sgp.best_eval , 3);	
	EXPECT_EQ(sgp.conflict_set.size() , 6);	
	
	EXPECT_EQ(sgp.calc_conflicts_in_group(1, 0 , 2, 10) , -2);

	EXPECT_EQ(sgp.calc_conflicts_player(0), 2);
	EXPECT_EQ(sgp.calc_conflicts_player(1), 2);
	EXPECT_EQ(sgp.calc_conflicts_player(2), 2);
	EXPECT_EQ(sgp.calc_conflicts_player(10), 0);
	EXPECT_EQ(sgp.calc_conflicts_player(11), 0);
	EXPECT_EQ(sgp.calc_conflicts_player(4), 0);
	EXPECT_EQ(sgp.calc_conflicts_player(5), 0);
}

}
