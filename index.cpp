#include "src/utils.hpp"
#include "include/env/dot_env.h"
#include <cstdlib>

int main(void) {
    env_load("",0);
    int fetchDailyNews = fetchNews(std::getenv("MEDIA_STACK_ACCESS_KEY"));
    if(fetchDailyNews < 0){
        return -1;
    }
    std::cout<<"Succesfully fetched news\n";
}