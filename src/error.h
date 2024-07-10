template<typename T>
void throwNotCriticalError(T what)
{
    std::string time;
    try
    {
        auto end = std::chrono::system_clock::now();
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);
        time=std::ctime(&end_time);
        time.pop_back();
    }
    catch (...)
    {
        time = "[TIME : UNKNOWN]";
    }
    std::cerr<<time<< "\tERROR:\t" << what <<"\tNOT CRITICAL"<< std::endl;
}
template<typename T>
void throwCriticalError(T what)
{
        std::string time;
    try
    {
        auto end = std::chrono::system_clock::now();
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);
        time=std::ctime(&end_time);
        time.pop_back();
    }
    catch (...)
    {
        time = "[TIME : UNKNOWN]";
    }
    std::cerr<<time<<"\tCRITICAL ERROR:\t"<<what<<"\tPROGRAM WILL END NOW"<<std::endl;
    exit(-1);
}