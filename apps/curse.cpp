/*****************************************************************************

    (c) 2013 Hobu, Inc. hobu.inc@gmail.com

    Author: Andrew Bell andrew.bell.ia at gmail.com

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#include <hexer/Processor.hpp>
#include <hexer/OGR.hpp>
#include <hexer/hexer.hpp>
#include "las.hpp"


#include <fstream>
#include <sstream>

#include <boost/algorithm/string/compare.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

using namespace std;


bool readHex(int& x, int& y, void* ctx)
{
    static int pos = 0;

    static int coords[] = {
        0, 0,
        1, 0,
        2, 1,
        3, 1,
        4, 2,
        4, 3,
        4, 4,
        3, 4,
        2, 5,
        1, 5,
        0, 5,
        -1, 4,
        -2, 4,
        -3, 3,
        -3, 2,
        -3, 1,
        -2, 1,
        -1, 0,
        -1, 2,
        0, 2,
        1, 3,
        2, 3
    };

    static int num_coords = sizeof(coords) / (sizeof(coords[0]));
    
    if (pos + 1 < num_coords) {
        x = coords[pos++];
        y = coords[pos++];
        return true;
    }
    return false;
}


std::string indent(int l)
{
    std::string tabs;

    tabs.append(l * 2, ' ');
    return tabs;
}

void dumpPath(hexer::Path *p)
{
    using namespace hexer;

    static int level = 0;
    Orientation o = p->orientation();
    std::string ostring = ((o == CLOCKWISE) ? "CLOCKWISE" : "ANTICLOCKWISE");
    indent(level);
    cerr << indent(level) << "Path length = " << p->pathLength() << "!\n";
    cerr << indent(level) << "Orientation = " << ostring << "!\n";
    vector<Path *> paths = p->subPaths();
    level++;
    for (int pi = 0; pi != paths.size(); ++pi)
    {
        dumpPath(paths[pi]);
    }
    level--;
}

void hextest(std::string filename)
{
    using namespace hexer;

    vector<GridInfo *> infos;
    GridInfo *gi = new GridInfo;

    gi->m_hexsize = 10;
    gi->m_density = 10;
    infos.push_back(gi);

    // LAS l(filename);
    // l.open();
    // process(infos, l.reader);
    
    processHexes(infos, readHex);

    // Dump hexes.
    for (HexIter iter = gi->begin(); iter != gi->end(); ++iter)
    {
        HexInfo hi = *iter;
        cerr << "Density/X/Y = " << hi.m_density << "/" <<
            hi.m_center.m_x << "/" << hi.m_center.m_y << "!\n";
    }

    delete gi;
}

void pathtest(std::string filename)
{
    using namespace hexer;

    vector<GridInfo *> infos;
    GridInfo *gi = new GridInfo;

    infos.push_back(gi);
    LAS l(filename);
    l.open();
    process(infos, l.reader);

    for (std::vector<Path*>::size_type pi = 0; pi < gi->rootPaths().size(); ++pi)
    {
        Path *p = gi->rootPaths()[pi];
        dumpPath(p);
    }

    delete gi;
}

void boundarytest(std::string filename)
{
    using namespace hexer;

    vector<GridInfo *> infos;
    GridInfo *gi = new GridInfo;

    infos.push_back(gi);
    
    LAS l(filename);
    l.open();
    process(infos, l.reader);

    std::ostringstream multi;
    multi.setf(std::ios::fixed);
    multi.precision(8);
    
    gi->toWKT(multi);

    std::cout << multi.str() << std::endl;

    delete gi;
}

std::string GetFullVersion( void )
{


        std::ostringstream os;

        os << " hexer " 
           << HEXER_VERSION_MAJOR << "."
           << HEXER_VERSION_MINOR << "."
           << HEXER_VERSION_REVISION;
        
        os << " at git revision ", std::string(g_GIT_SHA1,strlen(g_GIT_SHA1));


    #ifdef HEXER_HAVE_GDAL
        os << " with GDAL " << GDALVersionInfo("RELEASE_NAME");
    #endif


        return os.str();
    
}

void OutputHelp( std::ostream & oss, po::options_description const& options)
{
    oss << "--------------------------------------------------------------------" << std::endl;
    oss << "    curse (" << GetFullVersion() << ")" << std::endl;
    oss << "--------------------------------------------------------------------" << std::endl;

    oss << options << std::endl;

    oss <<"For more information, see the full documentation for hexer at:" << std::endl;
    
    oss << " http://github.com/hobu/hexer" << std::endl;
    oss << "----------------------------------------------------------" << std::endl;

}


po::options_description* getOptions()
{

    // po::options_description* basic_options = new po::options_description("basic options");
    // 
    // basic_options->add_options()
    //     ("help,h", po::value<bool>->zero_tokens()->implicit_value(true), "produce help message")
    //     ("debug,d", po::value<bool>->zero_tokens()->implicit_value(true), "Enable debug mode")
    //     ("verbose,v", po::value<boost::uint32_t>->default_value(0), "Set verbose message level")
    //     ("version", po::value<bool>->zero_tokens()->implicit_value(true), "Show version info")
    // ;
    //  

 
    
}

int main(int argc, char* argv[])
{

    
    if (argc < 3)
    {
        std::cerr << "please specify a command and a filename. $ hextest PATH filename.las" << std::endl;
		return 1;
    }
    
    std::string command(argv[1], strlen(argv[1]));
    std::string filename(argv[2], strlen(argv[2]));
    
	try
	{

		if (boost::iequals(command, "BOUNDARY"))
		{
			boundarytest(filename);
			return 0;
		}
    
		if (boost::iequals(command, "PATH"))
		{
			pathtest(filename);
			return 0;
		}

		if (boost::iequals(command, "HEX"))
		{
			hextest(filename);
			return 0;
		}    

		std::cout << "Command '" << command << "' not recognized" << std::endl;
		return 1;
	} catch (hexer::hexer_error const& e)
	{
		std::cout << "Hexer failed with error: '" << e.what() << "'" << std::endl;
		return 1;
	}
    
}
