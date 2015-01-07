#include "log4cpp/Category.hh"
#include "log4cpp/Appender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/Priority.hh"
#include "log4cpp/Ostreamappender.hh"
#include "log4cpp/FileAppender.hh"
#include <boost/program_options.hpp>
#include "sgp.hh"
#include "tabu.hh"

namespace po = boost::program_options;

int main(int argc , char **argv){
	
	log4cpp::Appender *console_appender = new log4cpp::OstreamAppender("console", &std::cout);
	console_appender->setLayout(new log4cpp::BasicLayout);

	log4cpp::Appender *file_appender = new log4cpp::FileAppender("default", "program.log");
	file_appender->setLayout(new log4cpp::BasicLayout);

	log4cpp::Category& root = log4cpp::Category::getRoot();
	root.addAppender(console_appender);
	//root.addAppender(file_appender);

	root.warn("testing error %d" , 1);

    po::options_description desc("Allowed options");
	po::positional_options_description pos;
	desc.add_options()
        ("help,h", "produce help message")
		("group,g", po::value<unsigned int>()->default_value(8), 
		 		"SGP Group value")
		("player,p", po::value<unsigned int>()->default_value(4), 
		 		"SGP Player value")
		("week,w", po::value<unsigned int>()->default_value(9), 
		 		"SGP Week value")
		("max-tries,i", 	po::value<unsigned int>()->default_value(10000), 
		 				"Maximal tries for local search")
		("max-stable-tries,s", po::value<unsigned int>()->default_value(300), 
		 				"Maximal stable tries for local search")
		("min-tabu", 	po::value<unsigned int>()->default_value(4), 
		 				"Minimum tabu iteration for elements")
		("max-tabu", 	po::value<unsigned int>()->default_value(100), 
		 				"Maximum tabu iteration for elements")
		("logger,l", 	po::value<unsigned int>()->default_value(2), 
		 				"Log level")
		("tabu",		"Try tabu search")
        ;
   
	pos.add("group", 1);
	pos.add("player", 1);
	pos.add("week", 1);

    po::variables_map vm;
	try {
        po::store(po::command_line_parser(argc, argv).options(desc)
													.positional(pos)
													.run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            // Print help message.
            std::cout << desc << std::endl;
            return EXIT_SUCCESS;
        }

        
    } catch(const po::error &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

	//Print info!
	std::cout << "Solving SGP( :\t" << vm["group"].as<unsigned int>() << " , "
									<< vm["player"].as<unsigned int>() << " , "
									<< vm["week"].as<unsigned int>() << " , "
									<< std::endl; 
	std::cout 	<< "Maximum tabu length set to:\t" 
				<< vm["max-tabu"].as<unsigned int>() << std::endl 
				<< ", Miniumu tabu length set to:\t"
				<< vm["min-tabu"].as<unsigned int>() << std::endl
				<< ", Maximum Tries:\t"
				<< vm["max-tries"].as<unsigned int>() << std::endl
				<< ", Maximum Stable Tries:\t"
				<< vm["max-stable-tries"].as<unsigned int>() << std::endl
				<< std::endl;
				


	switch(vm["logger"].as<unsigned int>()){
	
		case 1:
			root.setPriority(log4cpp::Priority::DEBUG);
			break;
		case 2:
			root.setPriority(log4cpp::Priority::INFO);
			break;
		case 3:
			root.setPriority(log4cpp::Priority::WARN);
			break;
		case 4:
			root.setPriority(log4cpp::Priority::ERROR);
			break;
		case 5:
			root.setPriority(log4cpp::Priority::FATAL);
			break;
		case 6:
			root.setPriority(log4cpp::Priority::NOTSET);
			break;

		default:
			assert(false);

	}
	
	/* Initializing the problem */
	sgp::SGP sgp1( 	vm["group"].as<unsigned int>(),
					vm["player"].as<unsigned int>(),
					vm["week"].as<unsigned int>());

	std::clock_t start; 
	double total = 0;
	
	
	if(vm.count("tabu")){
		root.debug("1");
		sgp::SGPDotuTabuSolver tabuSolver(	sgp1, 
										 vm["max-tries"].as<unsigned int>(),
									vm["max-stable-tries"].as<unsigned int>(),
										 vm["min-tabu"].as<unsigned int>(),
										 vm["max-tabu"].as<unsigned int>());
		root.debug("2");
		start = std::clock();
		tabuSolver.run();
		double duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
		//std::cout << sgp1 << std::endl;
		std::cout << "Tabu search finished, RUNTIME  = " << duration << " sec." << std::endl;
		total += duration;
	}
	std::cout << "---------FINAL BEST SOLUTION-----------" << std::endl << std::endl;
	std::cout << sgp1 << std::endl;
 
	std::cout << "TOTAL RUNTIME: " << total << " sec." << std::endl;

	return EXIT_SUCCESS;
}

