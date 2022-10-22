#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;
using time_point = std::chrono::system_clock::time_point;

std::string serializeTimePoint( const time_point& time, const std::string& format)
{
    std::time_t tt = std::chrono::system_clock::to_time_t(time);
    std::tm tm = *std::gmtime(&tt); //GMT (UTC)
    //std::tm tm = *std::localtime(&tt); //Locale time-zone, usually UTC by default.
    std::stringstream ss;
    ss << std::put_time( &tm, format.c_str() );
    return ss.str();
}

int main(){
    
    fs::path asd(fs::current_path());
    std::cout << asd.string() << std::endl;

    std::string path = asd.string().substr(0, asd.string().find_last_of("/"));

    auto timestamp = std::chrono::system_clock::now();

    std::string currentTime = serializeTimePoint(timestamp, "%Y-%m-%d %H-%M-%S");
    

    fs::create_directory("../mail-spool-directory/test-dir");

    std::ofstream myfile;
    myfile.open ("../mail-spool-directory/test-dir/" + currentTime );
    myfile << "Writing this to a file.\n";
    myfile.close();

    //std::cout << std::filesystem::current_path();


    return 0;
}